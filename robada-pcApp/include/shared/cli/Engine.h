#pragma once
#include <iostream>
#include <cli/command/CommandGroup.h>
#include <cli/command/Command.h>



namespace cli
{
    class Engine
    {
        // input stream, output stream, second output stream for robada comms.
        // CommandGroup 'root'
        // constructor, no-arg for console. A 'run' function that will parse things automatically
        // probably a built-in exit 'command'
        // help function

    private:
        std::istream& input;
        std::ostream& output;
        //std::ostream& robadaOutput;

        CommandGroup root;
        bool initialized;   // once initalized, an engine is able to interpret commands, but new commands may not be added.
                            // Main initialization task is setting up the help command.

    public:
        Engine(std::istream& input, std::ostream& output, std::string cliName, std::string briefDescription);

      
        // add a command to the interpreter. parent can be null if it is a root level command.
        // returns the command back if it was a group for easy use later. nullptr if not.
        cli::CommandGroup* addCommand(cli::CommandGroup* parent, cli::CommandBase* command);

        // initialize should be called after commands are added, but before interperting any commands.
        void initialize();

        // interpret a new line of input. Returns whether the program should continue running. Initialize needs to be called first.
        bool interpret();

    private:
        // check whether this command exists in the engine.
        bool parseCommand(); // returns whether the program should continue running.
        void implementHelp();
    };

 
}