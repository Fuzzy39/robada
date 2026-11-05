#pragma once
#include <cli/command/CommandBase.h>
#include <cli/argument/Argument.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>





namespace cli
{
    typedef std::map<const std::string&, void*> UserArgs;
    typedef std::function<bool(UserArgs, std::ostream&)> CommandFunc;


    class Command : public CommandBase
    {
       

    protected:
        int minArgs;    // -1 to mean no min
        CommandFunc onRun; // command implementation. returns success. On failure, prints description.
        std::vector<std::unique_ptr<cli::Argument>> arguments;
        TypeRegistry& typeRegistry;

    public:
        Command(TypeRegistry& typeRegistry, std::string name, CommandFunc onRun);

        // setup commands   
        Command* setDescription(std::string description);
        Argument* addArgument(std::string name, ArgTypeId type);
        // optional arguments will be added at the end of the argument list.
        Argument* addOptionalArgument(std::string name, ArgTypeId type);


        // function to do command
        bool run(std::vector<std::string>args, std::ostream& out) override;

        virtual void printHelp(std::ostream& out) override;

    protected:
        void verifyArgumentValid(std::string name, ArgTypeId type);

        // may return a nullptr if arguments are invalid.
        std::unique_ptr<UserArgs> parseArguments(std::vector<std::string> args, std::ostream& out) const;
    };  
}

#define COMMAND [](UserArgs args, std::ostream& out)->bool