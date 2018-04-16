#pragma once
#include "Singleton.h"
#include "core/process_management/idl/src/IHeartbeatReceiverCorbaDef.h"
#include "core/corba/src/ServantBase.h"

class SystemControllerMonitor
    : public virtual POA_TA_Base_Core::IHeartbeatReceiverCorbaDef,
      public virtual TA_Base_Core::ServantBase,
      public Singleton<SystemControllerMonitor>
{
public:

    SystemControllerMonitor();
    virtual void receiveHeartbeat(const char* host) {}
};
