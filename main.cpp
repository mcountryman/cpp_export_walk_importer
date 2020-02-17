#include <chrono>
#include <memory>
#include <iostream>
#include <Windows.h>

#include "import/import_resolver.hpp"

int main() {
  using namespace std::chrono;

  ImportResolver resolver;

  resolver.GetImport("kernel32.dll", "GetStdHandle");
  resolver.GetImport("kernel32.dll", "WriteConsoleA");
  resolver.GetImport("kernel32.dll", "BaseDumpAppcompatCache");
  resolver.GetImport("ntdll.dll", "EtwNotificationRegister");
  resolver.GetImport("kernelbase.dll", "Beep");

  auto begin = high_resolution_clock::now();
  resolver.Resolve();
  auto end = high_resolution_clock::now();
  auto span = duration_cast<duration<double>>(end - begin);

  std::cout << "Took: " << span.count() * 1000 << "ms" << std::endl;
}
