#pragma once
#include <cli/HasHelpInfo.h>
#include <functional>


namespace cli
{

    // CommandBase is an abstract class that represents something that the CLI engine can execute.
    class CommandBase : public HasHelpInfo
    {
    
    public:
        using HasHelpInfo::HasHelpInfo;

        virtual bool run(std::vector<std::string>args, std::ostream& out) = 0;
        
    };
}