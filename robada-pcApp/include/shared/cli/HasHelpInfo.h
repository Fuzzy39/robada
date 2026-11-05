#pragma once
#include <ostream>

namespace cli
{
    // HasHelpInfo is an interface that represents something that can be printed out by the help command in the CLI.
    class HasHelpInfo
    {
    public:
        virtual void printHelp(std::ostream& out) = 0;

        virtual const std::string& getName() const = 0;
        virtual const std::string& getDescription() const = 0;

        // returns a single line description. By default, this is the first line of getDescription.
        virtual const std::string getBriefDescription() const; 

    };
}