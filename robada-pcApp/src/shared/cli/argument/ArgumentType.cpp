#include <cli/argument/ArgumentType.h>

namespace cli
{
    ArgumentType::ArgumentType(std::string name, ArgIsOfTypeFunc& checkType, ConvertArgFunc& convertArg)
        : HasHelpInfo(name), checkType(checkType), convertArg(convertArg)
    {

    }

    ArgumentType* ArgumentType::setDescription(std::string description)
    {
        this->description = description;
        return this;
    }

    bool ArgumentType::CheckStringIsOfType(std::string token)
    {
        return checkType(token);
    }

    void* ArgumentType::ConvertStringToType(std::string token)
    {
        return convertArg(token);
    }

    void ArgumentType::printHelp(std::ostream& out)
    {
        out << "Type "<<name<<": "<<description<<"\n";
    }

}