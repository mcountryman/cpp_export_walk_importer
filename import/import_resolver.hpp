//
// Created by marvinc on 2/17/2020.
//
#ifndef CPPTEST_IMPORT_RESOLVER_HPP
#define CPPTEST_IMPORT_RESOLVER_HPP

#include <map>
#include <string>
#include <memory>
#include <future>
#include <functional>

#include "import.hpp"

class ImportResolver {
public:
  std::shared_ptr<Import> GetImport(const std::string &library, const std::string &procedure) {
    auto import = std::make_shared<Import>();
    _imports[library][procedure] = import;
    return import;
  }

  void Resolve();

private:
  std::map<std::string, std::map<std::string, std::shared_ptr<Import>>> _imports;
};


#endif //CPPTEST_IMPORT_RESOLVER_HPP
