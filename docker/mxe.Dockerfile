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

run useradd -ms /bin/bash kewluser
user kewluser
workdir /home/kewluser

RUN git clone https://github.com/mxe/mxe
WORKDIR /home/kewluser/mxe

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

ENV PATH=$PATH:/opt/mxe/usr/bin
ENV CMAKE=i686-w64-mingw32.static-cmake
