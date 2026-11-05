#include "robadaCLI/Engine.h"
#include "cli/Engine.h"

namespace robadaCLI
{
        

    Engine::Engine(std::istream& input, std::ostream& output) 
        : cli::Engine(input, output, "Robada CLI", "Software for controlling the 3-axis robot Robada.\n\"exit\" or \"quit\" to exit.")
    {
        registerSharedCommands();   
    }


    void Engine::registerSharedCommands()
    {
        addCommand(nullptr, new cli::Command("about", "basic information about Robada pcApp.", 0, 0,
            COMMAND
            {
                out << "Robada pcApp.\nControls the 3-axis robot Robada via bluetooth.\nThis software was made by Mason Hill.\nRobada was made by Matthew Lewis and Mason Hill.\n";
                return true;
            }));
        cli::CommandGroup* TestGroup = static_cast<cli::CommandGroup*>(addCommand(nullptr, 
            new cli::CommandGroup("test", "Demonstrate cli functionality.\nSeriously, just a test. I was in fact testing you.",
            COMMAND
            {
                return false;
            })));

        addCommand(TestGroup, new cli::Command("fart", "Says Fart.\nWhat did you expect?", 0, 0,
            COMMAND{
                out << "fart!\n";
                return true;
            }));
        addCommand(TestGroup, new cli::Command("echo", "Echos it's argument back, if provided.", 0, 1,
            COMMAND{
                if (args.size() == 0) { out << "The cave is utterly silent.\n"; return true; }
                out << "You hear '" << args[0] << "' reverberating through the cavern.\n";
                return true;
            }));

    }

}