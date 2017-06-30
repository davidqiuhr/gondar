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

## Code style

LLVM's
[clang-format](http://releases.llvm.org/4.0.0/tools/clang/docs/ClangFormat.html) tool
is used to keep the code style consistent. Conveniently, that tool has
a preset for Chromium code, and since we mostly work on Chromium code
that seems like a nice default.

To format all the source files run `make format`. You can also run
`clang-format` directly.

## Testing in a Windows VM

Download and decompress the Windows image:

    aws s3 cp s3://windows-installed-images/win10-installed.raw.lz4 .
    lz4 win10-installed.raw.lz4

Create the backing image for the emulated USB device:

    qemu-img create -f raw fakeusb.raw 16G
    
*TODO(nicholasbishop): could probably also set up a device passthrough
for testing on actual USB devices...*

Run the VM with three drives: the emulated USB device, the Windows
installation, and the UEFI firmware:

    qemu-kvm \
      -m 4G \
      -smp 4 \
      -usbdevice tablet \
      -device usb-storage,drive=fakeusb,removable=yes \
      -drive if=none,id=fakeusb,file=fakeusb.raw \
      -drive format=raw,file=../windows-vm/win10-installed.raw \
      -drive if=pflash,format=raw,file=../windows-vm/ovmf.fd \
      -net nic -net user,smb=/path/to/gondar

The `-net` options enable a Samba server to share your gondar build
between the host and the guest. (Your host must have the Samba server
installed for this to work.) Set `smb=` to the absolute path on your
host where your gondar build lives.

Within the guest you can access the shared drive via `\\10.0.2.4\qemu`.
