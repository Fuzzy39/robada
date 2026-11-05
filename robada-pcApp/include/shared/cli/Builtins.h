#pragma once
#include <cli/Engine.h>

namespace cli::builtins
{
    enum Types
    {
        TOKEN = 0,
        INT,
        DOUBLE,
        LAST
    };

    bool isToken(std::string&);
    void* convertToken(std::string&);

    bool isInt(std::string&);
    void* convertInt(std::string&);    

    bool isDouble(std::string&);
    void* convertDouble(std::string&); 
    
    void registerTypes(cli::Engine& engine);
}