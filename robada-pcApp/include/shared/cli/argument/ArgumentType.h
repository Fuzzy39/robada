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
    typedef std::function<void(void*)> DeleteValueFunc;

    typedef std::map<ArgTypeId, std::unique_ptr<ArgumentType>> TypeRegistry;
 
    class ArgumentType : public HasHelpInfo
    {
    private:
        
        ArgIsOfTypeFunc& checkType;
        ConvertArgFunc& convertArg;
        DeleteValueFunc& deleteValue;

    public:
        ArgumentType(std::string name, ArgIsOfTypeFunc& checkType, ConvertArgFunc& convertArg, DeleteValueFunc& deleteValue);
        ArgumentType* setDescription(std::string description);

        bool CheckStringIsOfType(std::string token);
        void* ConvertStringToType(std::string token);
        void DeleteValue(void* argValue);

        virtual void printHelp(std::ostream& out) override;
    };

   

}