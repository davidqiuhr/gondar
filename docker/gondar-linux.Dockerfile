FROM fedora:25

RUN dnf install -y \
	cmake \
	gcc \
	gcc-c++ \
	qt5-qtbase-devel \
	zlib-devel

# Build minizip
ADD minizip /opt/gondar/minizip
WORKDIR /opt/gondar/build/minizip
RUN cmake -DUSE_AES=OFF ../../minizip
RUN make -j

# Build gondar
ADD *.pro *.qrc /opt/gondar/
ADD images /opt/gondar/images
ADD plog /opt/gondar/plog
ADD src /opt/gondar/src
ADD test /opt/gondar/test

WORKDIR /opt/gondar/build
RUN qmake-qt5 ..
RUN make -j

# Run tests in a headless mode
RUN TESTARGS="-platform offscreen" make check
