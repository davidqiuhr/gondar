FROM neverware/gondar-build-mxe:v2

ENV PATH=$PATH:/opt/mxe/usr/bin

ADD minizip /opt/gondar/minizip
WORKDIR /opt/gondar/build/minizip-mxe
RUN /opt/mxe/usr/bin/i686-w64-mingw32.static-cmake ../../minizip
RUN make

WORKDIR /opt/gondar
ADD *.c *.cc *.h *.pro *.qrc /opt/gondar/
ADD images /opt/gondar/images

RUN /opt/mxe/usr/bin/i686-w64-mingw32.static-qmake-qt5 gondar.pro
RUN make -j4 release
