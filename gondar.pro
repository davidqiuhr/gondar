QT = core network widgets

CONFIG += console

HEADERS       = gondarwizard.h \
                downloader.h \
                deviceguy.h \
                shared.h \
                gondar.h
SOURCES       = gondarwizard.cc \
                downloader.cc \
                main.cc \
                deviceguy.c
RESOURCES     = gondarwizard.qrc

win32 {
  SOURCES += gondar.c
} else {
  SOURCES += stubs.cc  
}


# install
target.path = .
INSTALLS += target

TEMPLATE = app
TARGET = gondar
INCLUDEPATH += .
INCLUDEPATH += ms-sys/inc
CFLAGS = -O0 -g

win32 {
  LIBS += -lsetupapi -lole32 -lgdi32 -lwininet -lshlwapi -lcrypt32 -lwintrust -lcomdlg32 -luuid
}