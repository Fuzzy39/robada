#include <iostream>
#include <cli/Engine.h>

int main(void)
{
    cli::Engine parser(std::cin, std::cout);
    while(parser.interpret());
    return 0;
}