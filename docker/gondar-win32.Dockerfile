FROM neverware/gondar-build-mxe:v3

ENV TREAT_WARNINGS_AS_ERRORS=true

RUN dnf install -y texinfo

ADD libmicrohttpd /opt/gondar/libmicrohttpd
# build microhttpd first so it's faster to iterate on our own changes
WORKDIR /opt/gondar/libmicrohttpd
run ./bootstrap
run ./configure --host=i686-w64-mingw32.static --enable-static --disable-shared --disable-https
run make
run make install

ADD CMakeLists.txt Makefile /opt/gondar/
ADD gdisk /opt/gondar/gdisk
ADD infra /opt/gondar/infra
ADD minizip /opt/gondar/minizip
ADD plog /opt/gondar/plog
ADD resources /opt/gondar/resources
ADD src /opt/gondar/src
ADD test /opt/gondar/test

ARG RELEASE=false
ARG CHROMEOVER=false
ARG METRICS_API_KEY=""
ARG GOOGLE_SIGN_IN_CLIENT=""
ARG GOOGLE_SIGN_IN_SECRET=""

# copy microhttpd lib into mxe environment
RUN cp /usr/local/lib/libmicrohttpd.a /opt/mxe/usr/lib/gcc/i686-w64-mingw32.static/5.4.0/
RUN cp /usr/local/include/microhttpd.h /opt/mxe/usr/i686-w64-mingw32.static/include

RUN make -C /opt/gondar build-gondar
