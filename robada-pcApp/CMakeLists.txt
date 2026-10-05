cmake_minimum_required(VERSION 3.28)

project(robada-pcApp LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 23)

# simpleBLE
find_package(simpleble REQUIRED CONFIG)

# wxWidgets (copied from their documentation)
# find_package(wxWidgets REQUIRED COMPONENTS net core base)
# if(wxWidgets_USE_FILE) # not defined in CONFIG mode
#     include(${wxWidgets_USE_FILE})
# endif()

# shared includes/library
include_directories(PUBLIC "include/shared")

add_library(robadaLib STATIC "src/shared/cli/Command.cpp" "src/shared/cli/CommandGroup.cpp" "src/shared/cli/Engine.cpp" "src/shared/cli/Commands.cpp")


# Really simple simpleble test. Probably temporary.
add_executable(bleTest "bleTest/main.cpp")
target_link_libraries(bleTest simpleble::simpleble)


# GUI pcApp
# add_executable(pcApp "src/gui/main.cpp")
# target_include_directories(pcApp PUBLIC "include/gui")
# target_link_libraries(pcApp robadaLib)
# target_link_libraries(pcApp simpleble::simpleble)
# target_link_libraries(pcApp ${wxWidgets_LIBRARIES})


# Console pcApp
add_executable(pcAppConsole "src/console/main.cpp")
target_include_directories(pcAppConsole PUBLIC "include/console")
target_link_libraries(pcAppConsole robadaLib)
target_link_libraries(pcAppConsole simpleble::simpleble)
# target_link_libraries(pcAppConsole ${wxWidgets_LIBRARIES})