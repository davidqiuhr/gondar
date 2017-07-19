FROM neverware/gondar-build-mxe:v2

ENV PATH=$PATH:/opt/mxe/usr/bin

ADD minizip /opt/gondar/minizip
WORKDIR /opt/gondar/build/minizip-mxe
RUN /opt/mxe/usr/bin/i686-w64-mingw32.static-cmake -DUSE_AES=OFF ../../minizip
RUN make

WORKDIR /opt/gondar
ADD plog /opt/gondar/plog
ADD src /opt/gondar/src/
ADD test /opt/gondar/test
ADD *.pro *.qrc *.rc /opt/gondar/
ADD images /opt/gondar/images
ADD windows_resources /opt/gondar/windows_resources

ARG RELEASE=false
ARG CHROMEOVER=false
RUN /opt/mxe/usr/bin/i686-w64-mingw32.static-qmake-qt5 gondar.pro chromeover=$CHROMEOVER release=$RELEASE
RUN make -j4 release
