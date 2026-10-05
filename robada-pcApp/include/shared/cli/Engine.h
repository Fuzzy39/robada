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


    public:
        Engine(std::istream& input, std::ostream& output);
        bool interpret();

    private:
        bool parseCommand(); // returns whether the program should continue running.
        void implementCommands();
        void implementHelp();
    };
}