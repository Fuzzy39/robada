#include <cli/CommandGroup.h>


void cli::CommandGroup::addCommand(Command* command)
{
    subCommands.push_back(std::unique_ptr<Command>(command));
}

bool cli::CommandGroup::run(std::vector<std::string>args, std::ostream& out)
{
    // First, try to find a subcommand that fits the bill.
    cli::Command* subCommand = findSubCommand(args);
    if(subCommand != nullptr)
    {
        // run the subcommand.
       return subCommand->run(std::vector<std::string>(args.begin()+1, args.end()), out);
    }

    // Failing that, we run ourselves.
    bool toReturn = checkArgs(args.size(), out);
    if(toReturn) toReturn = onRun(args, out);
    if (toReturn) return true;

    // print a help message.
    out << description << "\nCommand Options:\n";
    for(std::unique_ptr<Command>& command : subCommands )
    {
        out << name <<" "<< command->getName()<<"\n";
    }

    return false;
}

cli::Command* cli::CommandGroup::findSubCommand(std::vector<std::string>args)
{
    if(args.size() == 0) return nullptr;

    for(std::unique_ptr<Command>& command : subCommands )
    {
        if (command->getName() == args[0])
        {
            return command.get();
        }
    }
    
    return nullptr;
}
