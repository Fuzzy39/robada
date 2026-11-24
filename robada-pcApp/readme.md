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

pcApp depends on wxWidgets for UI and simpleBLE for bluetooth communications. Both of these are gitsubmodules of the repo, and are built alongside the project.
Run `git submodule update --init --recursive` To clone all of the dependencies that pcApp has. See `https://git-scm.com/book/en/v2/Git-Tools-Submodules` for more information about submodules. 

### WxWidgets

Robada pcApp depends on wxWidgets version 3.3.4 for UI. Though many other versions of wxWidgets will probably work without issue.
To set the version being used, navigate to the wxWidgets folder and run `git checkout v3.3.4`.

On Linux, you will need to install the `libgtk-3-dev` package to build, as one of the linux options for wxWidgets depends on it.

### SimpleBLE
Robada pcApp depends on SimpleBLE version 1.4.0 for bluetooth communications, though many other versions may work without issue.
To set the version being used, navigate to the simpleble folder and run `git checkout v1.4.0`.

See more about building simpleble here: https://docs.simpleble.org/simpleble/usage


### Building
Once all of the dependencies are setup, run `cmake -B build -S .` in the robada-pcApp folder.

This sets up CMake with 'build' as the output folder.
In theory, the development environment should now be set up.

To build the project run `cmake --build build -j4`. If all goes well, you should find the executables somewhere in the build folder. Namely: `pcApp`, `pcAppConsole`, and (for now) `bleTest`. The first time will take a while, as wxWidgets and simpleBLE need to be built, but it shouldn't have to be built after the first time.

On Windows, if using visual studio as the build system, run `cmake --build build --config Release`
(This is because I installed simpleble wrong when I did it on my machine, if you do it properly you shouldn't need the --config. TODO: FIX AND REMOVE THIS)

