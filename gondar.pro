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
                deviceguy.c \
                gondar.c
RESOURCES     = gondarwizard.qrc

# install
target.path = .
INSTALLS += target

TEMPLATE = app
TARGET = gondar
INCLUDEPATH += .
INCLUDEPATH += ms-sys/inc
LIBS += -lsetupapi -lole32 -lgdi32 -lwininet -lshlwapi -lcrypt32 -lwintrust -lcomdlg32 -luuid
CFLAGS = -O0 -g
