FROM fedora:25

RUN dnf install -y \
    automake \
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

# this looks bad, because i intermix useradd with my file adds, but it works
run useradd -ms /bin/bash kewluser
ADD CMakeLists.txt Makefile /home/kewluser/gondar/
ADD gdisk /home/kewluser/gondar/gdisk
ADD infra /home/kewluser/gondar/infra
ADD minizip /home/kewluser/gondar/minizip
ADD plog /home/kewluser/gondar/plog
ADD resources /home/kewluser/gondar/resources
ADD src /home/kewluser/gondar/src
ADD test /home/kewluser/gondar/test
RUN mkdir -p /home/kewluser/gondar/build ; chown -R kewluser:kewluser /home/kewluser/gondar
user kewluser
workdir /home/kewluser

RUN git clone https://github.com/mxe/mxe
WORKDIR /home/kewluser/mxe
RUN git checkout 8285eb550400c4987e8ca202b6c4a80eb8658ed9

# end adding and permissioning

# Check out a specific MXE revision to ensure reproducible builds
#RUN git checkout 43214bf7e886bd310965f854dd3a37b64c685bfa

# Download and build each root package separately to (hopefully) limit
# rebuild time with future modifications

# The `-j` controls parallelism between MXE packages, while `JOBS`
# controls parallelism within a particular package's build. The value
# of eight was chosen quite arbitrarily.

RUN make -j8 download-zlib
RUN make -j8 zlib JOBS=8
RUN make -j8 download-qtbase
RUN make -j8 qtbase JOBS=8
RUN make -j8 download-qtwebview
RUN make -j8 qtwebview JOBS=8

ENV PATH=$PATH:/home/kewluser/mxe/usr/bin
ENV CMAKE=i686-w64-mingw32.static-cmake

# then the gondar part

ENV TREAT_WARNINGS_AS_ERRORS=true

WORKDIR /home/kewluser/gondar
#RUN chown kewluser:kewluser /home/kewluser/build

ARG RELEASE=false
ARG CHROMEOVER=false
ARG METRICS_API_KEY=""
RUN make -C /home/kewluser/gondar build-gondar

#RUN mkdir -p /home/kewluser/gondar/kewldir
