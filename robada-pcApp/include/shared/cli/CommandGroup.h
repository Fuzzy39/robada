#pragma once
#include "Command.h"
#include <string>
#include <memory>
#include <ostream>

namespace cli
{
    class CommandGroup : public Command
    {
    private:
        std::vector<std::unique_ptr<Command>> subCommands;
        bool isRoot; // whether this is the root command of an engine
        
        // function to do command is overwritten.
    public:
        CommandGroup(std::string name, std::string description, int minArgs, int maxArgs, CommandFunc onRun, bool isRoot = false);

        // This class takes ownership of the command.
        void addCommand(Command* command);

        bool run(std::vector<std::string>args, std::ostream& out);
        void printHelp(std::ostream& out);

 
        Command* findSubCommand(std::string name);
        bool commandExists(cli::Command* command); // check whether command exists as any child of this command (or child of child, etc.)
    };
}