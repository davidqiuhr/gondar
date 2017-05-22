FROM fedora:25

RUN dnf install -y \
	automake \
	cmake \
	gcc \
	gcc-c++ \
	git \
	kernel-devel \
	make \
	mingw64-qt5-qmake \
	mingw64-qt5-qtbase-static

WORKDIR /opt/gondar
RUN git clone https://github.com/nmoinvaz/minizip
WORKDIR /opt/gondar/minizip
RUN git checkout dc3ad01e3d5928e9105f770b7e896a8e9fe0d3b4
ADD *.patch /opt/gondar/
RUN git apply ../minizip.patch
RUN mkdir build
WORKDIR /opt/gondar/minizip/build
RUN mingw64-cmake ..
RUN make

WORKDIR /opt/gondar
ADD *.c *.cc *.h *.pro *.qrc /opt/gondar/
ADD images /opt/gondar/images

RUN mingw64-qmake-qt5 gondar.pro
RUN make -j4 release
