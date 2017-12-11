
QT       += core gui widgets network

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
	echat.h \
    MainForm.h \
    AudioForm.h \
    Widget.h \
    TextEdit.h \
    GHYTitleBar.h \
    MessageConsts.h \
    MessageScrollWidget.h \
    MessageWidget.h \
    eChatWidget.h \
    CSettingWidget.h \
    CAddEmailDialog.h \
    CHackConsts.h \
    myemotionitemwidget.h \
    myemotionwidget.h \
    myemotionwindow.h \
    Emotion.h

SOURCES += \
    main.cpp \
    echat.cpp \
    MainForm.cpp \
    AudioForm.cpp \
    Widget.cpp \
    TextEdit.cpp \
    GHYTitleBar.cpp \
    MessageConsts.cpp \
    MessageScrollWidget.cpp \
    MessageWidget.cpp \
    eChatWidget.cpp \
    CSettingWidget.cpp \
    CAddEmailDialog.cpp \
    CHackConsts.cpp \
    myemotionitemwidget.cpp \
    myemotionwidget.cpp \
    myemotionwindow.cpp \
    Emotion.cpp

RESOURCES  = \
    echat.qrc \
    myemotionwindow.qrc

FORMS += \
    myemotionwindow.ui
