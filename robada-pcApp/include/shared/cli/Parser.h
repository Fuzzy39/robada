#pragma once
#include <iostream>
#include "CommandGroup.h"

namespace cli
{
    class Parser
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
        Parser(std::istream& input, std::ostream& output);
    
        bool parseCommand(); // returns whether the program should continue running.
        
    private:
        void implementCommands();
    };
}