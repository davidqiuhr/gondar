FROM fedora:25

RUN dnf install -y \
	automake \
	cmake \
	gcc \
	gcc-c++ \
	git \
	kernel-devel \
	make \
	wget \
	gettext \
	bison \
	bzip2 \
	flex \
	gperf \
	intltool \
	libtool \
	patch \
	python \
	ruby \
	scons \
	zip \
	xz-static \
	p7zip \
	gdk-pixbuf2-devel \
	gdk-pixbuf2 \
	automake \
	which

WORKDIR /opt/gondar
RUN git clone https://github.com/mxe/mxe
WORKDIR /opt/gondar/mxe
RUN make qtbase --jobs=4
RUN make zlib

WORKDIR /opt/gondar
RUN git clone https://github.com/nmoinvaz/minizip
WORKDIR /opt/gondar/minizip
RUN git checkout dc3ad01e3d5928e9105f770b7e896a8e9fe0d3b4
WORKDIR /opt/gondar
RUN mkdir build
WORKDIR /opt/gondar/build
ENV PATH=$PATH:/opt/gondar/mxe/usr/bin
RUN /opt/gondar/mxe/usr/bin/i686-w64-mingw32.static-cmake ../minizip
RUN make

WORKDIR /opt/gondar
ADD *.c *.cc *.h *.pro *.qrc /opt/gondar/
WORKDIR /opt/gondar
ADD images /opt/gondar/images

ENV PATH=$PATH:/opt/gondar/mxe/usr/bin
RUN mxe/usr/bin/i686-w64-mingw32.static-qmake-qt5 gondar.pro
RUN make -j4 release
