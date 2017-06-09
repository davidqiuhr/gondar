FROM fedora:25

RUN dnf install cmake qt5-qtbase-devel

WORKDIR /opt/gondar/build/minizip
RUN cmake ../../minizip
RUN make

WORKDIR /opt/gondar
ADD src /opt/gondar/src/
ADD *.pro *.qrc /opt/gondar/
ADD images /opt/gondar/images

RUN /opt/mxe/usr/bin/i686-w64-mingw32.static-qmake-qt5 gondar.pro
RUN make -j4 release
