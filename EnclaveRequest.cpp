#include "EnclaveRequest.h"

namespace Pippip {

EnclaveRequest::EnclaveRequest(QObject *parent, SessionState *state)
: SessionTask(parent, state){
}

void EnclaveRequest::sessionComplete() {

}

} // namespace Pippip
