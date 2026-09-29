## Build Instructions
Robada pcApp uses Cmake as its build system, and depends on simpleBLE and wxWidgets version 3.2.

Install cmake if you haven't before starting.
Please follow the instructions to build and install simpleble via cmake:
https://docs.simpleble.org/simpleble/usage
And do the same with wx-widgets. When building on linux, you could instead install the libwxgtk3.2-dev package if it is available.
https://docs.wxwidgets.org/3.2/overview_install.html

To set up cmake for this project, run `cmake -B build -S .` in the robada-pcApp folder.
Then, to build the project:

On Linux, run `cmake --build build`

On Windows, if using visual studio as the build system, run `cmake --build build --config Release`
(This is because I installed simpleble wrong when I did it on my machine, if you do it properly you shouldn't need the --config.)

Provided everything works, you should find the executable(s) somewhere in the build folder.