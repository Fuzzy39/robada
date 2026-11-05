#pragma once
#include <cli/HasHelpInfo.h>
#include <cli/argument/ArgumentType.h>

namespace cli
{
    // Represents a command's argument.
    class Argument : public HasHelpInfo
    {
    private:
        ArgumentType& type;
    
    public:
        Argument(std::string name, ArgumentType& type);

        // configuration functions
        Argument* setDescription(std::string description);
        
        // Main functions
        ArgumentType& getType();
        
        // parse the argument from a string.
        // If parsing fails, an error message will be printed and nullptr is returned.
        // Otherwise, a pointer to a the argument is passed. the user function is responsible for 
        // deleting the result.
        void* tryGetArgument(const std::string& commandName, std::string& token, std::ostream& out);

        virtual void printHelp(std::ostream& out) override;
      

    };
}