#include <iostream>
#include <cstdlib>
#include <robadaCLI/Engine.h>

void registerConsoleCommands(cli::Engine& engine)
{
    engine.addCommand(nullptr, new cli::Command("clear", "Clears the console.", 0, 0, COMMAND{
#ifdef WINDOWS
        std::system("cls");
#else
        // Assume POSIX
        std::system ("clear");
#endif
        return true;
    }));
}



int main(void)
{
    try
    {
        robadaCLI::Engine parser(std::cin, std::cout);
        registerConsoleCommands(parser);
        parser.initialize();
        while(parser.interpret());
    }
    catch(const std::exception& e)
    {
        std::cerr <<"pcApp crashed with the following exception: "<< e.what() << '\n';
    }
    
    return 0;
}