#include <cli/argument/ArgumentType.h>

namespace cli
{
    ArgumentType::ArgumentType(std::string name, ArgIsOfTypeFunc& checkType, 
            ConvertArgFunc& convertArg, DeleteValueFunc& deleteValue)
        : HasHelpInfo(name), checkType(checkType), convertArg(convertArg), deleteValue(deleteValue)
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

    void ArgumentType::DeleteValue(void* value)
    {
        return deleteValue(value);
    }

    void ArgumentType::printHelp(std::ostream& out)
    {
        out << "Type "<<name<<": "<<description<<"\n";
    }

}