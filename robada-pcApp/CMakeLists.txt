cmake_minimum_required(VERSION 3.28)

project(robada-pcApp LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 23)

# Warnings
# if (MSVC)
#     # warning level 4
#     add_compile_options(/W4)
# else()
#     # additional warnings
#     add_compile_options(-Wall -Wextra -Wpedantic)
# endif()

# simpleBLE
set(SIMPLEBLE_INSTALL OFF)              # Makes it so there aren't any installation rules. Which we probably don't want.
add_subdirectory(simpleble/simpleble)

# wxWidgets
set(wxUSE_STL ON)           # Seems like a good idea.
set(wxUSE_DETECT_SM OFF)    # This is an option for the wxgtk part of wxWidgets. Got some strange linker error so I disabled the feature it was coming from. 
                            # This should only do anything on linux.
set(wxBUILD_SHARED OFF)     # For whatever reason wxWidgets doesn't seem to work as dynamic libraries on Windows for me? 
                            # (https://forums.wxwidgets.org/viewtopic.php?t=50339)
add_subdirectory(wxWidgets)

# RobadaLib
add_library(robadaLib SHARED "src/robadaLib/private/RequestQueue.cpp" "src/robadaLib/public/RobadaLib.cpp")
target_include_directories(robadaLib PUBLIC "include/robadaLib")



# cliLib
# cliLib, as it stands currently, is composed of two parts. A general purpose cli 'engine' of sorts, and tooling for it to be used for robada.
# since it's wrapped up as one library, it depends on robadaLib. Honestly the pure cli part could be split off into it's own (small) project entirely.
add_library(cliLib SHARED "src/cliLib/cli/Engine.cpp" "src/cliLib/cli/HasHelpInfo.cpp" "src/cliLib/cli/Builtins.cpp"
"src/cliLib/cli/argument/Argument.cpp" "src/cliLib/cli/argument/ArgumentType.cpp"  
"src/cliLib/cli/command/CommandGroup.cpp" "src/cliLib/cli/command/Command.cpp" 
"src/cliLib/robadaCLI/Engine.cpp")
target_include_directories(cliLib PUBLIC "include/cliLib")
target_include_directories(robadaLib PUBLIC "include/robadaLib/public")
target_link_libraries(cliLib robadaLib)



# Really simple simpleble test. Probably temporary.
add_executable(bleTest "bleTest/main.cpp")
target_link_libraries(bleTest simpleble::simpleble)

# GUI pcApp
add_executable(pcApp WIN32 "src/gui/main.cpp" "src/gui/TestPanel.cpp" "src/gui/ConsolePanel.cpp")

target_include_directories(pcApp PUBLIC "include/gui")
target_include_directories(pcApp PUBLIC "include/robadaLib/public")
target_include_directories(pcApp PUBLIC "include/cliLib")

target_link_libraries(pcApp robadaLib)
target_link_libraries(pcApp cliLib)
# target_link_libraries(pcApp simpleble::simpleble)
target_link_libraries(pcApp  wx::net wx::core wx::base)

# Console pcApp
add_executable(pcAppConsole "src/console/main.cpp")
target_include_directories(pcAppConsole PUBLIC "include/console")
target_include_directories(pcAppConsole PUBLIC "include/cliLib")
target_link_libraries(pcAppConsole cliLib)
# target_link_libraries(pcAppConsole simpleble::simpleble)
target_link_libraries(pcAppConsole ${wxWidgets_LIBRARIES})