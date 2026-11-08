#include <cli/Builtins.h>
namespace cli::builtins
{
    // token/string type
    TokenType::TokenType(): ArgumentType("arbitrary", "Any arbitrary value."){}

    bool TokenType::CheckStringIsOfType(std::string token)
    {
        return true;
    }

    void* TokenType::ConvertStringToType(std::string token)
    {
        return new std::string(token); 
    }

    void TokenType::DeleteValue(void* value)
    {
        delete static_cast<std::string*>(value);
    }

    // Integer type
    IntType::IntType(): ArgumentType("integer", "Any integer."){}

    bool IntType::CheckStringIsOfType(std::string token)
    {
        try
        {
            size_t chars;
            std::stoi(token, &chars);
            if(chars<token.size()) return false;
        }
        catch(const std::invalid_argument& e)
        {
            return false;
        }

        return true;
    }

    void* IntType::ConvertStringToType(std::string token)
    {
      
        return new int(std::stoi(token));
    }   

    void IntType::DeleteValue(void* value)
    {
        delete static_cast<int*>(value);
    }

    // Double type
    DoubleType::DoubleType(): ArgumentType("number", "Any number."){}

    bool DoubleType::CheckStringIsOfType(std::string token)
    {
        try
        {
           std::stod(token);
        }
        catch(const std::invalid_argument& e)
        {
            return false;
        }
        return true;
    }

    void* DoubleType::ConvertStringToType(std::string token)
    {
        return new double(std::stod(token));
    }   

    void DoubleType::DeleteValue(void* value)
    {
        delete static_cast<double*>(value);
    }


    void registerTypes(cli::Engine& engine)
    {
       
        engine.addType(TOKEN, new TokenType()); 
        engine.addType(INT, new IntType()); 
        engine.addType(DOUBLE, new DoubleType());
    }
}