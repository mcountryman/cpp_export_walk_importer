//
// Created by marvinc on 2/17/2020.
//
#include <locale>
#include <string>
#include <codecvt>
#include <Windows.h>
#include <winternl.h>

#include "import_resolver.hpp"

PEB *getProcessExecutionBlock() {
#ifdef _M_IX86
  return (PEB *) __readfsdword(0x30);
#elif _M_AMD64
  return (PEB *)__readgsqword(0x60);
#else
#error unsupported architecture
#endif
}

bool isEqualIgnoreCase(std::basic_string<char> a, UNICODE_STRING b) {
  auto bBuffer = b.Buffer;
  auto bLength = b.Length / 2;
  auto bLengthDelta = bLength - a.length();

  // Validate input
  if (bLength < 0) return false;
  if (bBuffer == nullptr) return false;
  if (bLengthDelta < 0) return false;

  // Check characters
  for (auto i = 0; i < a.length(); i++)
    if (std::tolower(a[i]) != std::tolower(bBuffer[i + bLengthDelta]))
      return false;

  return true;
}

void resolveImportsForLibrary(std::map<std::string, std::shared_ptr<Import>> imports, LDR_DATA_TABLE_ENTRY *library) {
  auto base = reinterpret_cast<std::uint8_t *>(library->DllBase);
  auto dosHeader = reinterpret_cast<IMAGE_DOS_HEADER *>(base);
  if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    throw std::runtime_error("Bad image format = invalid `IMAGE_DOS_HEADER` magic");

  auto ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS *>(base + dosHeader->e_lfanew);
  if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
    throw std::runtime_error("Bad image format = invalid `IMAGE_NT_HEADERS` magic");

  auto exportDirectoryAddrRel = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
  auto exportDirectory = reinterpret_cast<IMAGE_EXPORT_DIRECTORY *>(base + exportDirectoryAddrRel.VirtualAddress);

  auto exportNames = reinterpret_cast<std::uintptr_t *>(base + exportDirectory->AddressOfNames);
  auto exportFunctions = reinterpret_cast<std::uintptr_t *>(base + exportDirectory->AddressOfFunctions);
  auto exportNameOrdinals = reinterpret_cast<std::uint16_t *>(base + exportDirectory->AddressOfNameOrdinals);

  for (auto i = 0; i < exportDirectory->NumberOfNames; i++) {
    auto exportName = std::string(reinterpret_cast<char *>(base + exportNames[i]));
    auto import = imports.find(exportName);
    if (import == imports.end())
      continue;

    auto exportNameOrdinal = exportNameOrdinals[i];
    auto exportFunction = exportFunctions[exportNameOrdinal];

    // Check forwarded name
    if (
      exportFunction > exportDirectoryAddrRel.VirtualAddress &&
      exportFunction < exportDirectoryAddrRel.VirtualAddress + exportDirectoryAddrRel.Size) {

      throw std::runtime_error("Forwarded exports not supported!");
    }

    import->second->Resolve(reinterpret_cast<void *>(base + exportFunction));
    imports.erase(import);
  }

  if (!imports.empty()) {
    throw std::runtime_error("Not all imports found!");
  }
}

void ImportResolver::Resolve() {
  auto peb = getProcessExecutionBlock();
  auto link = peb->Ldr->InMemoryOrderModuleList.Flink;
  auto firstLink = link;
  auto imports = _imports;

  do {
    auto entry = CONTAINING_RECORD(link, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

    for (auto &importsByLibrary : _imports) {
      if (isEqualIgnoreCase(importsByLibrary.first, entry->FullDllName)) {
        resolveImportsForLibrary(importsByLibrary.second, entry);
      }
    }

    link = link->Flink;
  } while (link != firstLink);
}

#include "import_resolver.hpp"
