QT = core network widgets

CONFIG += console static

HEADERS       = src/deviceguy.h \
                src/diskwritethread.h \
                src/downloader.h \
                src/gondar.h \
                src/gondarwizard.h \
                src/image_select_page.h \
                src/neverware_unzipper.h \
                src/shared.h \
                src/unzipthread.h
SOURCES       = minizip/minishared.c \
                src/deviceguy.cc \
                src/diskwritethread.cc \
                src/downloader.cc \
                src/gondarwizard.cc \
                src/image_select_page.cc \
                src/main.cc \
                src/neverware_unzipper.c \
                src/unzipthread.cc
RESOURCES     = gondarwizard.qrc

INCLUDEPATH += minizip

win32 {
  INCLUDEPATH += ms-sys/inc
  SOURCES += src/gondar.c
} else {
  SOURCES += src/stubs.cc  
}

*gcc* {
  QMAKE_CFLAGS += -Wextra -Wmissing-declarations
}

*g++* {
  QMAKE_CXXFLAGS += -Wextra -Wmissing-declarations -std=c++11 -Wsuggest-override
}

# install
target.path = .
INSTALLS += target

TEMPLATE = app
TARGET = gondar

win32 {
  LIBS += -Lbuild/minizip-mxe
  LIBS += -lsetupapi -lole32 -lgdi32 -lwininet -lshlwapi -lcrypt32 -lwintrust -lcomdlg32 -luuid

  # Needed for static linking
  LIBS += -lqwindows

  equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 7) {
    # Needed for static linking in Qt5.6, but breaks the build in Qt5.8
    LIBS += -lQt5PlatformSupport
  }
} else {
  LIBS += -Lminizip
}

LIBS += -lminizip -lz -laes
