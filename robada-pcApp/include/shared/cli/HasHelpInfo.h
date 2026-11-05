#pragma once
#include <ostream>

namespace cli
{
    // HasHelpInfo is an interface that represents something that can be printed out by the help command in the CLI.
    class HasHelpInfo
    {
    protected:
        std::string name;
        std::string description; // one or more line description for help documentation.

    public:
        HasHelpInfo(std::string name, std::string description);
        HasHelpInfo(std::string name);

        virtual void printHelp(std::ostream& out) = 0;

        virtual const std::string& getName() const;
        virtual const std::string& getDescription() const;

        // returns a single line description. By default, this is the first line of getDescription.
        virtual const std::string getBriefDescription() const; 

    };
}