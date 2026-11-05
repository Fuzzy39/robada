#include <cli/command/Command.h>
#include <stdexcept>
#include <ostream>
#include <sstream>

namespace cli
{
    Command::Command(TypeRegistry& typeRegistry, std::string name, CommandFunc onRun)
        : CommandBase(name), typeRegistry(typeRegistry), onRun(onRun)
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

    Argument* Command::addArgument(std::string name, ArgTypeId type)
    {
        verifyArgumentValid(name, type);
        // actually do the work.
        arguments.insert(arguments.begin()+minArgs+1, std::make_unique<Argument>(name, typeRegistry[type]));
        minArgs++;
        return arguments[minArgs].get();

    }

    // optional arguments will be added at the end of the argument list.
    Argument* Command::addOptionalArgument(std::string name, ArgTypeId type)
    {
        verifyArgumentValid(name, type);
        arguments.push_back(std::make_unique<Argument>(name, typeRegistry[type]));
        return arguments[arguments.size()-1].get();
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
        bool toReturn = argsToPass.get() == nullptr;

        if(toReturn)
        {
            toReturn = onRun(*argsToPass, out);

            // free arguments after the call.
            for (UserArgs::iterator it = argsToPass->begin(); it != argsToPass->end(); it++)
            {
                delete it->second;
            }
        } 

        
        // print help text if the command fails.
        if (!toReturn) printHelp(out);
        return toReturn;
    }

    


    std::unique_ptr<UserArgs> Command::parseArguments(std::vector<std::string> args, std::ostream& out) const
    {
        if(args.size()<minArgs)
        {
            out << "Too few arguments for "<< name <<". Expected at least "<<minArgs<<".\n";   
            return std::make_unique<UserArgs>(nullptr);
        }

        if(args.size()>arguments.size())
        {
            out << "Too many arguments for "<< name <<". Expected at most "<<arguments.size()<<".\n";  
            return std::make_unique<UserArgs>(nullptr);
        }

        // actually try to parse the args.
        std::unique_ptr<UserArgs> toReturn = std::make_unique<UserArgs>();
        size_t i = 0;

        for(const std::unique_ptr<Argument>& arg : arguments)
        {   
            void* argumentValue = arg->tryGetArgument(getName(), args[i], out);

            if(argumentValue == nullptr)
            {
                // This argument failed to parse. Abort.
                return std::make_unique<UserArgs>(nullptr);
            }

            toReturn->insert({arg->getName(),  argumentValue});
            i++;
        }

        return toReturn;
    }

    // a pretty lame implementation, if you ask me...
    void Command::printHelp(std::ostream& out)
    {
        out << "Help for " << name << ": " << description << "\nArguments:\n";
        for(const std::unique_ptr<Argument>& arg : arguments)
        {  
            out<<"\t";
            arg->printHelp(out);
        }
    }
}