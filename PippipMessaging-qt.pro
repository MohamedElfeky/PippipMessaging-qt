#-------------------------------------------------
#
# Project created by QtCreator 2017-03-03T15:28:47
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PippipMessaging-qt
TEMPLATE = app
CONFIG += debug declarative_debug

SOURCES += main.cpp\
        mainwindow.cpp \
    AccountParameters.cpp \
    MessageDatabase.cpp \
    Vault.cpp \
    NewAccountCreator.cpp \
    Authenticator.cpp \
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
    EnclaveResponse.cpp \
    LogoutRequest.cpp \
    ContactsDialog.cpp \
    ContactManager.cpp \
    NicknameHandler.cpp \
    WhitelistHandler.cpp \
    EmptyStringValidator.cpp \
    ContactHandler.cpp \
    RequestHandler.cpp \
    MessageManager.cpp \
    ByteCodec.cpp \
    StringCodec.cpp \
    Constants.cpp \
    KeyFilter.cpp \
    Nicknames.cpp \
    Function.cpp \
    ContactRequests.cpp \
    RequestManager.cpp \
    EnclaveRequestTask.cpp \
    UpdateContactTask.cpp \
    SessionState.cpp \
    AddNicknameTask.cpp \
    LoadNicknamesTask.cpp \
    DeleteNicknameTask.cpp \
    UpdateNicknameTask.cpp \
    RequestContactTask.cpp \
    AddContactsTask.cpp \
    GetContactsTask.cpp \
    ContactAdapter.cpp \
    ContactDirector.cpp \
    SQLLiteContactsDatabaseImpl.cpp \
    DatabaseImpl.cpp \
    GetRequestsTask.cpp \
    StatusController.cpp \
    RESTTask.cpp \
    JsonValidator.cpp \
    ServerProgressPopover.cpp \
    AckRequestsTask.cpp

HEADERS  += mainwindow.h \
    AccountParameters.h \
    MessageDatabase.h \
    Vault.h \
    NewAccountCreator.h \
    Authenticator.h \
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
    SessionException.h \
    EnclaveResponse.h \
    EnclaveException.h \
    LogoutRequest.h \
    ContactsDialog.h \
    ContactManager.h \
    Contact.h \
    Nickname.h \
    Entity.h \
    ContactRequest.h \
    NicknameHandler.h \
    WhitelistHandler.h \
    EmptyStringValidator.h \
    ContactHandler.h \
    RequestHandler.h \
    MessageManager.h \
    ByteCodec.h \
    Message.h \
    MessageException.h \
    StringCodec.h \
    RSAKeys.h \
    Constants.h \
    KeyFilter.h \
    Nicknames.h \
    Function.h \
    ContactRequests.h \
    RequestManager.h \
    EnclaveRequestTask.h \
    UpdateContactTask.h \
    TaskException.h \
    AddNicknameTask.h \
    LoadNicknamesTask.h \
    DeleteNicknameTask.h \
    UpdateNicknameTask.h \
    RequestContactTask.h \
    AddContactsTask.h \
    GetContactsTask.h \
    ContactAdapter.h \
    DatabaseException.h \
    ContactDirector.h \
    SQLLiteContactsDatabaseImpl.h \
    ContactsDatabase.h \
    DatabaseImpl.h \
    GetRequestsTask.h \
    StatusController.h \
    RESTTask.h \
    ValidationException.h \
    JsonValidator.h \
    ServerProgressPopover.h \
    AckRequestsTask.h \
    PippipException.h

FORMS    += mainwindow.ui \
    NewAccountDialog.ui \
    NewAccountHelpDialog.ui \
    LoginDialog.ui \
    NicknamesDialog.ui \
    ContactsDialog.ui \
    AddContactDialog.ui \
    ServerProgressPopover.ui

QT += network

QMAKE_CXXFLAGS += -std=c++14


RESOURCES += \
    pippip.qrc

LIBPATH += /Users/stevebrenneis/Development/git/pippip/lib
INCLUDEPATH += /Users/stevebrenneis/Development/git/pippip/include
LIBS += -lcoder -lcryptokitty

DISTFILES += \
    license-template

