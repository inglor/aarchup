#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include "CliWrapper.h"


CliWrapper::CliWrapper(const char *cliCommand) {
    this->_cliCommand = cliCommand;
}

CliWrapper::~CliWrapper() = default;

std::string CliWrapper::execute() {
    std::array<char, 128> buffer = {0};
    std::string result;
    std::shared_ptr<FILE> pipe(popen(CliWrapper::_cliCommand, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }
    return result;
}
