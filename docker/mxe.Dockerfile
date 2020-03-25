FROM fedora:31

RUN dnf install -y \
    autoconf \
    automake \
    #autopoint \
    bison \
    bzip2 \
    cmake \
    flex \
    #g++-multilib \
    gcc \
    gcc-c++ \
    gdk-pixbuf2 \
    gdk-pixbuf2-devel \
    gettext \
    git \
    gperf \
    intltool \
    kernel-devel \
    #libc6-dev-i386 \
    #libgdk-pixbuf2.0-dev \
    #libltdl-dev \
    #libssl-dev \
    libtool \
    #libxml-parser-perl \
    lzip \
    make \
    openssl-devel \
    p7zip \
    pkg-config \
    patch \
    perl \
    python \
    python2 \
    ruby \
    scons \
    sed \
    wget \
    which \
    xz-static \
    zip

RUN dnf install -y \
  mariadb-devel

RUN git clone https://github.com/mxe/mxe /opt/mxe
WORKDIR /opt/mxe

# Check out a specific MXE revision to ensure reproducible builds
# this is master as of 2020/03/25
RUN git checkout 3a84473a0d1b9b523798565382e3763c3ed08f87

# Download and build each root package separately to (hopefully) limit
# rebuild time with future modifications

# The `-j` controls parallelism between MXE packages, while `JOBS`
# controls parallelism within a particular package's build. The value
# of eight was chosen quite arbitrarily.

RUN make -j4 download-zlib
RUN make -j4 zlib JOBS=4
RUN make -j4 download-qtbase
RUN make -j4 qtbase JOBS=4 EXCLUDE_PKGS='ocaml%'
#RUN make -j8 download-libmysqlclient
#RUN make -j4 libmysqlclient JOBS=4 EXCLUDE_PKGS='ocaml%'

ENV PATH=$PATH:/opt/mxe/usr/bin
ENV CMAKE=i686-w64-mingw32.static-cmake
