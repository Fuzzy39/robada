#pragma once
#include <cli/command/CommandBase.h>
#include <string>
#include <vector>
#include <functional>


namespace cli
{
    class Command : public CommandBase
    {
       

    protected:
        int minArgs;    // -1 to mean no min
        int maxArgs;    // -1 to mean no max.
        CommandFunc onRun; // command implementation. returns success. On failure, prints description.

    public:
        Command(std::string name, std::string description, int minArgs, int maxArgs, CommandFunc onRun);

        // function to do command
        bool run(std::vector<std::string>args, std::ostream& out) override;

    protected:
        bool checkArgs(size_t argsCount, std::ostream& out) const;
    };
}