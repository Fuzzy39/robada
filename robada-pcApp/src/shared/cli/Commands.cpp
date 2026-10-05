#include <cli/Engine.h>

void cli::Engine::implementCommands()
{
    root.addCommand(new Command("about", "basic information about Robada pcApp.", 0, 0,
        COMMAND
        {
            out << "Robada pcApp.\nControls the 3-axis robot Robada via bluetooth.\nThis software was made by Mason Hill.\nRobada was made by Matthew Lewis and Mason Hill.\n";
            return true;
        }));
    CommandGroup* TestGroup = new CommandGroup("test", "Demonstrate cli functionality.\nSeriously, just a test. I was in fact testing you.", -1, -1,
        COMMAND
        {
            return false;
        });
    TestGroup->addCommand(new Command("fart", "Says Fart.\nWhat did you expect?", 0, 0,
        COMMAND{
            out << "fart!\n";
            return true;
        }));
    TestGroup->addCommand(new Command("echo", "Echos it's argument back, if provided.", 0, 1,
        COMMAND{
            if (args.size() == 0) { out << "The cave is utterly silent.\n"; return true; }
            out << "You hear '" << args[0] << "' reverberating through the cavern.\n";
            return true;
        }));
    root.addCommand(TestGroup);

}