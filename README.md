# Gondar
[Crummy Wizard!](https://www.youtube.com/watch?v=1TVjEGqJoMg)

Gondar is a project that aims to allows users to download one of our latest
CloudReady releases and burn it onto a USB stick for immediate use.

We're primarily targeting Windows 10 for compatibility.

## Building
### With package.py
To make a build in one step, run:
```
./package.py
```
`package.py` will handle building everything for you using mxe in a
docker container.  Check out `docker/gondar-win32.Dockerfile` to see
how that happens.
    
To configure a debub build, modify `gondar.pro` with 

```
qmake CONFIG+=debug
```

### With MXE directly

* Get MXE `git clone https://github.com/mxe/mxe`

* Build required packages for mxe `make qtbase` in mxe dir

* run the command included in winqmake

### Build for Linux

1. Install dependencies (distro specific):

    Fedora:
    
        dnf install cmake qt5-qtbase-devel

    Ubuntu:

        aptitude install cmake qt5-default

2. make -f Makefile.linux

### Code style

LLVM's
[clang-format](http://releases.llvm.org/4.0.0/tools/clang/docs/ClangFormat.html) tool
is used to keep the code style consistent. Conveniently, that tool has
a preset for Chromium code, and since we mostly work on Chromium code
that seems like a nice default.

To format all the source files run `make format`. You can also run
`clang-format` directly.
