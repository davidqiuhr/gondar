FROM neverware/gondar-build-mxe:v2

WORKDIR /opt/gondar
RUN git clone https://github.com/nmoinvaz/minizip
WORKDIR /opt/gondar/minizip
RUN git checkout dc3ad01e3d5928e9105f770b7e896a8e9fe0d3b4
WORKDIR /opt/gondar
RUN mkdir -p build/minizip-mxe
WORKDIR /opt/gondar/build/minizip-mxe
ENV PATH=$PATH:/opt/gondar/mxe/usr/bin
RUN /opt/mxe/usr/bin/i686-w64-mingw32.static-cmake ../../minizip
RUN make

WORKDIR /opt/gondar
ADD *.c *.cc *.h *.pro *.qrc /opt/gondar/
WORKDIR /opt/gondar
ADD images /opt/gondar/images

ENV PATH=$PATH:/opt/mxe/usr/bin
RUN /opt/mxe/usr/bin/i686-w64-mingw32.static-qmake-qt5 gondar.pro
RUN make -j4 release
