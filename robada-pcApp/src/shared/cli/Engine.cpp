#include <cli/Engine.h>
#include <istream>
#include <ostream>
#include <cstring>

cli::Engine::Engine(std::istream& input, std::ostream& output)
    : input(input), output(output), 
      root(ROOT_NAME, "Software for controlling the 3-axis robot Robada.\n\"exit\" or \"quit\" to exit.", -1, -1, COMMAND{return false;})
{
    implementCommands();
    implementHelp();

    output << "Welcome to Robada CLI! 'help' for help.\n";
    output << "> ";
}

std::string& trim(std::string& s)
{
    return s.erase(s.find_last_not_of(" \n\r\t") + 1).erase(0, s.find_first_not_of(" \n\r\t"));
}

bool cli::Engine::interpret()
{
    if (parseCommand())
    {
        std::cout << "\n> ";
        return true;
    }
    
    return false;
}

bool cli::Engine::parseCommand()
{
    const size_t MAX_LINE_LEN = 1000;
    char buffer[MAX_LINE_LEN];
    
    input.getline(buffer, MAX_LINE_LEN, '\n');
    
    if(strlen(buffer)==MAX_LINE_LEN-1)
    {
        output<<"Command input exceeded maximum length.\n";
        return true;
    }

    // convert to lowercase.
    for(int i = 0; i<strlen(buffer); i++)
    {
        buffer[i] = std::tolower(buffer[i]);
    }


    std::string line(buffer);
    line = trim(line);

    if(line.empty())
    {
        // carry on with life if it's empty.
        return true;
    }
    
    // quit the program?
    if(line == "exit" || line == "quit")
    {
        return false;
    }

    // Okay, now we need to construct the argument list.
    std::vector<std::string> args;
    while(!line.empty())
    {
        size_t pos = line.find_first_of(" \n\r\t");
        if(pos == std::string::npos)
        {
           
            args.push_back(line);
            line = "";
            continue;
        }

        std::string token = std::string(line.begin(), line.begin()+pos);
        args.push_back(token);
        line = trim(line.erase(0, pos));
    }

    // now that we have them, we call our command group.
    root.run(args, output);
    return true;
}

void cli::Engine::implementHelp()
{
    // help is a fancy special command.
    root.addCommand(new Command("help", "Get information about a command.\nUse arguments to get information about a specific command.", 0, -1,
        [&](std::vector<std::string> args, std::ostream& out)->bool
        {
            // we use the args as a list to find a command of that name and print the help for it.
            CommandGroup* current = &root;

            for (int i = 0; i<args.size(); i++)
            {
                std::string& commandName = args[i];

                Command* next = current->findSubCommand(commandName);
                if (next == nullptr)
                {
                    out << "Couldn't find command '" << commandName << "'.\n";
                    current->printHelp(out);
                    return true;
                }

                current = dynamic_cast<CommandGroup*>(next);
                if (current == nullptr)
                {
                    if (i != args.size() - 1)
                    {
                        out << "'"<<commandName << "' Does not have subcommands.\n";
                    }

                    next->printHelp(out);
                    return true;
                }
            }

            current -> printHelp(out);
            return true;
        }));
}