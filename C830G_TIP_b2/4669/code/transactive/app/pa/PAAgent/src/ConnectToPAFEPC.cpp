/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/ConnectToPAFEPC.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/pa/PAAgent/src/ConnectToPAFEPC.h"

#include "core/utilities/src/DebugUtil.h"

#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/Table100.h"
#include "app/pa/PAAgent/src/Table200.h"
#include "app/pa/PAAgent/src/Table1200.h"
#include "app/pa/PAAgent/src/Table201.h"
#include "app/pa/PAAgent/src/Table1201.h"
#include "app/pa/PAAgent/src/Table202.h"
#include "app/pa/PAAgent/src/Table204.h"
#include "app/pa/PAAgent/src/Table1204.h"
#include "app/pa/PAAgent/src/Table301.h"
#include "app/pa/PAAgent/src/Table1301.h"
#include "app/pa/PAAgent/src/Table302.h"
#include "app/pa/PAAgent/src/Table303.h"
#include "app/pa/PAAgent/src/Table1303.h"
#include "app/pa/PAAgent/src/Table350.h"
#include "app/pa/PAAgent/src/Table550.h"
#include "app/pa/PAAgent/src/Table560.h"
#include "app/pa/PAAgent/src/Table570.h"
#include "app/pa/PAAgent/src/Table590.h"
#include "app/pa/PAAgent/src/Table603.h"
#include "app/pa/PAAgent/src/Table604.h"
#include "app/pa/PAAgent/src/Table610.h"
#include "app/pa/PAAgent/src/Table1610.h"
#include "app/pa/PAAgent/src/Table630.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{
ConnectToPAFEPC::ConnectToPAFEPC( TA_IRS_Bus::Scheduler& socketScheduler,
                                  TA_IRS_Bus::Scheduler& processScheduler,
                                  PasTableManager& pasTableManager,
                                  PASConnection& pasConnection )
    : TA_IRS_Bus::IEvent( ACE_OS::gettimeofday() + ACE_Time_Value( 1 ) )
    , m_socketScheduler( socketScheduler )
    , m_processScheduler( processScheduler )
    , m_pasTableManager( pasTableManager )
    , m_pasConnection( pasConnection )
{
}

ConnectToPAFEPC::ConnectToPAFEPC( ACE_Time_Value expiryTime,
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

ConnectToPAFEPC::~ConnectToPAFEPC()
{
}

void ConnectToPAFEPC::consume()
{
    LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "Consuming ConnectToPAFEPC event.");

    // Remove existing events.
    this->m_socketScheduler.clear();

    // liuyu++ #243 terminate error
    if (this->m_pasConnection.getTerminate() == true)
    {
        return;
    }
    // ++liuyu #243 terminate error

    this->m_pasConnection.ensureConnected();

    // Schedule the fast polling events.
    this->m_socketScheduler.post( new ReadTable302( this->m_pasTableManager.getTable302(),
                                                    this->m_socketScheduler,
                                                    this->m_processScheduler,
                                                    this->m_pasTableManager ) );
    
    // Schedule non recurring events.
    this->m_socketScheduler.post( new ReadTable100( this->m_pasTableManager.getTable100(),
                                                    this->m_pasTableManager.getTable560(),
                                                    this->m_socketScheduler,
                                                    this->m_processScheduler,
                                                    this->m_pasTableManager ) );
    
    this->m_socketScheduler.post( new ReadTable202( this->m_pasTableManager.getTable202(),
                                                    this->m_pasTableManager.getTable560(),
                                                    this->m_socketScheduler,
                                                    this->m_processScheduler,
                                                    this->m_pasTableManager ) );
    
    this->m_socketScheduler.post( new ReadTable550( this->m_pasTableManager.getTable550(),
                                                    this->m_pasTableManager.getTable560(),
                                                    this->m_socketScheduler,
                                                    this->m_processScheduler,
                                                    this->m_pasTableManager ) );
    
    this->m_socketScheduler.post( new ReadTable570( this->m_pasTableManager.getTable570(),
                                                    this->m_pasTableManager.getTable560(),
                                                    this->m_socketScheduler,
                                                    this->m_processScheduler,
                                                    this->m_pasTableManager ) );
    
    this->m_socketScheduler.post( new ReadTable590( this->m_pasTableManager.getTable590(),
                                                    this->m_pasTableManager.getTable560(),
                                                    this->m_socketScheduler,
                                                    this->m_processScheduler,
                                                    this->m_pasTableManager ) );
    
    this->m_socketScheduler.post( new ReadTable603( this->m_pasTableManager.getTable603(),
                                                    this->m_pasTableManager.getTable560(),
                                                    this->m_socketScheduler,
                                                    this->m_processScheduler,
                                                    this->m_pasTableManager ) );
    
    this->m_socketScheduler.post( new ReadTable604( this->m_pasTableManager.getTable604(),
                                                    this->m_pasTableManager.getTable560(),
                                                    this->m_socketScheduler,
                                                    this->m_processScheduler,
                                                    this->m_pasTableManager ) );
}

