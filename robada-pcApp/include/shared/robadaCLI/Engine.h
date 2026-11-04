#pragma once
#include "cli/Engine.h"

namespace robadaCLI
{
    class Engine : public cli::Engine
    {
    public:
        Engine(std::istream& input, std::ostream& output);
    private:
        // register commands that are shared between GUI and CLI.
        void registerSharedCommands();

    };
}