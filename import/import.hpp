//
// Created by marvinc on 2/17/2020.
//

#ifndef CPPTEST_IMPORT_HPP
#define CPPTEST_IMPORT_HPP

#include <string>
#include <utility>
#include <exception>

class Import {
public:
  Import() {
    _pointer = nullptr;
    _hasResolved = false;
  };
public:
  bool hasResolved() { return _hasResolved; }

public:
  template<class TRet, class... TArgs>
  TRet InvokeStdCall(TArgs... args) {
    if (!_hasResolved) {
      throw new std::runtime_error("Import has not been resolved!");
    }

    auto fn = reinterpret_cast<TRet(__stdcall *)(TArgs...)>(_pointer);
    return fn(args...);
  }

  void Resolve(void *pointer) {
    _pointer = pointer;
    _hasResolved = true;
  }

private:
  void *_pointer;
  bool _hasResolved;
};

#endif //CPPTEST_IMPORT_HPP
