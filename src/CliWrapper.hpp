#ifndef AARCHUP_CLIWRAPPER_H
#define AARCHUP_CLIWRAPPER_H

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

class CliWrapper {
  const char *_cliCommand;

 public:
  CliWrapper(const char *cliCommand);

  std::string execute();

  virtual ~CliWrapper();

  std::string parseOutput(const std::shared_ptr<FILE> &pipe) const;
};

#endif
