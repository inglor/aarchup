#ifndef AARCHUP_CLIWRAPPER_H
#define AARCHUP_CLIWRAPPER_H

#include <iostream>

class CliWrapper {
    const char *_cliCommand;
public:

    CliWrapper(const char *cliCommand);

    std::string execute();

    virtual ~CliWrapper();
};


#endif
