#ifndef PIPPIP_CRITICALALERT_H
#define PIPPIP_CRITICALALERT_H

#include "Alert.h"

namespace Pippip {

class CriticalAlert : public Alert {
        Q_OBJECT

    public:
        explicit CriticalAlert(const QString& title,
                                const QString& text,
                                const QString& detailed = "",
                                QObject *parent = 0);

};

} // namespace Pippip

#endif // PIPPIP_CRITICALALERT_H
