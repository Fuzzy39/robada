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

    class TokenType : public ArgumentType
    {
    public:
        TokenType();

        virtual bool CheckStringIsOfType(std::string token);

        virtual void* ConvertStringToType(std::string token);

        virtual void DeleteValue(void* value);
    };
    
    class IntType : public ArgumentType
    {
    public:
        IntType();

        virtual bool CheckStringIsOfType(std::string token);

        virtual void* ConvertStringToType(std::string token);

        virtual void DeleteValue(void* value);
    };
    
    class DoubleType : public ArgumentType
    {
    public:
        DoubleType();

        virtual bool CheckStringIsOfType(std::string token);

        virtual void* ConvertStringToType(std::string token);

        virtual void DeleteValue(void* value);
    };

    void registerTypes(cli::Engine& engine);
}