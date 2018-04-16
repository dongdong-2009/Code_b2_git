#pragma once
#include "bus/ats/ats_agent/IDL/src/IAtsTrainCorbaDef.h"
#include "core/threads/src/Thread.h"
#include "core/naming/src/NamedObject.h"
#include <ace/Condition_Thread_Mutex.h>
#include <ace/Thread_Mutex.h>

namespace TA_IRS_App
{


typedef TA_Base_Core::NamedObject < TA_IRS_Bus::IAtsTrainCorbaDef,
TA_IRS_Bus::IAtsTrainCorbaDef_ptr,
TA_IRS_Bus::IAtsTrainCorbaDef_var > AtsTrainCorbaObject;

class ITrainList;

class AtsTrainDataPoller : public TA_Base_Core::Thread
{
public:
    AtsTrainDataPoller(int pollingInterval, ITrainList& trainList); // second
    ~AtsTrainDataPoller();
    virtual void run();
    virtual void terminate();
private:
    AtsTrainCorbaObject m_atsTrainCorbaObject;
    int m_pollingInterval;
    bool m_shouldRunning;
    ITrainList& m_trainList;

    ACE_Thread_Mutex m_lock;
    ACE_Condition_Thread_Mutex m_cond;

};

}
