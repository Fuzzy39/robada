cmake_minimum_required(VERSION 3.28)

project(robada-pcApp LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

find_package(simpleble REQUIRED CONFIG)


#add_library(ocr_lib STATIC "src/shared/test.cpp" "src/shared/stb_image.cpp")

add_executable(bleTest "src/main.cpp")
target_include_directories(bleTest PUBLIC "include")
target_link_libraries(bleTest simpleble::simpleble)
#target_link_libraries(train ocr_lib)
