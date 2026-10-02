#include <cli/Parser.h>
#include <istream>
#include <ostream>
#include <cstring>

#define COMMAND [](std::vector<std::string> args, std::ostream& out)->bool

cli::Parser::Parser(std::istream& input, std::ostream& output)
    : input(input), output(output), 
      root("", "Robada CLI. \"exit\" or \"quit\" to exit.", -1, -1, COMMAND{return false;})
{
    implementCommands();
}

std::string& trim(std::string& s)
{
    return s.erase(s.find_last_not_of(" \n\r\t") + 1).erase(0, s.find_first_not_of(" \n\r\t")-1);
}

bool cli::Parser::parseCommand()
{
    const size_t MAX_LINE_LEN = 1000;
    char buffer[MAX_LINE_LEN];
    
    input.getline(buffer, MAX_LINE_LEN, '\n');
    
    if(strlen(buffer)==MAX_LINE_LEN-1)
    {
        output<<"Command input exceeded maximum length.\n";
        return true;
    }

    // convert to lowercase.
    for(int i = 0; i<strlen(buffer); i++)
    {
        buffer[i] = std::tolower(buffer[i]);
    }

    output<< "Raw: '"<<buffer<<"'.\n";
    std::string line(buffer);
    line = trim(line);
    output<< "Trimmed: '"<<line<<"'.\n";
    if(line.empty())
    {
        // carry on with life if it's empty.
        return true;
    }
    
    // quit the program?
    if(line == "exit" || line == "quit")
    {
        return false;
    }

    // Okay, now we need to construct the argument list.
    std::vector<std::string> args;
    while(!line.empty())
    {
        size_t pos = line.find_first_of(" \n\r\t");
        args.push_back(std::string(line.begin(), line.begin()+pos-1));
        line = trim(line.erase(0, pos));
    }

    // now that we have them, we call our command group.
    root.run(args, output);
    return true;
}


void cli::Parser::implementCommands()
{

}