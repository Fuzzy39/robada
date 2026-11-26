#include <cli/argument/Argument.h>

namespace cli
{
    Argument::Argument(std::string name, std::string description, ArgumentType& type) 
        : HasHelpInfo(name, description), type(type)
    {

    }

    // Main functions
    ArgumentType& Argument::getType()
    {
        return type;
    }
    
    // parse the argument from a string.
    // If parsing fails, an error message will be printed and nullptr is returned.
    // Otherwise, a pointer to a the argument is passed. the user function is responsible for 
    // deleting the result.
    void* Argument::tryGetArgument(const std::string& commandName, std::string& token, std::ostream& out)
    {
        if(!type.CheckStringIsOfType(token))
        {
            out << name <<" argument of "<<commandName<<" must be a(n) " << type.getName()<<".\n";
            return nullptr;
        }

        return type.ConvertStringToType(token);
    }

    void Argument::printHelp(std::ostream& out)
    {
        out << name << " (" << type.getName() << "): " << description;
    }
}