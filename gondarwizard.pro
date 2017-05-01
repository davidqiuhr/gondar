QT = core network widgets

HEADERS       = gondarwizard.h \
                downloader.h
SOURCES       = gondarwizard.cc \
                downloader.cc \
                main.cc
RESOURCES     = gondarwizard.qrc

# install
target.path = .
INSTALLS += target
