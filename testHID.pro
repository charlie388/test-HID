#-------------------------------------------------
#
# Project created by QtCreator 2014-08-07T14:43:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testHID
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    hid_pnp.cpp

HEADERS  += mainwindow.h \
    hid_pnp.h

FORMS    += mainwindow.ui

#-------------------------------------------------
# Add the Signal11's hidapi library that was
# created
#-------------------------------------------------
macx: LIBS += -L../HIDAPI/mac -lHIDAPI
win32: LIBS += -L../HIDAPI/windows -lHIDAPI
unix: !macx: LIBS += -L../HIDAPI/linux -lHIDAPI

#-------------------------------------------------
# Make sure to add the required libraries or
# frameoworks for the hidapi to work depending on
# what OS is being used
#-------------------------------------------------
macx: LIBS += -framework CoreFoundation -framework IOkit
win32: LIBS += -lSetupAPI
unix: !macx: LIBS += -lusb-1.0

win32: RC_ICONS = icon-4.ico