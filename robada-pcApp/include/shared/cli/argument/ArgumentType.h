#pragma once
#include <functional>
#include <map>
#include <memory>
#include <cli/HasHelpInfo.h>

namespace cli
{
    class ArgumentType;

    typedef int ArgTypeId;

    typedef std::map<ArgTypeId, std::unique_ptr<ArgumentType>> TypeRegistry;
 
    class ArgumentType : public HasHelpInfo
    {
    private:
    public:
        ArgumentType(std::string name);
        ArgumentType(std::string name, std::string description);

        ArgumentType* setDescription(std::string description);

        virtual bool CheckStringIsOfType(std::string token) = 0;
        virtual void* ConvertStringToType(std::string token) = 0;
        virtual void DeleteValue(void* argValue) = 0;

        virtual void printHelp(std::ostream& out) override;
    };

   

}