#include <cli/command/Command.h>
#include <stdexcept>
#include <ostream>
#include <sstream>
#include <iostream>

namespace cli
{
    Command::Command(TypeRegistry& typeRegistry, std::string name )
        : CommandBase(name), typeRegistry(typeRegistry), onRun(Command::defaultCommand), variadicArgument(nullptr)
    {
        if(onRun==nullptr)
        {
            throw std::invalid_argument("Command::Command: A command's onRun function may not be null.");
        }
    }

    // setup commands   
    Command* Command::setDescription(std::string description)
    {
        this->description = description;
        return this;
    }

    Command* Command::setImplementation(const CommandFunc& command)
    {
        onRun = command;
        return this;
    }

    Command* Command::addArgument(std::string name, std::string description, ArgTypeId type)
    {
        verifyArgumentValid(name, type);
        // actually do the work.
        arguments.insert(arguments.begin()+minArgs+1, 
            std::make_unique<Argument>(name, description, *typeRegistry[type]));
        minArgs++;
        return this;

    }

    // optional arguments will be added at the end of the argument list.
    Command* Command::addOptionalArgument(std::string name, std::string description, ArgTypeId type)
    {
        verifyArgumentValid(name, type);
        arguments.push_back(std::make_unique<Argument>(name, description, *typeRegistry[type]));
        return this;
    }

    Command* Command::addVariadicArgument(std::string name, std::string description, ArgTypeId type)
    {
        verifyArgumentValid(name, type);
        if(variadicArgument) throw std::invalid_argument("Command::addVariadicArgument: This command already has a variadic argument.\n");
        variadicArgument = std::make_unique<Argument>(name, description, *typeRegistry[type]);
        return this;
    }

    void Command::verifyArgumentValid(std::string name, ArgTypeId type)
    {
        // check the type exists
        if(typeRegistry.find(type) == typeRegistry.end())    
        {
            std::ostringstream error;
            error << "Command::addArgument: Type with id: "<<type<<" does not exist.\n";
            throw std::invalid_argument(error.str());
        }    

        // ensure argument names are unique
        for(std::unique_ptr<Argument>& arg : arguments)
        {
            if(arg->getName() == name)
            {
                std::ostringstream error;
                error << "Command::addArgument: Argument '"<<name<<"' Already exists in command '"<<getName()<<"'\n";
                throw std::invalid_argument(error.str());
            }
        }
    }


    // function to do command
    bool cli::Command::run(std::vector<std::string>args, std::ostream& out)
    {
        std::unique_ptr<UserArgs> argsToPass = parseArguments(args, out);
        bool toReturn = argsToPass.get() != nullptr;
        std::cout<<"To return: "<<toReturn<<"\n";
        if(toReturn)
        {
            toReturn = onRun(*argsToPass, out);

            // free arguments after the call.

            // variadic arguments
            if(variadicArgument)
            {
                auto variadicArgs = static_cast< std::vector<void*>*>((*argsToPass)[variadicArgument->getName()]);
                for(void* arg : *variadicArgs)
                {
                    variadicArgument->getType().DeleteValue(arg);
                }
            }

            // normal arguments
            for(const std::unique_ptr<Argument>& arg : arguments)
            {
                void* argToDelete =  (*argsToPass)[arg->getName()];
                if(argToDelete != nullptr) arg->getType().DeleteValue(argToDelete);
            }
        } 

        
        // print help text if the command fails.
        if (!toReturn) printHelp(out);
        return toReturn;
    }

    std::unique_ptr<UserArgs> Command::parseArguments(std::vector<std::string> rawArguments, std::ostream& out) const
    {
        if(rawArguments.size()<minArgs)
        {
            out << "Too few arguments for "<< name <<". Expected at least "<<minArgs<<".\n";   
            return std::unique_ptr<UserArgs>(nullptr);
        }

        if(rawArguments.size()>arguments.size() && !variadicArgument)
        {
            out << "Too many arguments for "<< name <<". Expected at most "<<arguments.size()<<".\n";  
            return std::unique_ptr<UserArgs>(nullptr);
        }

        // actually try to parse the args.
        std::unique_ptr<UserArgs> toReturn = std::make_unique<UserArgs>();
        size_t i = 0;

        for(const std::unique_ptr<Argument>& arg : arguments)
        {   
            if(i>= rawArguments.size())
            {
                // Optional argument is not present.
                std::cout<<"Optional Argument not present.\n";
                toReturn->insert({arg->getName(), nullptr});
                continue;
            }

            void* argumentValue = arg->tryGetArgument(getName(), rawArguments[i], out);

            if(argumentValue == nullptr)
            {
                // This argument failed to parse. Abort.
                std::cout<<"Argument failed to parse.\n";
                return std::unique_ptr<UserArgs>(nullptr);
            }

            std::cout<<"Added Argument\n";
            toReturn->insert({arg->getName(),  argumentValue});
            i++;
        }

        if(!variadicArgument) return toReturn;

        // parse variadic arguments
        std::vector<void*>* varargs = new std::vector<void*>();
        toReturn->insert({variadicArgument->getName(), varargs});

        for(int i = arguments.size(); i<rawArguments.size(); i++)
        {
            void* argumentValue = variadicArgument->tryGetArgument(getName(), rawArguments[i], out);

            if(argumentValue == nullptr)
            {
                // This argument failed to parse. Abort.
                return std::unique_ptr<UserArgs>(nullptr);
            }
            varargs->push_back(argumentValue);
        }

        return toReturn;
    }

    void Command::printHelp(std::ostream& out)
    {
        out << "Help for " << name << ": " << description;
        
        if(arguments.size()>0 || variadicArgument)
        {
           out << "\nArguments:\n";
        }

        for(const std::unique_ptr<Argument>& arg : arguments)
        {  
            out<<"\t";
            arg->printHelp(out);
        }

        if(variadicArgument)
        {
            out<<"\tRepeatable: ";
            variadicArgument->printHelp(out);
        }
    }

    bool Command::defaultCommand(UserArgs args, std::ostream& out)
    {
        out << "This command has no implementation defined.\n";
        return true;
    }
}