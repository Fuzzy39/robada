#pragma once
#include <cli/command/CommandBase.h>
#include <cli/argument/Argument.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>





namespace cli
{
    typedef std::map<const std::string, void*> UserArgs;
    typedef std::function<bool(UserArgs, std::ostream&)> CommandFunc;


    class Command : public CommandBase
    {
       

    protected:

        TypeRegistry& typeRegistry;
        CommandFunc onRun; // command implementation. returns success. On failure, prints description.

        int minArgs;
        std::vector<std::unique_ptr<cli::Argument>> arguments;
        std::unique_ptr<cli::Argument> variadicArgument; // null if no vararg

    public:
        Command(TypeRegistry& typeRegistry, std::string name);
    
        // setup commands   
        Command* setDescription(std::string description);

        Command* setImplementation(const CommandFunc& command);

        // add an argument with a name and description.
        Command* addArgument(std::string name, std::string description, ArgTypeId type);

        // optional arguments will be added at the end of the argument list.
        Command* addOptionalArgument(std::string name, std::string description, ArgTypeId type); 

        // Add a variadic argument to the command, allowing a command to have potentially unlimited arguments.
        // These arguments go last in userargs and are returned as a vector<void*> with the given name.
        // A command can only have a single variadic argument.
        Command* addVariadicArgument(std::string name, std::string description, ArgTypeId type); 

        // function to do command
        bool run(std::vector<std::string>args, std::ostream& out) override;

        virtual void printHelp(std::ostream& out) override;

    protected:
        void verifyArgumentValid(std::string name, ArgTypeId type);

        // Argument Handling:
        // may return a nullptr if arguments are invalid.
        std::unique_ptr<UserArgs> parseArguments(std::vector<std::string> rawArguments, std::ostream& out) const;
        
        bool checkArgumentsSize(std::vector<std::string> rawArguments, std::ostream& out) const;

        bool parseVariadicArguments(UserArgs* toReturn, std::vector<std::string> rawArguments, std::ostream& out) const;

        void deleteArgumentValues(UserArgs&) const;

        static bool defaultCommand(UserArgs args, std::ostream& out);
    };  
}

#define COMMAND [](cli::UserArgs args, std::ostream& out)->bool