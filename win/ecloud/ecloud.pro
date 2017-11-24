
QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX
greaterThan(QT_MAJOR_VERSION, 4): QT += core-private widgets-private

TEMPLATE = app

DESTDIR = ../../run

INCLUDEPATH += ../include
INCLUDEPATH += ../../src/mod
QMAKE_LIBDIR += ../lib

CONFIG(debug, debug|release){
    win32:TARGET=$$join(TARGET,,,d)
    win32:QMAKE_CXXFLAGS_DEBUG += -Fd$$DESTDIR/$$TARGET$$().pdb
}

HEADERS += \
    CUploadWidget.h \
    CBrowserWidget.h \
    CSettingWidget.h

SOURCES += \
    CUploadWidget.cpp \
    main.cpp \
    CBrowserWidget.cpp \
    CSettingWidget.cpp

RESOURCES  = \
    ecloud.qrc
