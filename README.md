to build:
- get MXE `git clone https://github.com/mxe/mxe`
- build required packages for mxe `make qtbase` in mxe dir
note: there is currently an issue with this, but it worked last week
issue: https://github.com/mxe/mxe/issues/1762

run the command included in winqmake
make

To build for linux:

    mkdir build
    cd build
    qmake ..
    make

Note the `build` directory; the name doesn't matter but this ensures
your source directory doesn't get cluttered with build output.

To configure a debug build run qmake like this:

    qmake CONFIG+=debug
    
With the appropriate packages installed you can also create a Windows
build by replacing the `qmake` command with `mingw64-qmake-qt5`.
