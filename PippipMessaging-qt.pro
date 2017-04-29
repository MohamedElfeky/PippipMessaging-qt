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
    EnclaveRequester.cpp \
    NewAccountDialog.cpp \
    NewAccountHelpDialog.cpp \
    RESTHandler.cpp \
    RESTTimer.cpp \
    ParameterGenerator.cpp \
    EntropyStream.cpp \
    UDPListener.cpp \
    SessionTask.cpp \
    NewAccountRequest.cpp \
    NewAccountResponse.cpp \
    NewAccountFinish.cpp \
    NewAccountFinal.cpp \
    LoginDialog.cpp \
    Alert.cpp \
    CriticalAlert.cpp \
    AuthRequest.cpp \
    AuthChallenge.cpp \
    AuthResponse.cpp \
    ServerAuthorized.cpp \
    ClientAuthorized.cpp

HEADERS  += mainwindow.h \
    User.h \
    AccountParameters.h \
    MessageDatabase.h \
    Contacts.h \
    Vault.h \
    NewAccountCreator.h \
    Authenticator.h \
    EnclaveRequester.h \
    NewAccountDialog.h \
    SessionState.h \
    NewAccountHelpDialog.h \
    RESTHandler.h \
    RESTTimer.h \
    ParameterGenerator.h \
    EntropyStream.h \
    UDPListener.h \
    SessionTask.h \
    PostPacket.h \
    NewAccountRequest.h \
    NewAccountResponse.h \
    NewAccountFinish.h \
    NewAccountFinal.h \
    VaultException.h \
    LoginDialog.h \
    Alert.h \
    CriticalAlert.h \
    AuthRequest.h \
    AuthChallenge.h \
    AuthResponse.h \
    ServerAuthorized.h \
    ClientAuthorized.h

FORMS    += mainwindow.ui \
    NewAccountDialog.ui \
    NewAccountHelpDialog.ui \
    LoginDialog.ui

QT += network

QMAKE_CXXFLAGS += -std=c++11 -I/usr/local/include/CryptoKitty-C

unix:!macx: LIBS += -L/usr/local/lib64/ -lcoder -lcthread -lcryptokitty

RESOURCES += \
    pippip.qrc

