/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/PASWriteEvent.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PasExceptions.h"
#include "core/threads/src/Thread.h"
#include "app/pa/PAAgent/src/PASWriteEvent.h"
#include "app/pa/PAAgent/src/PasTableManager.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{
    ACE_Future<int> PASWriteEvent::ignoredFuture;

    PASWriteEvent::PASWriteEvent( AbstractWriteablePASTable& writeableTable,
                                  ACE_Future<int> future,
                                  TA_IRS_Bus::Scheduler& socketScheduler,
                                  TA_IRS_Bus::Scheduler& processScheduler,
                                  IPasEventSource& eventSource )
                                  : PASEvent(socketScheduler, processScheduler, eventSource),
                                  m_future(future),
                                  m_writeableTable(writeableTable)
    {
    }

    PASWriteEvent::PASWriteEvent( AbstractWriteablePASTable& writeableTable,
                                  TA_IRS_Bus::Scheduler& socketScheduler,
                                  TA_IRS_Bus::Scheduler& processScheduler,
                                  IPasEventSource& eventSource )
                                  : PASEvent(socketScheduler, processScheduler, eventSource),
                                  m_future(ignoredFuture),
                                  m_writeableTable(writeableTable)
    {
    }

    PASWriteEvent::PASWriteEvent( ACE_Time_Value timeToExpire,
                                  AbstractWriteablePASTable& writeableTable,
                                  ACE_Future<int> future,
                                  TA_IRS_Bus::Scheduler& socketScheduler,
                                  TA_IRS_Bus::Scheduler& processScheduler,
                                  IPasEventSource& eventSource )
                                  : PASEvent(timeToExpire, socketScheduler, processScheduler, eventSource)
                                  , m_future(future)
                                  , m_writeableTable(writeableTable)
    {
    }
    
    PASWriteEvent::PASWriteEvent( ACE_Time_Value timeToExpire,
                                  AbstractWriteablePASTable& writeableTable,
                                  TA_IRS_Bus::Scheduler& socketScheduler,
                                  TA_IRS_Bus::Scheduler& processScheduler,
                                  IPasEventSource& eventSource )
                                  : PASEvent(timeToExpire, socketScheduler, processScheduler, eventSource)
                                  , m_future(ignoredFuture)
                                  , m_writeableTable(writeableTable)
    {
    }

    PASWriteEvent::~PASWriteEvent()
    {
        this->m_future.set( AbstractPASTable::PAS_ERR_GENERIC );
    }

    void PASWriteEvent::consume()
    {
        try
        {
            writeTable();
            this->m_writeableTable.removeWriteEvent(*this);
        }
        catch( TA_Base_Core::PasConnectionException& pce )
        {
            PASConnectionExceptionReason reason = pce.getReason();

            // Only set the future to an error if the pas connection could
            // not reconnect
            switch (reason)
            {
            case UNABLE_TO_CONNECT:
                this->m_socketScheduler.clear();
                this->m_socketScheduler.post( this->m_pasEventSource.getDisconnectEvent() );
                this->m_processScheduler.post( this->m_pasEventSource.getInvalidateEvent() );
                this->m_socketScheduler.post( this->m_pasEventSource.getConnectEvent() );
                break;

            case DISCONNECTED:
                this->m_socketScheduler.clear();
                this->m_processScheduler.post( this->m_pasEventSource.getInvalidateEvent() );
                this->m_socketScheduler.post( this->m_pasEventSource.getConnectEvent() );
                break;
            }
        }
    }

    void PASWriteEvent::cancel()
    {
    }
}
