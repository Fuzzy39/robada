#include <cli/CommandGroup.h>
#include <sstream>

// Too many arguments in my constructor?
// I mean, yeah.
cli::CommandGroup::CommandGroup
    (std::string name, std::string description, int minArgs, int maxArgs, CommandFunc onRun, bool isRoot)
    : cli::Command(name, description, minArgs, maxArgs, onRun), isRoot(isRoot) {}


void cli::CommandGroup::addCommand(Command* command)
{
    // check that a command with this name doesn't already exist.
    for(std::unique_ptr<cli::Command>& com : subCommands)
    {
        if(command->getName()!=com->getName()) continue;
        
        std::ostringstream error;
        error<<"Can't add another sub-command with the name "<<com->getName()<<" to Command "<<getName()<<".";
        throw std::invalid_argument(error.str());
    }

    subCommands.push_back(std::unique_ptr<Command>(command));
}

bool cli::CommandGroup::run(std::vector<std::string>args, std::ostream& out)
{
    
    // First, try to find a subcommand that fits the bill.
    if (args.size() != 0)
    {
        cli::Command* subCommand = findSubCommand(args[0]);
        if (subCommand != nullptr)
        {
            // run the subcommand.
            return subCommand->run(std::vector<std::string>(args.begin() + 1, args.end()), out);
        }
    }

    // Failing that, we run ourselves.
    bool toReturn = checkArgs(args.size(), out);


    if(toReturn) toReturn = onRun(args, out);
    if (toReturn) return true;


    // print a help message.
    if(args.size()>0) out <<"Unrecognized command '"<<args[0]<<"'.\n";
    printHelp(out);

    return false;
}

cli::Command* cli::CommandGroup::findSubCommand(std::string name)
{

    for(std::unique_ptr<Command>& command : subCommands)
    {
        if (command->getName() == name)
        {
            return command.get();
        }
    }
    
    return nullptr;
}

bool cli::CommandGroup::commandExists(cli::Command* toFind)
{
    // yes, we exist!
    if(toFind == this) return true;

    // Failing that, do a depth first search I guess?
    for(std::unique_ptr<Command>&  command: subCommands)
    {
        if ( command.get() == toFind) return true;

        // if the command we're looking at is a group, search it as well.
        cli::CommandGroup* group = dynamic_cast<cli::CommandGroup*>(command.get());
        if (group != nullptr && group->commandExists(toFind)) return true;
      
    }
            
    return false;
}

void cli::CommandGroup::printHelp(std::ostream& out)
{
    out << "Help for " << name << ": " << description << "\nCommands:\n";
    for (std::unique_ptr<Command>& command : subCommands)
    {
        if (isRoot)
        {
            out << "\t" << command->getName() << " - " << command->getBriefDescription() << "\n";
            continue;
        }
        out << "\t" << name << " " << command->getName() << " - " << command->getBriefDescription() << "\n";
    }
}