to build:
- get MXE `git clone https://github.com/mxe/mxe`
- build required packages for mxe `make qtbase` in mxe dir
note: there is currently an issue with this, but it worked last week
issue: https://github.com/mxe/mxe/issues/1762

To build for linux:

    qmake && make
    
To build debug:

    qmake CONFIG+=debug && make

to build for windows:
run the command included in winqmake
make
