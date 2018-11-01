#ifndef AARCHUP_CLIWRAPPER_H
#define AARCHUP_CLIWRAPPER_H

#include <iostream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>

class CliWrapper {
    const char *_cliCommand;
public:

    CliWrapper(const char *cliCommand);

    std::string execute();

    virtual ~CliWrapper();

    std::string parseOutput(const std::shared_ptr<FILE> &pipe) const;
};


#endif
