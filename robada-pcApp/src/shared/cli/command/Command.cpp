#include <cli/command/Command.h>
#include <stdexcept>
#include <ostream>

cli::Command::Command(std::string name, std::string description, int minArgs, int maxArgs, CommandFunc onRun)
    : CommandBase(name, description), minArgs(minArgs), maxArgs(maxArgs), onRun(onRun)
{
    if(onRun==nullptr)
    {
        throw std::invalid_argument("Command::Command: A command's onRun function may not be null.");
    }

    if(minArgs>=0 && maxArgs>=0 && minArgs>maxArgs)
    {
        throw std::invalid_argument("Command::Command: minArgs must be less than or equal to maxArgs.");
    }
}

// function to do command
bool cli::Command::run(std::vector<std::string>args, std::ostream& out)
{
    bool toReturn = checkArgs(args.size(), out);

    if(toReturn) toReturn = onRun(args, out);
    
    // print help text if the command fails.
    if (!toReturn) printHelp(out);
    return toReturn;
}



bool cli::Command::checkArgs(size_t argsCount, std::ostream& out) const
{
    if(minArgs>=0 && argsCount<minArgs)
    {
        out << "Too few arguments for "<< name <<". Expected at least "<<minArgs<<".\n";   
        return false;
    }

    if(maxArgs>=0 && argsCount>maxArgs)
    {
        out << "Too many arguments for "<< name <<". Expected at most "<<maxArgs<<".\n";  
        return false; 
    }

    return true;
}
