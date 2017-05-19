FROM fedora:25

RUN dnf install -y \
	make \
	mingw64-qt5-qmake \
	mingw64-qt5-qtbase-static \
	git \
	cmake \
	make \
	automake \
	gcc \
	gcc-c++ \
	kernel-devel

WORKDIR /opt/gondar
RUN git clone https://github.com/nmoinvaz/minizip
WORKDIR /opt/gondar/minizip
RUN mkdir winders
WORKDIR /opt/gondar/minizip/winders
RUN mingw64-cmake ..
RUN make

WORKDIR /opt/gondar
ADD *.c *.cc *.h *.pro *.qrc /opt/gondar/
ADD images /opt/gondar/images

RUN mingw64-qmake-qt5 gondar.pro
RUN make -j4 release
