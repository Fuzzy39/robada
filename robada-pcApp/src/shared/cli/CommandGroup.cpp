#include <cli/CommandGroup.h>

const std::string cli::ROOT_NAME = "Robada CLI";

void cli::CommandGroup::addCommand(Command* command)
{
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


void cli::CommandGroup::printHelp(std::ostream& out)
{
    out << "Help for " << name << ": " << description << "\nCommands:\n";
    for (std::unique_ptr<Command>& command : subCommands)
    {
        if (name == ROOT_NAME)
        {
            out << "  " << command->getName() << " - " << command->getBriefDescription() << "\n";
            continue;
        }
        out << "  " << name << " " << command->getName() << " - " << command->getBriefDescription() << "\n";
    }
}