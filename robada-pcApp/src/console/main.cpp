#include <iostream>
#include <cli/Parser.h>

int main(void)
{
    std::cout<<"Welcome to Robada CLI!\n";
    std::cout<<"> ";
    cli::Parser parser(std::cin, std::cout);
    while(parser.parseCommand())
    {
        std::cout<<"> ";
    }
    return 0;
}