
QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX
greaterThan(QT_MAJOR_VERSION, 4): QT += core-private widgets-private

TEMPLATE = app

DESTDIR = ../bin

CONFIG(debug, debug|release){
    win32:TARGET=$$join(TARGET,,,d)
    win32:QMAKE_CXXFLAGS_DEBUG += -Fd$$DESTDIR/$$TARGET$$().pdb
}

HEADERS += \
    CSettingWidget.h \
    CBrowserWidget.h \
    CUploadWidget.h

SOURCES += \
    CSettingWidget.cpp \
    main.cpp \
    CBrowserWidget.cpp \
    CUploadWidget.cpp
