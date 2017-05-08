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
                deviceguy.cc
RESOURCES     = gondarwizard.qrc

win32 {
  INCLUDEPATH += ms-sys/inc
  SOURCES += gondar.c
} else {
  QMAKE_CXXFLAGS += -Wmissing-declarations
  SOURCES += stubs.cc  
}


# install
target.path = .
INSTALLS += target

TEMPLATE = app
TARGET = gondar

win32 {
  LIBS += -lsetupapi -lole32 -lgdi32 -lwininet -lshlwapi -lcrypt32 -lwintrust -lcomdlg32 -luuid
}
