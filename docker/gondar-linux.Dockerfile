FROM fedora:25

RUN dnf install -y \
	cmake \
	gcc \
	gcc-c++ \
	qt5-qtbase-devel \
    which \
	zlib-devel

ADD Makefile *.pro *.qrc /opt/gondar/
ADD images /opt/gondar/images
ADD minizip /opt/gondar/minizip
ADD plog /opt/gondar/plog
ADD src /opt/gondar/src
ADD test /opt/gondar/test

# Build gondar and run tests
RUN make -C /opt/gondar build-gondar test
