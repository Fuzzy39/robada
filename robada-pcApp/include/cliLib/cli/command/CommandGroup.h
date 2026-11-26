#pragma once
#include <cli/command/CommandBase.h>
#include <cli/command/Command.h>
#include <string>
#include <memory>
#include <ostream>
#include <vector>

namespace cli
{

    class CommandGroup : public CommandBase
    {
    private:
        std::vector<std::unique_ptr<CommandBase>> subCommands;
        bool isRoot; // whether this is the root command of an engine
        
        // function to do command is overwritten.
    public:
        CommandGroup(std::string name, bool isRoot = false);
        CommandGroup* setDescription(std::string description);

        // This class takes ownership of the command.
        void addCommand(CommandBase* command);

        bool run(std::vector<std::string>args, std::ostream& out) override;
        void printHelp(std::ostream& out) override;

 
        CommandBase* findSubCommand(std::string name);
        bool commandExists(cli::CommandBase* command); // check whether command exists as any child of this command (or child of child, etc.)
    };
}