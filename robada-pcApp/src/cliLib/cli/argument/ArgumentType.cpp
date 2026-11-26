#include <cli/argument/ArgumentType.h>
#include <iostream>

namespace cli
{
    ArgumentType::ArgumentType(std::string name): HasHelpInfo(name){}
  
    ArgumentType::ArgumentType(std::string name, std::string description): HasHelpInfo(name, description){}

    ArgumentType* ArgumentType::setDescription(std::string description)
    {
        this->description = description;
        return this;
    }

    void ArgumentType::printHelp(std::ostream& out)
    {
        out << "Type "<<name<<": "<<description<<"\n";
    }

}