FROM fedora:25

RUN dnf install -y \
	make \
	mingw64-qt5-qmake \
	mingw64-qt5-qtbase-static

ADD *.c *.cc *.h *.pro *.qrc /opt/gondar/
ADD images opt/gondar/images

WORKDIR /opt/gondar
RUN mingw64-qmake-qt5 gondar.pro
RUN make -j4 release
