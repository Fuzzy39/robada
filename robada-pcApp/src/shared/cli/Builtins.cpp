#include <cli/Builtins.h>
namespace cli::builtins
{
    bool isToken(std::string& token)
    {
        return true;
    }

    void* convertToken(std::string& token)
    {
        return new std::string(token); 
    }

    bool isInt(std::string& token)
    {
        try
        {
            std::stoi(token);
        }
        catch(const std::invalid_argument& e)
        {
            return false;
        }

        return true;
    }
    void* convertInt(std::string& token)
    {
        return new int(std::stoi(token));
    }   

    bool isDouble(std::string& token)
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

    void* convertDouble(std::string& token)
    {
        return new double(std::stod(token));
    }
    
    void registerTypes(cli::Engine& engine)
    {
        ArgIsOfTypeFunc is = isToken;
        ConvertArgFunc conv = convertToken;
        engine.addType(TOKEN, "arbitrary", is, conv)
            ->setDescription("Any arbitrary value."); 
        
        is = isInt;
        conv = convertToken;
        engine.addType(INT, "integer", is, conv)
            ->setDescription("Any integer.");

        is = isDouble;
        conv = convertDouble;
        engine.addType(DOUBLE, "number", is, conv)
            ->setDescription("Any number.");
    }
}