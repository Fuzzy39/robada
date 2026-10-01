#pragma once
#include <string>
#include <vector>
namespace cli
{
    class Command
    {
        typedef bool(*CommandFunc)(std::vector<std::string>);

    private:
        std::string& name;
        std::string& description; // one line description for help documentation.
        CommandFunc onRun; // command implementation. returns success. On failure, prints description.
    
    public:
        Command(std::string& name, std::string& description, CommandFunc onRun);

        // function to do command
        virtual bool run(std::vector<std::string>args(), std::ostream out);
    };
}