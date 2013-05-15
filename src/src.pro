include(../common.pri)
TEMPLATE = app
QT += dbus
CONFIG += debug \
    meegotouch \
    link_pkgconfig \
    dcpconfig
PKGCONFIG += libsocialweb-qt marmazon
INCLUDEPATH += /usr/include/libsocialweb
MOC_DIR = .moc
OBJECTS_DIR = .obj
MGEN_OUTDIR = .gen

include($$[QT_INSTALL_DATA]/mkspecs/dcpconfig.pri)

#Not working right now - not sure why...
#INCLUDEPATH += $$DCP_INSTALL_HEADERS
#LIBS += -L$$DCP_INSTALL_LIB -lduicontrolpanel
INCLUDEPATH += /usr/include/duicontrolpanel
LIBS += -lduicontrolpanel

DEFINES += THEMEDIR=\\\"\"$$M_PREFIX/share/themes/base/meegotouch/$$TARGET/\"\\\"
target.path = $$M_INSTALL_BIN

STYLE_HEADERS += switemlistitemstyle.h

HEADERS += $$STYLE_HEADERS \
    main.h \
    switemlistpage.h \
    switemlistitem.h \
    switemlistitemcreator.h \
    nocfgsvcpage.h \
    socialwebapp.h \
    switempage.h

SOURCES += main.cpp \
    switemlistpage.cpp \
    switemlistitem.cpp \
    switemlistitemcreator.cpp \
    nocfgsvcpage.cpp \
    socialwebapp.cpp \
    switempage.cpp

# Install
INSTALLS += target
