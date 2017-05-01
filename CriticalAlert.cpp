#include "CriticalAlert.h"

namespace Pippip {

CriticalAlert::CriticalAlert(const QString& title,
                            const QString& text,
                            const QString& detailed,
                            QObject *parent)
: Alert(title, text, detailed, QMessageBox::Critical, parent) {
}

} // namespace Pippip
