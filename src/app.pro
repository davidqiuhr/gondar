# pass chromeover/beerover value into code
eval(CHROMEOVER = $$chromeover) {
  equals(CHROMEOVER, 'true') {
    DEFINES += CHROMEOVER
  }
}

QT = core network widgets

CONFIG += static

eval(RELEASE = $$release) {
  !equals(RELEASE, 'true') {
    CONFIG += console
  }
}


HEADERS       = admin_check_page.h \
                chromeover_login_page.h \
                deviceguy.h \
                diskwritethread.h \
                downloader.h \
                gondar.h \
                gondarwizard.h \
                image_select_page.h \
                log.h \
                neverware_unzipper.h \
                shared.h \
                unzipthread.h \
                wizard_page.h
SOURCES       = ../minizip/minishared.c \
                ../minizip/miniunz.c \
                admin_check_page.cc \
                chromeover_login_page.cc \
                deviceguy.cc \
                diskwritethread.cc \
                downloader.cc \
                gondarwizard.cc \
                image_select_page.cc \
                log.cc \
                main.cc \
                neverware_unzipper.cc \
                unzipthread.cc \
                wizard_page.cc
RESOURCES     = ../gondarwizard.qrc

INCLUDEPATH += ../minizip ../plog/include

win32 {
  RC_FILE       = ../gondar.rc
  INCLUDEPATH += ms-sys/inc
  SOURCES += gondar.c
} else {
  SOURCES += stubs.cc  
}

*gcc* {
  QMAKE_CFLAGS += -Wextra -Wmissing-declarations
  # Expose fopen64 in stdio.h declarations
  QMAKE_CFLAGS += -D_LARGEFILE64_SOURCE

  # Disable the main() function in minizip/miniunz.c
  QMAKE_CFLAGS += -DNOMAIN
}

*g++* {
  QMAKE_CXXFLAGS += -Wextra -Wmissing-declarations -std=c++11 -Wsuggest-override

  # Treat plog as a system header so that it doesn't cause compiler warnings
  QMAKE_CXXFLAGS += -isystem ../../plog/include
  QMAKE_CXXFLAGS += -isystem ../plog/include

  # Mark Qt headers in the MXE build as system headers. Without this
  # the above compiler warning options apply to Qt headers, which can
  # spew a lot of warnings.
  win32 {
    QMAKE_CXXFLAGS += -isystem ../../mxe/usr/i686-w64-mingw32.static/qt5/include/QtCore
    QMAKE_CXXFLAGS += -isystem ../../mxe/usr/i686-w64-mingw32.static/qt5/include/QtNetwork
    QMAKE_CXXFLAGS += -isystem ../../mxe/usr/i686-w64-mingw32.static/qt5/include/QtWidgets
  }
}

# install
target.path = .
INSTALLS += target

TEMPLATE = app
TARGET = ../cloudready-usb-creator

win32 {
  LIBS += -L../build/minizip-mxe
  LIBS += -lsetupapi -lole32 -lgdi32 -lwininet -lshlwapi -lcrypt32 -lwintrust -lcomdlg32 -luuid

  # Needed for static linking
  LIBS += -lqwindows

  equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 7) {
    # Needed for static linking in Qt5.6, but breaks the build in Qt5.8
    LIBS += -lQt5PlatformSupport
  }
} else {
  LIBS += -L../minizip
}

LIBS += -lminizip -lz -laes
