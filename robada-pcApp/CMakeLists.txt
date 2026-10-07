cmake_minimum_required(VERSION 3.28)

project(robada-pcApp LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 23)

# simpleBLE
find_package(simpleble REQUIRED CONFIG)

# wxWidgets
set(wxUSE_STL ON)           # Seems like a good idea.
set(wxUSE_DETECT_SM OFF)    # This is an option for the wxgtk part of wxWidgets. Got some strange linker error so I disabled the feature it was coming from. 
                            # This should only do anything on linux.
set(wxBUILD_SHARED OFF)     # For whatever reason wxWidgets doesn't seem to work as dynamic libraries on Windows for me? 
                            # (https://forums.wxwidgets.org/viewtopic.php?t=50339)
add_subdirectory(wxWidgets)

# shared includes/library
include_directories(PUBLIC "include/shared")
add_library(robadaLib STATIC "src/shared/cli/Command.cpp" "src/shared/cli/CommandGroup.cpp" "src/shared/cli/Engine.cpp" "src/shared/cli/Commands.cpp")

# Really simple simpleble test. Probably temporary.
add_executable(bleTest "bleTest/main.cpp")
target_link_libraries(bleTest simpleble::simpleble)

# GUI pcApp
add_executable(pcApp WIN32 "src/gui/main.cpp")
target_include_directories(pcApp PUBLIC "include/gui")
target_link_libraries(pcApp robadaLib)
target_link_libraries(pcApp simpleble::simpleble)
target_link_libraries(pcApp  wx::net wx::core wx::base)

# Console pcApp
add_executable(pcAppConsole "src/console/main.cpp")
target_include_directories(pcAppConsole PUBLIC "include/console")
target_link_libraries(pcAppConsole robadaLib)
target_link_libraries(pcAppConsole simpleble::simpleble)
target_link_libraries(pcAppConsole ${wxWidgets_LIBRARIES})