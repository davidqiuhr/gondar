QT = core network widgets

CONFIG += console static

HEADERS       = gondarwizard.h \
                downloader.h \
                deviceguy.h \
                shared.h \
                gondar.h \
                neverware_unzipper.h
SOURCES       = gondarwizard.cc \
                downloader.cc \
                main.cc \
                deviceguy.cc \
                neverware_unzipper.c \
                minizip/minishared.c
RESOURCES     = gondarwizard.qrc

INCLUDEPATH += minizip 

win32 {
  INCLUDEPATH += ms-sys/inc
  SOURCES += gondar.c
} else {
  SOURCES += stubs.cc  
}

*gcc* {
  QMAKE_CFLAGS += -Wextra -Wmissing-declarations
}

*g++* {
  QMAKE_CXXFLAGS += -Wextra -Wmissing-declarations
}

# install
target.path = .
INSTALLS += target

TEMPLATE = app
TARGET = gondar

LIBS += -lminizip -lz -laes
LIBPATH += minizip/native

win32 {
  LIBS += -lsetupapi -lole32 -lgdi32 -lwininet -lshlwapi -lcrypt32 -lwintrust -lcomdlg32 -luuid

  # Needed for static linking
  LIBS += -lqwindows

  equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 7) {
    # Needed for static linking in Qt5.6, but breaks the build in Qt5.8
    LIBS += -lQt5PlatformSupport
  }
}
