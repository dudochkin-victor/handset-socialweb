include(common.pri)
TEMPLATE = subdirs
SUBDIRS += src translations

OTHER_FILES += theme/base/style/meego-handset-socialweb.css

# Themes
basetheme.files = theme/base/*
basetheme.path = $$M_THEME_DIR/base/meegotouch/$$TARGET/

# Desktop
desktop_entry.files = share/applications/meego-handset-socialweb.desktop
desktop_entry.path = $$M_INSTALL_DATA/applications

# DBUS Service files
dbus_service.files = share/dbus-1/services/*
dbus_service.path = $$M_INSTALL_DATA/dbus-1/services

# Icon file
icon.files = share/pixmaps/icons-Applications-socialweb.png
icon.path = $$M_INSTALL_DATA/pixmaps

# Install
INSTALLS += desktop_entry \
    dbus_service \
    basetheme \
    icon
