FROM fedora:25

RUN dnf install -y make mingw64-qt5-qmake

ADD *.c *.cc *.h *.pro *.qrc /opt/gondar/
ADD images opt/gondar/images

WORKDIR /opt/gondar
RUN mingw64-qmake-qt5 gondar.pro
RUN make -j4 release

ENV LIBDIR /usr/x86_64-w64-mingw32/sys-root/mingw/bin

# Copy the smallest set of libraries we can get away with
RUN cp \
	${LIBDIR}/Qt5Core.dll \
	${LIBDIR}/Qt5Gui.dll \
	${LIBDIR}/Qt5Network.dll \
	${LIBDIR}/Qt5Widgets.dll \
	${LIBDIR}/iconv.dll \
	${LIBDIR}/libGLESv2.dll \
	${LIBDIR}/libgcc_s_seh-1.dll \
	${LIBDIR}/libglib-2.0-0.dll \
	${LIBDIR}/libharfbuzz-0.dll \
	${LIBDIR}/libharfbuzz-0.dll \
	${LIBDIR}/libintl-8.dll \
	${LIBDIR}/libpcre-1.dll \
	${LIBDIR}/libpcre16-0.dll \
	${LIBDIR}/libpng16-16.dll \
	${LIBDIR}/libstdc++-6.dll \
	${LIBDIR}/libwinpthread-1.dll \
	${LIBDIR}/zlib1.dll \
	release/
