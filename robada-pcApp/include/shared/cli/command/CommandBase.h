#pragma once
#include <cli/HasHelpInfo.h>
#include <functional>


#define COMMAND [](std::vector<std::string> args, std::ostream& out)->bool


namespace cli
{
    typedef std::function<bool(std::vector<std::string>, std::ostream&)> CommandFunc;

    // CommandBase is an abstract class that represents something that the CLI engine can execute.
    class CommandBase : public HasHelpInfo
    {
    protected:
        const std::string name;
        const std::string description; // one or more line description for help documentation.
    
    public:
        CommandBase(std::string name, std::string description);

        virtual bool run(std::vector<std::string>args, std::ostream& out) = 0;

        virtual void printHelp(std::ostream& out) override;
        virtual const std::string& getName() const override;
        virtual const std::string& getDescription() const override;
    };
}