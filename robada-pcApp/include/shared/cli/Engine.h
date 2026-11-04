#pragma once
#include <iostream>
#include "CommandGroup.h"

#define COMMAND [](std::vector<std::string> args, std::ostream& out)->bool

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
        Engine(std::istream& input, std::ostream& output);

      
        // add a command to the interpreter. parent can be null if it is a root level command.
        // returns command back for ease of use.
        cli::Command* addCommand(cli::CommandGroup* parent, cli::Command* command);

        // initialize should be called after commands are added, but before interperting any commands.
        void initialize();

        // interpret a new line of input. Returns whether the program should continue running. Initialize needs to be called first.
        bool interpret();

    private:
        // check whether this command exists in the engine.
        bool parseCommand(); // returns whether the program should continue running.
        void implementHelp();
    };

    // register commands that are shared between GUI and CLI.
    cli::Engine& registerSharedCommands(cli::Engine& engine);
}