#include <cli/command/CommandBase.h>

namespace cli
{
    // a pretty lame implementation, if you ask me...
    CommandBase::CommandBase(std::string name, std::string description)
        : HasHelpInfo(name, description) {}

    void CommandBase::printHelp(std::ostream& out)
    {
        out << "Help for " << name << ": " << description << "\n";
    }

}