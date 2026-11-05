#include <cli/command/CommandBase.h>

namespace cli
{
    // a pretty lame implementation, if you ask me...
    CommandBase::CommandBase(std::string name, std::string description)
        : name(name), description(description){}

    void CommandBase::printHelp(std::ostream& out)
    {
        out << "Help for " << name << ": " << description << "\n";
    }

    const std::string& CommandBase::getName() const
    {
        return name;
    }

    const std::string& CommandBase::getDescription() const 
    {
        return description;
    }
}