#pragma once
#include <functional>
#include <map>
#include <memory>
#include <cli/HasHelpInfo.h>

namespace cli
{
    class ArgumentType;

    typedef int ArgTypeId;
    typedef std::function<bool(std::string&)> ArgIsOfTypeFunc;
    typedef std::function<void*(std::string&)> ConvertArgFunc;
    typedef std::map<ArgTypeId, std::unique_ptr<ArgumentType>> TypeRegistry;
 
    class ArgumentType : public HasHelpInfo
    {
    private:
        
        ArgIsOfTypeFunc& checkType;
        ConvertArgFunc& convertArg;

    public:
        ArgumentType(std::string name, ArgIsOfTypeFunc& checkType, ConvertArgFunc& convertArg);
        ArgumentType* setDescription(std::string description);

        bool CheckStringIsOfType(std::string token);
        void* ConvertStringToType(std::string token);

        virtual void printHelp(std::ostream& out) override;
    };

   

}