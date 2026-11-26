#include <cli/HasHelpInfo.h>

// Whole file for just one function. Feels wasteful, almost.
namespace cli
{
    HasHelpInfo::HasHelpInfo(std::string name, std::string description)
        : name(name), description(description)
    {

    }

    HasHelpInfo::HasHelpInfo(std::string name)
        : name(name), description("No description provided.")
    {

    }


    const std::string HasHelpInfo::getBriefDescription() const
    {
        size_t index = getDescription().find_first_of("\n");

        if (index == std::string::npos) return getDescription();
        return std::string(getDescription()).erase(index);
    }

    const std::string& HasHelpInfo::getName() const
    {
        return name;
    }

    const std::string& HasHelpInfo::getDescription() const 
    {
        return description;
    }
}