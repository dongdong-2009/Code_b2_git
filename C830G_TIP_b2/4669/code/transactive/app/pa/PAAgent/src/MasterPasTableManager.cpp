/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/MasterPasTableManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2009/05/13 15:05:48 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "app/pa/PAAgent/src/MasterPasTableManager.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{

// ExceptionChecked
MasterPasTableManager::MasterPasTableManager( unsigned long locationKey ) 
    : PasTableManager( locationKey )

{
    FUNCTION_ENTRY("Constructor");

    m_table1201 = new Table1201( this->m_socketScheduler, this->m_processScheduler, *this);
    m_table1204 = new Table1204();
    m_table1301 = new Table1301( this->m_socketScheduler, this->m_processScheduler, *this);
    m_table1303 = new Table1303( locationKey );
    m_table1200 = new Table1200(m_table202, m_table302, m_table1303);
    m_table1610 = new Table1610();
    m_table560  = new Table560(this->m_socketScheduler,
                               this->m_processScheduler,
                               m_table100,
                               m_table202,
                               NULL,
                               m_table1204,
                               NULL,
                               m_table1303,
                               m_table550,
                               m_table570,
                               m_table590,
                               m_table603,
                               m_table604,
                               NULL,
                               m_table1610,
                               *this );

    FUNCTION_EXIT;
}

Table200& MasterPasTableManager::getTable200()
{
    TA_ASSERT(false, "Not StationPasTable instance.");
    return *(this->m_table200);
}

Table1200& MasterPasTableManager::getTable1200()
{
    return *(this->m_table1200);
}

Table201& MasterPasTableManager::getTable201()
{
    TA_ASSERT(false, "Not StationPasTable instance.");
    return *(this->m_table201);
}

Table1201& MasterPasTableManager::getTable1201()
{
    return *(this->m_table1201);
}

Table204& MasterPasTableManager::getTable204()
{
    TA_ASSERT(false, "Not StationPasTable instance.");
    return *(this->m_table204);
}

Table1204& MasterPasTableManager::getTable1204()
{
    return *(this->m_table1204);
}

Table301& MasterPasTableManager::getTable301()
{
    TA_ASSERT(false, "Not StationPasTable instance.");
    return *(this->m_table301);
}

Table1301& MasterPasTableManager::getTable1301()
{
    return *(this->m_table1301);
}

Table303& MasterPasTableManager::getTable303()
{
    TA_ASSERT(false, "Not StationPasTable instance.");
    return *(this->m_table303);
}

Table1303& MasterPasTableManager::getTable1303()
{
    return *(this->m_table1303);
}

Table610& MasterPasTableManager::getTable610()
{
    TA_ASSERT(false, "Not StationPasTable instance.");
    return *(this->m_table610);
}

Table1610& MasterPasTableManager::getTable1610()
{
    return *(this->m_table1610);
}


ConnectToPAFEPC* MasterPasTableManager::getConnectEvent()
{
    LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugInfo, "Returning MASTER connect event.");
    return new ConnectToMasterPAFEPC( this->m_socketScheduler,
                                      this->m_processScheduler,
                                      *this,
                                      *(PASConnection::getInstance()) );
}

InvalidatePASDataPoints* MasterPasTableManager::getInvalidateEvent()
{
    LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugInfo, "Returning MASTER invalidate event.");
    return new InvalidateMasterPASDataPoints( this->m_processScheduler, *this );
}

std::vector<AbstractWriteablePASTable*> MasterPasTableManager::getAllWriteableTables()
{
    std::vector<AbstractWriteablePASTable*> writeableTables;

    writeableTables.push_back(this->m_table1201);
    writeableTables.push_back(this->m_table1301);
    writeableTables.push_back(this->m_table350);
    writeableTables.push_back(this->m_table560);
    writeableTables.push_back(this->m_table570);
    writeableTables.push_back(this->m_table604);
    writeableTables.push_back(this->m_table630);

    return writeableTables;
}

// ExceptionChecked
MasterPasTableManager::~MasterPasTableManager()
{
    FUNCTION_ENTRY("Destructor");

    FUNCTION_EXIT;
}

void MasterPasTableManager::setMonitor()
{
    PasTableManager::setMonitor();
}

void MasterPasTableManager::setControl()
{
    PasTableManager::setControl();
}

//wuzhongyi CL-19136
//Table1301 Global Messages Sequence Broadcast Request
void MasterPasTableManager::forceTerminateAllPA()
{
	FUNCTION_ENTRY("forceTerminateAllPA");

	TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage nullZones;
	nullZones.length(0);
	for( int messageSeqId =1; messageSeqId <=4; messageSeqId++ )
	{
		//can trigger exception
		try
		{
			getTable1301().setTableDataAndWrite( messageSeqId, 0, 0, 0, 0, false, nullZones);		
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Table1301 setTableDataAndWrite messageSeqId=%d  announceId=0 periodInsecs=0 starttime=0 stoptime=0 isEventTrigger=false", messageSeqId);
		}
		catch(TA_Base_Core::TransactiveException& err)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Table1301 setTableDataAndWrite ERROR messageSeqId=%d  announceId=0 periodInsecs=0 starttime=0 stoptime=0 isEventTrigger=false", messageSeqId);
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "unknown exception when Table1301 setTableDataAndWrite messageSeqId=%d", messageSeqId);
		}
	}
	FUNCTION_EXIT;
}

} // namespace TA_IRS_App

