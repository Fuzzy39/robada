#include <cli/Parser.h>
#include <istream>
#include <ostream>
#include <cstring>

#define COMMAND [](std::vector<std::string> args, std::ostream& out)->bool

cli::Parser::Parser(std::istream& input, std::ostream& output)
    : input(input), output(output), 
      root(ROOT_NAME, "\"exit\" or \"quit\" to exit.", -1, -1, COMMAND{return false;})
{
    implementCommands();
}

std::string& trim(std::string& s)
{
    return s.erase(s.find_last_not_of(" \n\r\t") + 1).erase(0, s.find_first_not_of(" \n\r\t"));
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


    std::string line(buffer);
    line = trim(line);

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
        if(pos == std::string::npos)
        {
           
            args.push_back(line);
            line = "";
            continue;
        }

        std::string token = std::string(line.begin(), line.begin()+pos);
        args.push_back(token);
        line = trim(line.erase(0, pos));
    }

    // now that we have them, we call our command group.
    root.run(args, output);
    return true;
}


void cli::Parser::implementCommands()
{
    root.addCommand(new Command("about", "basic information about Robada pcApp.", 0, 0, 
        COMMAND
        { 
            out<<"Robada pcApp.\nControls the 3-axis robot Robada via bluetooth.\nThis software was made by Mason Hill.\nRobada was made by Matthew Lewis and Mason Hill.\n";  
            return true;
        }));
    CommandGroup* TestGroup = new CommandGroup("test", "Demonstrate cli functionality. Has subcommands, but can take one argument.", 1, 1, 
        COMMAND
        {
            out<<"Test '"<<args[0]<<"'? I hardly know 'er!\n";
            return true;
        });
    TestGroup->addCommand(new Command("fart", "Says Fart. what did you expect?",0,0, 
        COMMAND{
            out<<"fart!\n";
            return true;
        }));
    TestGroup->addCommand(new Command("echo", "Echos it's argument back, if provided.",0,1,
        COMMAND{
            if(args.size()==0){ out<<"The cave is utterly silent.\n"; return true;}
            out<<"You hear '"<<args[0]<<"' reverberating through the cavern.\n";
            return true;
        }));
    root.addCommand(TestGroup);

}