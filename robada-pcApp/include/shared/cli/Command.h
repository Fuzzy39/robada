#pragma once
#include <string>
#include <vector>
namespace cli
{
    typedef bool(*CommandFunc)(std::vector<std::string>, std::ostream&);
    class Command
    {
       

    protected:
        const std::string name;
        const std::string description; // one line description for help documentation.
        int minArgs;    // -1 to mean no min
        int maxArgs;    // -1 to mean no max.
        CommandFunc onRun; // command implementation. returns success. On failure, prints description.

    public:
        Command(std::string name, std::string description, int minArgs, int maxArgs, CommandFunc onRun);

        // function to do command
        virtual bool run(std::vector<std::string>args, std::ostream& out);

        const std::string& getName() const;
        const std::string& getDescription() const;

    protected:
        bool checkArgs(size_t argsCount, std::ostream& out) const;
    };
}