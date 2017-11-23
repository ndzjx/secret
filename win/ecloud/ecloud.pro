
QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += NOMINMAX
greaterThan(QT_MAJOR_VERSION, 4): QT += core-private widgets-private

TARGET = ecloud
TEMPLATE = app

HEADERS += \
    CUploadWidget.h

SOURCES += \
    CUploadWidget.cpp \
    main.cpp
