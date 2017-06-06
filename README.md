# Gondar
[Crummy Wizard!](https://www.youtube.com/watch?v=1TVjEGqJoMg)

Gondar is a project that aims to allows users to download one of our latest
CloudReady releases and burn it onto a USB stick for immediate use.

We're primarily targeting Windows 10 for compatibility.

## Building
### With docker
To build in one step with docker, run:
```
./package.py
```

`package.py` will handle building everything for you using mxe in a docker
container.  Check out `./Dockerfile` to see how that happens.
    
To configure a debub build, modify `gondar.pro` with 

```
qmake CONFIG+=debug
```

### With MXE

* Get MXE `git clone https://github.com/mxe/mxe`

* Build required packages for mxe `make qtbase` in mxe dir
  Note: there is currently an issue with this, but it worked last week
    issue: https://github.com/mxe/mxe/issues/1762

* run the command included in winqmake

