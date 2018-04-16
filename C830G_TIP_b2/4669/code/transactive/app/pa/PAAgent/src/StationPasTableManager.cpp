/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/StationPasTableManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/05/13 15:05:48 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "app/pa/PAAgent/src/StationPasTableManager.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{

// ExceptionChecked
StationPasTableManager::StationPasTableManager( unsigned long locationKey ) 
    : PasTableManager( locationKey )

{
    FUNCTION_ENTRY("Constructor");

    m_table201 = new Table201( this->m_socketScheduler, this->m_processScheduler, *this );
    m_table204 = new Table204();
    m_table301 = new Table301( this->m_socketScheduler, this->m_processScheduler, *this );
    m_table303 = new Table303( locationKey );
    m_table200 = new Table200(m_table202, m_table302, m_table303);
    m_table610 = new Table610();
    m_table560 = new Table560( this->m_socketScheduler,
                               this->m_processScheduler,
                               m_table100,
                               m_table202,
                               m_table204,
                               NULL,
                               m_table303,
                               NULL,
                               m_table550,
                               m_table570,
                               m_table590,
                               m_table603,
                               m_table604,
                               m_table610,
                               NULL,
                               *this );

    FUNCTION_EXIT;
}

Table200& StationPasTableManager::getTable200()
{
    return *(this->m_table200);
}

Table1200& StationPasTableManager::getTable1200()
{
    TA_ASSERT(false, "Not MasterPasTable instance.");
    return *(this->m_table1200);
}

Table201& StationPasTableManager::getTable201()
{
    return *(this->m_table201);
}

Table1201& StationPasTableManager::getTable1201()
{
    TA_ASSERT(false, "Not MasterPasTable instance.");
    return *(this->m_table1201);
}

Table204& StationPasTableManager::getTable204()
{
    return *(this->m_table204);
}

Table1204& StationPasTableManager::getTable1204()
{
    TA_ASSERT(false, "Not MasterPasTable instance.");
    return *(this->m_table1204);
}

Table301& StationPasTableManager::getTable301()
{
    return *(this->m_table301);
}

Table1301& StationPasTableManager::getTable1301()
{
    TA_ASSERT(false, "Not MasterPasTable instance.");
    return *(this->m_table1301);
}

Table303& StationPasTableManager::getTable303()
{
    return *(this->m_table303);
}

Table1303& StationPasTableManager::getTable1303()
{
    TA_ASSERT(false, "Not MasterPasTable instance.");
    return *(this->m_table1303);
}

Table610& StationPasTableManager::getTable610()
{
    return *(this->m_table610);
}

Table1610& StationPasTableManager::getTable1610()
{
    TA_ASSERT(false, "Not MasterPasTable instance.");
    return *(this->m_table1610);
}

ConnectToPAFEPC* StationPasTableManager::getConnectEvent()
{
    LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugInfo, "Returning STATION connect event.");
    return new ConnectToStationPAFEPC( this->m_socketScheduler,
                                       this->m_processScheduler,
                                       *this,
                                       *(PASConnection::getInstance()) );
}

InvalidatePASDataPoints* StationPasTableManager::getInvalidateEvent()
{
    LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugInfo, "Returning STATION invalidate event.");
    return new InvalidateStationPASDataPoints( this->m_processScheduler, *this );
}

std::vector<AbstractWriteablePASTable*> StationPasTableManager::getAllWriteableTables()
{
    std::vector<AbstractWriteablePASTable*> writeableTables;

    writeableTables.push_back(this->m_table201);
    writeableTables.push_back(this->m_table301);
    writeableTables.push_back(this->m_table350);
    writeableTables.push_back(this->m_table560);
    writeableTables.push_back(this->m_table570);
    writeableTables.push_back(this->m_table604);
    writeableTables.push_back(this->m_table630);

    return writeableTables;
}

// ExceptionChecked
StationPasTableManager::~StationPasTableManager()
{
    FUNCTION_ENTRY("Destructor");

    FUNCTION_EXIT;
}

void StationPasTableManager::setMonitor()
{
    PasTableManager::setMonitor();
}

void StationPasTableManager::setControl()
{
    PasTableManager::setControl();
}

//wuzhongyi CL-19136
//Table301 Messages Sequence Broadcast Request
void StationPasTableManager::forceTerminateAllPA()
{
	FUNCTION_ENTRY("forceTerminateAllPA");

	TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage nullZones;
	nullZones.length(0);
	for( int messageSeqId =1; messageSeqId <=4; messageSeqId++ )
	{
		try
		{
			//can trigger exception
			getTable301().setTableDataAndWrite( messageSeqId, 0, 0, 0,	0, false, nullZones );	
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Table301 setTableDataAndWrite messageSeqId=%d  announceId=0 announceId=0 periodInsecs=0 starttime=0 stoptime=0 isEventTrigger=false", messageSeqId);
		}
		catch(TA_Base_Core::TransactiveException& err)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Table301 setTableDataAndWrite ERROR messageSeqId=%d announceId=0 periodInsecs=0 starttime=0 stoptime=0 isEventTrigger=false", messageSeqId);
			LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", err.what());
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "unknown exception when Table301 setTableDataAndWrite messageSeqId=%d", messageSeqId);
		}
	}

	FUNCTION_EXIT;
}

} // namespace TA_IRS_App

