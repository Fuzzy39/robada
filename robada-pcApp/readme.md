# Robada pcApp
Robada pcApp is a cross-platform app that allows the user to command and control Robada wirelessly using Bluetooth Low Energy (BLE).
It consists of two programs, `pcApp` and `pcAppConsole`, which provide GUI and CLI interfaces.

Robada pcApp is written in C++ and is set up with CMake to enable cross-platform development.
This readme is primarily for setting up the development environment for the pcApp.

## Setup and Build Instructions
Before you begin, you will need to install Git and CMake if you have not already. You will also need a C++ build system installed for CMake to use.
On Windows this can be accomplished by installing Visual Studio, though there are other methods.
Most Linux systems already have this installed, but `binutils` is a common package that can be installed if needed.


First, you will need to clone the git repository. 

`git clone https://github.com/Fuzzy39/robada.git`

Should clone the project and create a folder for it in the working directory.

### WxWidgets

Robada pcApp depends on wxWidgets version 3.2 for UI. In order to make the build system work cross platform with Cmake, wxWidgets is
a git submodule of this project. After cloning you will need to run a couple additional commands:

`git submodule init`

followed by

`git submodule update`

This should download the wxWidgets source code for the project.

Once done, confirm that the wxWidgets submodule is on the correct branch. (I'm not confident!) Navigate to robade-pcApp/wxWidgets and run `git branch`. Confirm the selected branch is 3.2.

Then, navigate to the wxWidgets directory and run `git submodule init` and `update` again. This downloads wxWidgets' dependencies.

On Linux, you will need to install the `libgtk-3-dev` package to build, as one of the linux options for wxWidgets depends on it.

### SimpleBLE
Robada pcApp also depends on simpleBLE for cross-platform BLE capabilities.
Please follow the instructions to build and install simpleble via CMake:
https://docs.simpleble.org/simpleble/usage



### Building
Once all of the dependencies are setup, run `cmake -B build -S .` in the robada-pcApp folder.

This sets up CMake with 'build' as the output folder.
In theory, the development environment should now be set up.

To build the project run `cmake --build build -j4`. If all goes well, you should find the executables somewhere in the build folder. Namely: `pcApp`, `pcAppConsole`, and (for now) `bleTest`. The first time will take a while, as wxWidgets needs to be built, but it shouldn't have to be built after the first time.

On Windows, if using visual studio as the build system, run `cmake --build build --config Release`
(This is because I installed simpleble wrong when I did it on my machine, if you do it properly you shouldn't need the --config. TODO: FIX AND REMOVE THIS)

