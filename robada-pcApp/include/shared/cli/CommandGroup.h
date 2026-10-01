#pragma once
#include <string>
#include "Command.h"
#include <memory>

namespace cli
{
    class CommandGroup : public Command
    {
    private:
        std::vector<std::unique_ptr<Command>> subCommands;
        
        // function to do command is overwritten.
    public:
        // constructor, add/remove commands maybe. Implementation of command.

        // This class takes ownership of the command.
        void addCommand(Command* command);

        bool run(std::vector<std::string>args(), std::ostream out);
    };
}