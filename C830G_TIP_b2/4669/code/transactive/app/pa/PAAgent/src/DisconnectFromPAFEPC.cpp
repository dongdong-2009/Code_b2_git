/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/DisconnectFromPAFEPC.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/pa/PAAgent/src/DisconnectFromPAFEPC.h"

#include "core/utilities/src/DebugUtil.h"

#include "app/pa/PAAgent/src/PasTableManager.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{
DisconnectFromPAFEPC::DisconnectFromPAFEPC( TA_IRS_Bus::Scheduler& socketScheduler,
                                            TA_IRS_Bus::Scheduler& processScheduler,
                                            PasTableManager& pasTableManager,
                                            PASConnection& pasConnection )
    : m_socketScheduler( socketScheduler )
    , m_processScheduler( processScheduler )
    , m_pasTableManager( pasTableManager )
    , m_pasConnection( pasConnection )
{
}

DisconnectFromPAFEPC::DisconnectFromPAFEPC( ACE_Time_Value expiryTime,
                                            TA_IRS_Bus::Scheduler& socketScheduler,
                                            TA_IRS_Bus::Scheduler& processScheduler,
                                            PasTableManager& pasTableManager,
                                            PASConnection& pasConnection )
    : TA_IRS_Bus::IEvent( expiryTime )
    , m_socketScheduler( socketScheduler )
    , m_processScheduler( processScheduler )
    , m_pasTableManager( pasTableManager )
    , m_pasConnection( pasConnection )
{
}

DisconnectFromPAFEPC::~DisconnectFromPAFEPC()
{
}

void DisconnectFromPAFEPC::consume()
{
    LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "Consuming DisconnectFromPAFEPC event.");
    
    destroyAllWriteablePASTableEvents();
    delete this;
}

void DisconnectFromPAFEPC::cancel()
{
    delete this;
}

void DisconnectFromPAFEPC::destroyAllWriteablePASTableEvents()
{
    std::vector<AbstractWriteablePASTable*> writeableTables =
        this->m_pasTableManager.getAllWriteableTables();
    
    // destroy all the table write events
    for (std::vector<AbstractWriteablePASTable*>::iterator itr = writeableTables.begin();
    itr != writeableTables.end(); itr++)
    {
        AbstractWriteablePASTable* writeableTable = *itr;
        
        writeableTable->destroyCurrentWriteEvents();
    }
}

} // end namespace TA_IRS_App
