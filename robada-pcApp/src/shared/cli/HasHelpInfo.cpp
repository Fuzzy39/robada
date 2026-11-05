#include <cli/HasHelpInfo.h>

// Whole file for just one function. Feels wasteful, almost.

const std::string cli::HasHelpInfo::getBriefDescription() const
{
    size_t index = getDescription().find_first_of("\n");

    if (index == std::string::npos) return getDescription();
    return std::string(getDescription()).erase(index);
}
