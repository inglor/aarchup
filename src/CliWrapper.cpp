#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include "CliWrapper.hpp"


CliWrapper::CliWrapper(const char *cliCommand) {
    this->_cliCommand = cliCommand;
}

CliWrapper::~CliWrapper() = default;

std::string CliWrapper::execute() {
    std::array<char, 128> buffer = {0};
    std::string result;
    std::shared_ptr<FILE> pipe(popen(CliWrapper::_cliCommand, "r"), pclose);
    if (!pipe) {
        std::stringstream ss;
        ss << "Failed to execute command " << _cliCommand;
        throw std::runtime_error(ss.str());
    }
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }
    return result;
}
