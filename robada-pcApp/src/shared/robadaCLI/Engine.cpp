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

        

        auto test = addCommandGroup(nullptr, "test")
            ->setDescription("Demonstrate cli functionality.\nSeriously, just a test. I was in fact testing you.");
        
        
        addCommand(test, "sum")
            ->setDescription("sums all of its arguments.\nI mean, checks out, right?")
            ->addArgument("Term 1", "The first term to add.", cli::builtins::Types::DOUBLE)
            ->addVariadicArgument("Term ...", "Additional terms to add.", cli::builtins::Types::DOUBLE)
            ->setImplementation(COMMAND
            {
                double sum = *static_cast<double*>(args["Term 1"]);
                std::vector<void*>* arguments = static_cast<std::vector<void*>*>(args["Term ..."]);

                for(void* arg : *arguments)
                {
                    sum += *static_cast<double*>(arg);
                }

                out<<"The sum is: "<<sum<<".\n";
                return true; 
            });
            
        addCommand(test, "repeat")
            ->setDescription("Repeats it's first argument as many times as its second."
                             "\nIf the repeat count isn't provided, repeats twice.")
            ->addArgument("Word", "The word to repeat.", cli::builtins::Types::TOKEN)
            ->addOptionalArgument("Repeat Count", "The number of times to repeat word. Two is the default.",
                                    cli::builtins::Types::INT)
            ->setImplementation(COMMAND
            {
                std::string* word = static_cast<std::string*>(args["Word"]);
                
                int* repeats = static_cast<int*>(args["Repeat Count"]);
                int repeatCount = 2;
                if(repeats != nullptr){ repeatCount = *repeats; }

                for(int i = 0; i<repeatCount; i++)
                {
                    out<<*word<<" ";
                }

                out<<"\n";
                return true; 
            });
        
        addCommand(nullptr, "add")
            ->setDescription("Add two numbers.")
            ->addArgument("Term 1", "The first number to add.", cli::builtins::Types::DOUBLE)
            ->addArgument("Term 2", "The second number to add.", cli::builtins::Types::DOUBLE)
            ->setImplementation(COMMAND
            {
               double Term1 = *static_cast<double*>(args["Term 1"]);
               double Term2 = *static_cast<double*>(args["Term 2"]);
               out << Term1 <<" + "<< Term2 <<" = "<< (Term1+Term2)<<"\n";
               return true; 
            });
    }

}