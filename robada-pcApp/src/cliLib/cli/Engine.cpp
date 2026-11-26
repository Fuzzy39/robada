#include <cli/Engine.h>
#include <cli/Builtins.h>
#include <istream>
#include <ostream>
#include <cstring>
#include <sstream>

namespace cli
{

    cli::Engine::Engine(std::istream& input, std::ostream& output, std::string cliName, std::string briefDescription)
        : input(input), output(output), typeRegistry(),
        root(cliName, true)
    {
        initialized = false;
        root.setDescription(briefDescription);

        builtins::registerTypes(*this);
    }

    cli::ArgumentType* Engine::addType(ArgTypeId id,  ArgumentType* type)
    {
        if(initialized) throw std::logic_error("Engine::addType: Can't add commands after initialization.");

        // first check the type doesn't already exist.
        if(typeRegistry.find(id)!=typeRegistry.end())
        {
            std::ostringstream error;
            error<<"Engine::addType: type with id "<< id <<"' already exists.\n";
            throw std::invalid_argument(error.str());
        }

        typeRegistry.insert({id, std::unique_ptr<ArgumentType>(type)});
        return typeRegistry[id].get();
    }



    cli::Command* Engine::addCommand(cli::CommandGroup* parent, std::string name)
    {
        if(initialized) throw std::logic_error("Engine::addCommand: Can't add commands after initialization.");

        if(parent == nullptr) parent = &root; // root is the parent if this is a root level command. checks out.

        // check that the parent is known to the engine.
        if(!root.commandExists(parent)) // root is part of root, so this will return true in that case.
        {
            std::ostringstream error;
            error<<"Engine::addCommand: parent '"<< parent->getName() <<"' is not a command known to this engine.\n";
            throw std::invalid_argument(error.str());
        }
        
        // okay, add it
        // Now, probably this should be a unique pointer from the start but I'm being lazy here.
        Command* toReturn = new Command(typeRegistry, name); 
        parent->addCommand(toReturn);
        return toReturn;
    }

    cli::CommandGroup* Engine::addCommandGroup(cli::CommandGroup* parent, std::string name)
    {
        // this is sorta duplicated but for expediency's sake...
        if(initialized) throw std::logic_error("Engine::addCommandGroup: Can't add commands after initialization.");

        if(parent == nullptr) parent = &root; // root is the parent if this is a root level command. checks out.

        // check that the parent is known to the engine.
        if(!root.commandExists(parent)) // root is part of root, so this will return true in that case.
        {
            std::ostringstream error;
            error<<"Engine::addCommandGroup: parent '"<< parent->getName() <<"' is not a command known to this engine.\n";
            throw std::invalid_argument(error.str());
        }
        
        // okay, add it
        // Now, probably this should be a unique pointer from the start but I'm being lazy here.
        CommandGroup* toReturn = new CommandGroup(name); 
        parent->addCommand(toReturn);
        return toReturn;
    }

    void cli::Engine::initialize()
    {
        implementHelp();

        if(initialized) throw std::logic_error("Engine::initalize: Already initialized.");
        initialized=true;

        output << "Welcome to Robada CLI! 'help' for help.\n";
        output << "> ";
    }

    bool cli::Engine::interpret()
    {
        if(!initialized) throw std::logic_error("Engine::interpret: Engine not initialized. Call Engine::intialize first."); 

        if (parseCommand())
        {
            output << "\n> ";
            return true;
        }
        
        return false;
    }





    // Private functions


    // helper for parseCommand
    std::string& trim(std::string& s)
    {
        return s.erase(s.find_last_not_of(" \n\r\t") + 1).erase(0, s.find_first_not_of(" \n\r\t"));
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
        //addCommand(nullptr, "help", )
        addCommand(nullptr, "help")
            ->setDescription(
                "Get information about a command.\nUse arguments to get information"
                " about a specific command by typing it the way you would a normal command."
                "\ne.g. 'help help' would provide help info for this command.")
            ->addVariadicArgument("arguments", "Specify the command the way you would call it.", builtins::Types::TOKEN)
            ->setImplementation(
                [&](UserArgs args, std::ostream& out)->bool
                {   
                    // we use the args as a list to find a command of that name and print the help for it.

                    // extract the variadic argument(s)
                    void* vararg = args["arguments"];
                    auto arguments = static_cast<std::vector<void*>*>(vararg);

                    CommandGroup* current = &root;
                    for (int i = 0; i<arguments->size(); i++)
                    {
                        std::string& commandName = *static_cast<std::string*>((*arguments)[i]);

                        CommandBase* next = current->findSubCommand(commandName);
                        if (next == nullptr)
                        {
                            out << "Couldn't find command '" << commandName << "'.\n";
                            current->printHelp(out);
                            return true;
                        }

                        current = dynamic_cast<CommandGroup*>(next);
                        if (current == nullptr)
                        {
                            if (i != arguments->size() - 1)
                            {
                                out << "'"<<commandName << "' Does not have subcommands.\n";
                            }

                            next->printHelp(out);
                            return true;
                        }
                    }

                    current -> printHelp(out);
                    return true;
                });
        
        
    }

}