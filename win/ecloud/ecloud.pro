
QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX
greaterThan(QT_MAJOR_VERSION, 4): QT += core-private widgets-private

TEMPLATE = app

HEADERS += \
    CSettingWidget.h \
    CBrowserWidget.h \
    CUploadWidget.h

SOURCES += \
    CSettingWidget.cpp \
    main.cpp \
    CBrowserWidget.cpp \
    CUploadWidget.cpp