void ConnectToPAFEPC::repostAllWriteablePASTableEvents()
{
    std::vector<AbstractWriteablePASTable*> writeableTables =
        this->m_pasTableManager.getAllWriteableTables();
    
    // repost all the table write events
    for (std::vector<AbstractWriteablePASTable*>::iterator itr = writeableTables.begin();
    itr != writeableTables.end(); itr++)
    {
        AbstractWriteablePASTable* writeableTable = *itr;
        
        writeableTable->repostCurrentWriteEvents(m_socketScheduler);
    }
}
ConnectToMasterPAFEPC::ConnectToMasterPAFEPC( TA_IRS_Bus::Scheduler& socketScheduler,
                                              TA_IRS_Bus::Scheduler& processScheduler,
                                              PasTableManager& pasTableManager,
                                              PASConnection& pasConnection )
    : ConnectToPAFEPC( socketScheduler,
                       processScheduler,
                       pasTableManager,
                       pasConnection )
{
}

ConnectToMasterPAFEPC::ConnectToMasterPAFEPC( ACE_Time_Value expiryTime,
                                              TA_IRS_Bus::Scheduler& socketScheduler,
                                              TA_IRS_Bus::Scheduler& processScheduler,
                                              PasTableManager& pasTableManager,
                                              PASConnection& pasConnection )
    : ConnectToPAFEPC( expiryTime,
                       socketScheduler,
                       processScheduler,
                       pasTableManager,
                       pasConnection )

{
}


ConnectToMasterPAFEPC::~ConnectToMasterPAFEPC()
{
}

void ConnectToMasterPAFEPC::consume()
{
    try
    {
        ConnectToPAFEPC::consume();

        // liuyu++ #243 terminate error
        if (this->m_pasConnection.getTerminate() == true)
        {
            return;
        }
        // ++liuyu #243 terminate error

        // Schedule the fast polling events.
		// TD 17013
		this->m_pasTableManager.getTable1200().setInitalThreadValue(true);
		// TD 17013
 
        this->m_socketScheduler.post( new ReadTable1200( this->m_pasTableManager.getTable1200(),
                                                         this->m_socketScheduler,
                                                         this->m_processScheduler,
                                                         this->m_pasTableManager ) );
        this->m_socketScheduler.post( new ReadTable560( this->m_pasTableManager.getTable560(),
                                                        this->m_socketScheduler,
                                                        this->m_processScheduler,
                                                        this->m_pasTableManager ) );

        // Schedule an initial read
        this->m_socketScheduler.post( new ReadTable1204( this->m_pasTableManager.getTable1204(),
                                                         this->m_pasTableManager.getTable560(),
                                                         this->m_socketScheduler,
                                                         this->m_processScheduler,
                                                         this->m_pasTableManager ) );
//        this->m_socketScheduler.post( new ReadTable610( this->m_pasTableManager.getTable610(),
//                                                        this->m_pasTableManager.getTable560(),
//                                                        this->m_socketScheduler,
//                                                        this->m_processScheduler,
//                                                        this->m_pasTableManager ) );
        this->m_socketScheduler.post( new ReadTable1610( this->m_pasTableManager.getTable1610(),
                                                         this->m_pasTableManager.getTable560(),
                                                         this->m_socketScheduler,
                                                         this->m_processScheduler,
                                                         this->m_pasTableManager ) );

        repostAllWriteablePASTableEvents();
    }
    catch( TA_Base_Core::PasConnectionException& )
    {
        this->m_socketScheduler.post( new ConnectToMasterPAFEPC( ACE_OS::gettimeofday() + ACE_Time_Value( 10, 0 ),
                                                                 this->m_socketScheduler,
                                                                 this->m_processScheduler,
                                                                 this->m_pasTableManager,
                                                                 this->m_pasConnection ) );
    }
    delete this;
}

