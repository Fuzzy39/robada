#include <iostream>
#include <cli/Engine.h>

int main(void)
{
    try
    {
        cli::Engine parser(std::cin, std::cout);
        parser.initialize();
        while(parser.interpret());
    }
    catch(const std::exception& e)
    {
        std::cerr <<"pcApp crashed with the following exception: "<< e.what() << '\n';
    }
    
    return 0;
}