FROM neverware/gondar-build-mxe:v2

ENV PATH=$PATH:/opt/mxe/usr/bin
ENV CMAKE=i686-w64-mingw32.static-cmake
ENV TREAT_WARNINGS_AS_ERRORS=true

ADD CMakeLists.txt Makefile *.pro *.qrc *.rc /opt/gondar/
ADD images /opt/gondar/images
ADD infra /opt/gondar/infra
ADD minizip /opt/gondar/minizip
ADD plog /opt/gondar/plog
ADD src /opt/gondar/src
ADD test /opt/gondar/test
ADD windows_resources /opt/gondar/windows_resources

ARG RELEASE=false
RUN make -C /opt/gondar build-gondar