void ConnectToMasterPAFEPC::cancel()
{
    delete this;
}


ConnectToStationPAFEPC::ConnectToStationPAFEPC( TA_IRS_Bus::Scheduler& socketScheduler,
                                              TA_IRS_Bus::Scheduler& processScheduler,
                                              PasTableManager& pasTableManager,
                                              PASConnection& pasConnection )
    : ConnectToPAFEPC( socketScheduler,
                       processScheduler,
                       pasTableManager,
                       pasConnection )
{
}

ConnectToStationPAFEPC::ConnectToStationPAFEPC( ACE_Time_Value expiryTime,
                                              TA_IRS_Bus::Scheduler& socketScheduler,
                                              TA_IRS_Bus::Scheduler& processScheduler,
                                              PasTableManager& pasTableManager,
                                              PASConnection& pasConnection )
    : ConnectToPAFEPC( expiryTime,
                       socketScheduler,
                       processScheduler,
                       pasTableManager,
                       pasConnection )

{
}


ConnectToStationPAFEPC::~ConnectToStationPAFEPC()
{
}

void ConnectToStationPAFEPC::consume()
{
    try
    {
        ConnectToPAFEPC::consume();

        // liuyu++ #243 terminate error
        if (this->m_pasConnection.getTerminate() == true)
        {
            return;
        }
        // ++liuyu #243 terminate error
        // Schedule the fast polling events.
		// TD 17013
		this->m_pasTableManager.getTable200().setInitalThreadValue(true);
		// TD 17013

        this->m_socketScheduler.post( new ReadTable200( this->m_pasTableManager.getTable200(),
                                                        this->m_socketScheduler,
                                                        this->m_processScheduler,
                                                        this->m_pasTableManager ) );
        this->m_socketScheduler.post( new ReadTable560( this->m_pasTableManager.getTable560(),
                                                        this->m_socketScheduler,
                                                        this->m_processScheduler,
                                                        this->m_pasTableManager ) );

        // Schedule an initial read
        this->m_socketScheduler.post( new ReadTable204( this->m_pasTableManager.getTable204(),
                                                        this->m_pasTableManager.getTable560(),
                                                        this->m_socketScheduler,
                                                        this->m_processScheduler,
                                                        this->m_pasTableManager ) );
        this->m_socketScheduler.post( new ReadTable610( this->m_pasTableManager.getTable610(),
                                                        this->m_pasTableManager.getTable560(),
                                                        this->m_socketScheduler,
                                                        this->m_processScheduler,
                                                        this->m_pasTableManager ) );

        repostAllWriteablePASTableEvents();
    }
    catch( TA_Base_Core::PasConnectionException& )
    {
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "Rescheduling ConnectToPAFEPC event.");
        this->m_socketScheduler.post( new ConnectToStationPAFEPC( ACE_OS::gettimeofday() + ACE_Time_Value( 10, 0 ),
                                                                 this->m_socketScheduler,
                                                                 this->m_processScheduler,
                                                                 this->m_pasTableManager,
                                                                 this->m_pasConnection ) );
    }
    delete this;
}

void ConnectToStationPAFEPC::cancel()
{
    LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "Cancelling ConnectToPAFEPC event.");
    delete this;
}



} // namespace TA_IRS_App

