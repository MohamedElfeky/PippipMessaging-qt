#-------------------------------------------------
#
# Project created by QtCreator 2017-03-03T15:28:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PippipMessaging-qt
TEMPLATE = app
CONFIG += debug declarative_debug

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
    ClientAuthorized.cpp \
    EnclaveRequest.cpp \
    NicknamesDialog.cpp \
    NicknameManager.cpp \
    EnclaveResponse.cpp \
    LogoutRequest.cpp \
    ContactsDialog.cpp \
    ContactManager.cpp \
    AddContactDialog.cpp \
    EnterKeyFilter.cpp \
    NicknameHandler.cpp \
    WhitelistHandler.cpp \
    EmptyStringValidator.cpp

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
    ClientAuthorized.h \
    EnclaveRequest.h \
    NicknamesDialog.h \
    NicknameManager.h \
    SessionException.h \
    EnclaveResponse.h \
    EnclaveException.h \
    LogoutRequest.h \
    ContactsDialog.h \
    ContactManager.h \
    AddContactDialog.h \
    Contact.h \
    Nickname.h \
    Entity.h \
    ContactRequest.h \
    EnterKeyFilter.h \
    NicknameHandler.h \
    WhitelistHandler.h \
    EmptyStringValidator.h

FORMS    += mainwindow.ui \
    NewAccountDialog.ui \
    NewAccountHelpDialog.ui \
    LoginDialog.ui \
    NicknamesDialog.ui \
    ContactsDialog.ui \
    AddContactDialog.ui

QT += network

QMAKE_CXXFLAGS += -std=c++14


RESOURCES += \
    pippip.qrc

LIBPATH += /Users/stevebrenneis/Development/git/pippip/lib
INCLUDEPATH += /Users/stevebrenneis/Development/git/pippip/include
LIBS += -lcoder -lcryptokitty

DISTFILES += \
    license-template

