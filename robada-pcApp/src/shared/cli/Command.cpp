#include <cli/Command.h>
#include <stdexcept>
#include <ostream>

cli::Command::Command(std::string name, std::string description, int minArgs, int maxArgs, CommandFunc onRun): 
    name(name), description(description), 
    minArgs(minArgs), maxArgs(maxArgs), onRun(onRun)
{
    if(onRun==nullptr)
    {
        throw std::invalid_argument("A command's onRun function may not be null.");
    }

    if(minArgs>maxArgs)
    {
        throw std::invalid_argument("minArgs must be less than or equal to maxArgs.");
    }
}

// function to do command
bool cli::Command::run(std::vector<std::string>args, std::ostream& out)
{
    bool toReturn = checkArgs(args.size(), out);

    if(toReturn) toReturn = onRun(args, out);
    if(!toReturn)
    {
        // print help text if the command fails.
        out <<"Help for "<<name<<": "<< description <<"\n";
    }
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


const std::string& cli::Command::getName() const
{
    return name;
}

const std::string& cli::Command::getDescription() const
{
    return description;
}
