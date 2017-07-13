#include "RequestHandler.h"
#include "ui_ContactsDialog.h"

RequestHandler::RequestHandler(Ui::ContactsDialog *u, QObject *parent)
: QObject(parent),
  ui(u) {

}

void RequestHandler::setRequests(const Pippip::RequestList &list) {

    requests = list;

}
