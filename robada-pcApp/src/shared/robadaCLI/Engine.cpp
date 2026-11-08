#include "robadaCLI/Engine.h"
#include "cli/Engine.h"
#include "cli/Builtins.h"

namespace robadaCLI
{
        

    Engine::Engine(std::istream& input, std::ostream& output) 
        : cli::Engine(input, output, "Robada CLI", "Software for controlling the 3-axis robot Robada.\n\"exit\" or \"quit\" to exit.")
    {
        registerSharedCommands();   
    }


    void Engine::registerSharedCommands()
    {

        addCommand(nullptr, "about")
            ->setDescription("basic information about Robada pcApp.")
            ->setImplementation(COMMAND
            {
                out << "Robada pcApp.\nControls the 3-axis robot Robada via bluetooth.\
                \nThis software was made by Mason Hill.\nRobada was made by Matthew Lewis and Mason Hill.\n";
                return true;
            });
        std::cout<<"About done\n";

        // auto test = addCommandGroup(nullptr, "test")
        //     ->setDescription("Demonstrate cli functionality.\nSeriously, just a test. I was in fact testing you.");

        
        // addCommand(test, "fart")
        //     ->setDescription("Says Fart.\nWhat did you expect?")
        //     ->setImplementation(COMMAND
        //     {
        //         out << "fart!\n";
        //         return true; 
        //     });
        
        // addCommand(test, "echo")
        //     ->setDescription("Echos it's argument back, if provided.")
        //     ->addOptionalArgument("toEcho", "The word to echo", cli::builtins::Types::TOKEN)
        //     ->setImplementation(COMMAND
        //     {
        //         std::string* toEcho = static_cast<std::string*>(args["toEcho"]);

        //         if(toEcho == nullptr){ out << "The cave is utterly silent.\n"; return true; }
        //         out << "You hear '" << toEcho << "' reverberating through the cavern.\n";
        //         return true; 
        //     });
        
        // addCommand(test, "add")
        //     ->setDescription("Add two numbers.")
        //     ->addArgument("Term1", "The first number to add.", cli::builtins::Types::DOUBLE)
        //     ->addArgument("Term2", "The second number to add.", cli::builtins::Types::DOUBLE)
        //     ->setImplementation(COMMAND
        //     {
        //        double Term1 = *static_cast<double*>(args["Term1"]);
        //        double Term2 = *static_cast<double*>(args["Term2"]);
        //        out << Term1 <<" + "<< Term2 <<" = "<< (Term1+Term2)<<"\n";
        //        return true; 
        //     });

    }

}