FROM fedora:29

RUN dnf install -y \
    automake \
    bison \
    bzip2 \
    cmake \
    flex \
    gcc \
    gcc-c++ \
    gdk-pixbuf2 \
    gdk-pixbuf2-devel \
    gettext \
    git \
    gperf \
    intltool \
    kernel-devel \
    libtool \
    lzip \
    make \
    mingw32-qt5-qtbase-static \
    openssl-devel \
    p7zip \
    patch \
    python \
    ruby \
    scons \
    wget \
    which \
    xz-static \
    zip

ENV CMAKE=mingw32-cmake
#ENV TREAT_WARNINGS_AS_ERRORS=true

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
RUN make -C /opt/gondar build-gondar
WORKDIR /opt/gondar
