#-------------------------------------------------
#
# Project created by QtCreator 2017-03-03T15:28:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PippipMessaging-qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    User.cpp \
    AccountParameters.cpp \
    MessageDatabase.cpp \
    Contacts.cpp \
    Vault.cpp \
    NewAccountCreator.cpp \
    Authenticator.cpp \
    EnclaveRequester.cpp

HEADERS  += mainwindow.h \
    User.h \
    AccountParameters.h \
    MessageDatabase.h \
    Contacts.h \
    Vault.h \
    NewAccountCreator.h \
    Authenticator.h \
    EnclaveRequester.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11

unix:!macx: LIBS += -L/usr/local/lib64/ -lcoder -lcthread -lcryptokitty

RESOURCES += \
    images.qrc

