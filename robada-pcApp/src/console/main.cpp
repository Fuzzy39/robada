#include <iostream>
#include <robadaCLI/Engine.h>

int main(void)
{
    try
    {
        robadaCLI::Engine parser(std::cin, std::cout);
        parser.initialize();
        while(parser.interpret());
    }
    catch(const std::exception& e)
    {
        std::cerr <<"pcApp crashed with the following exception: "<< e.what() << '\n';
    }
    
    return 0;
}