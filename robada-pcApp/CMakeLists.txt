cmake_minimum_required(VERSION 3.28)

project(robada-pcApp LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 23)

# simpleBLE
find_package(simpleble REQUIRED CONFIG)

# wxWidgets (copied from their documentation)
find_package(wxWidgets REQUIRED COMPONENTS net core base)
if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()


# Really simple simpleble test. Probably temporary.
add_executable(bleTest "bleTest/main.cpp")
target_link_libraries(bleTest simpleble::simpleble)


add_executable(pcApp "src/test.cpp")
target_include_directories(pcApp PUBLIC "include")
target_link_libraries(pcApp simpleble::simpleble)
target_link_libraries(pcApp ${wxWidgets_LIBRARIES})
