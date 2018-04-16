/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasZonesState.cpp $
  * @author:  Ripple
  * @version: $Revision: #12 $
  *
  * Last modification: $DateTime: 2014/04/10 18:43:22 $
  * Last modified by:  $Author: qi.huang $
  *
  */
#include "ace/OS.h"

#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasReading.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/PasZonesState.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

namespace TA_IRS_App
{

const int TASK_ZONE_INDEX = 1;
const int TASK_PAGING_CONSOLE_INDEX = 2;
const int TASK_TRANSACTION_INDEX = 3;//added by lixiaoxia
const int PAS_OCC_ZONE_COUNT_PER_STN = TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_ZONE_ID - TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID + 1;

PasZonesState* PasZonesState::s_pInstance = NULL;

PasZonesState::PasZonesState() : IPasPollingObserver(), IPasDataProcessor(), ITransactionWorkerUser(),
m_mapStationZoneIdToZoneState(),
m_mapStationZoneIdToTransactionState(),
m_mapZoneKeyToTransactionState(),
m_mapPagingConsoleStates(),
m_ulZoneRefreshTime( 0u ),
m_ulPagingConsoleRefreshTime( 0u ),
m_usZoneStateCounter( 0u ),
m_bForceUpdateZonesState( true ),
m_bZonesStateDpInitialized( false )
{
    initZoneState();
	initTransactionState();
    initPagingConsoleState();

    PasTransaction* pZoneReader = NULL;
    PasTransaction* pPagingConsole = NULL;
	PasTransaction* pTransactionReader = NULL;

    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        // All DVA message library in OCC
        pZoneReader = new PasReading( true, *this, 
            PAS_ADDRESS_OCC_ZONE_STATE_START, PAS_ADDRESS_OCC_ZONE_STATE_END );

        pPagingConsole = new PasReading( true, *this, 
            PAS_POLLING_OCC_CONSOLE_START, PAS_POLLING_OCC_CONSOLE_END );

		pTransactionReader = new PasReading( true, *this,
			PAS_ADDRESS_OCC_ZONE_TRANSID_STATE_START, PAS_ADDRESS_OCC_ZONE_TRANSID_STATE_END );
    }
    else
    {
        // DVA message library in current station
        pZoneReader = new PasReading( false, *this, 
            PAS_ADDRESS_STN_ZONE_STATE_START, PAS_ADDRESS_STN_ZONE_STATE_END );

        pPagingConsole = new PasReading( true, *this, 
            PAS_POLLING_STN_CONSOLE_START, PAS_POLLING_STN_CONSOLE_END );

		pTransactionReader = new PasReading( true, *this,
			PAS_ADDRESS_STN_ZONE_TRANSID_STATE_START, PAS_ADDRESS_STN_ZONE_TRANSID_STATE_END );
    }

    PasTransactionManager::getInstance().bindTransactionWorker( *this, pZoneReader, TASK_ZONE_INDEX, true );
    PasTransactionManager::getInstance().bindTransactionWorker( *this, pPagingConsole, TASK_PAGING_CONSOLE_INDEX, true );
	PasTransactionManager::getInstance().bindTransactionWorker( *this, pTransactionReader, TASK_TRANSACTION_INDEX, true);
}

PasZonesState::~PasZonesState()
{
    PasTransactionManager::getInstance().unbindTransactionWorker( *this );
}

PasZonesState& PasZonesState::getInstance()
{
    if ( NULL == s_pInstance )
    {
        TA_THROW( TA_Base_Core::TransactiveException("Instance of PasZonesState is not created") );
    }

    return *s_pInstance;
}

void PasZonesState::createInstance()
{
    if ( NULL != s_pInstance )
    {
        return;
    }

    s_pInstance = new PasZonesState();
}

void PasZonesState::destoryInstance()
{
    if ( NULL == s_pInstance )
    {
        return;
    }

    delete s_pInstance;
    s_pInstance = NULL;
}

void PasZonesState::initZoneState()
{
    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        // For each station, inert a empty map
        for ( unsigned short usStationId = TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID; 
            usStationId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_STN_ID; ++usStationId )
        {
            MapZoneIdToState mapEmptyStationState;
            std::pair<MapStationToZoneState::iterator, bool> pairNewItem = 
                m_mapStationZoneIdToZoneState.insert( MapStationToZoneState::value_type(usStationId, mapEmptyStationState) );

            // Check map insertion result
            if (( m_mapStationZoneIdToZoneState.end() == pairNewItem.first ) || !pairNewItem.second )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "std::map insert failure for MapStationToZoneState" );
                continue;
            }
            // For each zone, set default zone state
            for ( unsigned int nZoneId = static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID); 
                nZoneId <= static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_ZONE_ID); ++nZoneId )
            {
                pairNewItem.first->second.insert( MapZoneIdToState::value_type( nZoneId, PAS_ZONE_STATE_FREE ) );
            }
        }
    }
    else
    {
        MapZoneIdToState mapEmptyStationState;
        std::pair<MapStationToZoneState::iterator, bool> pairNewItem = 
            m_mapStationZoneIdToZoneState.insert( MapStationToZoneState::value_type(CUR_STATION_ID, mapEmptyStationState) );

        // Check map insertion result
        if (( m_mapStationZoneIdToZoneState.end() != pairNewItem.first ) && pairNewItem.second )
        {
            // For each zone, set default zone state
            for ( unsigned int nZoneId = static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_ZONE_ID); 
                nZoneId <= static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MAX_ZONE_ID); ++nZoneId )
            {
                pairNewItem.first->second.insert( MapZoneIdToState::value_type( nZoneId, PAS_ZONE_STATE_FREE ) );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "std::map insert failure for MapStationToZoneState" );
        }
    }
}

//added by lixiaoxia
void PasZonesState::initTransactionState()
{
	if ( CachedConfig::getInstance()->getIsMaster() )
	{
		// For each station, inert a empty map
		for ( unsigned short usStationId = TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID; 
			usStationId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_STN_ID; ++usStationId )
		{
			MapZoneIdToState mapEmptyStationState;
			std::pair<MapStationToTransactionState::iterator, bool> pairNewItem = 
				m_mapStationZoneIdToTransactionState.insert( MapStationToTransactionState::value_type(usStationId, mapEmptyStationState) );

			// Check map insertion result
			if (( m_mapStationZoneIdToTransactionState.end() == pairNewItem.first ) || !pairNewItem.second )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "std::map insert failure for MapStationToTransactionState" );
				continue;
			}
			// For each zone, set default zone state
			for ( unsigned int nZoneId = static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID); 
				nZoneId <= static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_ZONE_ID); ++nZoneId )
			{
				pairNewItem.first->second.insert( MapZoneIdToState::value_type( nZoneId, PAS_FREE_TRANSACTION ) );
			}
		}

		TA_IRS_Bus::PaZoneRecords lstAllZones = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneCachedMap().getAllRecords();

		for ( TA_IRS_Bus::PaZoneRecordsIt itZoneLoop = lstAllZones.begin(); lstAllZones.end() != itZoneLoop; ++itZoneLoop )
		{
			unsigned int unZoneId = static_cast<unsigned int>(itZoneLoop->m_id);
			unsigned int usStationId = 0;
			try
			{
				usStationId = TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey( itZoneLoop->m_locationKey );
			}
			catch ( std::exception& expWhat )
			{
				// Failed to write the changes
				LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
				continue;
			}
			catch (...)
			{
				// Failed to write the changes
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
					"Caught unknown exception when calling CachedMaps::getPasStationIdFromLocationKey" );
				continue;
			}

			if (( static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID) > unZoneId ) || 
				( static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_ZONE_ID) < unZoneId ))
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Zone id %d is not in range", unZoneId );
				continue;
			}

			if (( TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID > usStationId ) || 
				( TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_STN_ID < usStationId ))
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station id %d is not in range", usStationId );
				continue;
			}

			unsigned short* pZoneState = &(m_mapStationZoneIdToTransactionState[usStationId][unZoneId]);
			m_mapZoneKeyToTransactionState.insert( MapZoneKeyToTransactionStatePtr::value_type(itZoneLoop->m_pKey, pZoneState) );
		}
	}
	else
	{
		MapZoneIdToState mapEmptyStationState;
		std::pair<MapStationToTransactionState::iterator, bool> pairNewItem = 
			m_mapStationZoneIdToTransactionState.insert( MapStationToTransactionState::value_type(CUR_STATION_ID, mapEmptyStationState) );

		// Check map insertion result
		if (( m_mapStationZoneIdToTransactionState.end() != pairNewItem.first ) && pairNewItem.second )
		{
			// For each zone, set default zone state
			for ( unsigned int nZoneId = static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_ZONE_ID); 
				nZoneId <= static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MAX_ZONE_ID); ++nZoneId )
			{
				pairNewItem.first->second.insert( MapZoneIdToState::value_type( nZoneId, PAS_FREE_TRANSACTION ) );
			}
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "std::map insert failure for MapStationToTransactionState" );
		}


        TA_IRS_Bus::PaZoneRecords lstAllZones = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneCachedMap().getAllRecords();

        for ( TA_IRS_Bus::PaZoneRecordsIt itZoneLoop = lstAllZones.begin(); lstAllZones.end() != itZoneLoop; ++itZoneLoop )
        {
            unsigned int unZoneId = static_cast<unsigned int>(itZoneLoop->m_id);

            if (( static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_ZONE_ID) > unZoneId ) || 
                ( static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MAX_ZONE_ID) < unZoneId ))
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Zone id %d is not in range", unZoneId );
                continue;
            }

			unsigned short* pZoneState = &(m_mapStationZoneIdToTransactionState[CUR_STATION_ID][unZoneId]);
			m_mapZoneKeyToTransactionState.insert( MapZoneKeyToTransactionStatePtr::value_type(itZoneLoop->m_pKey, pZoneState) );
        }
    }
}

void PasZonesState::initPagingConsoleState()
{
    std::map<unsigned short, std::string> mapPagingConsoles = CachedConfig::getInstance()->getPagingConsoles();

    m_mapPagingConsoleStates.clear();

    for ( std::map<unsigned short, std::string>::iterator itLoop = mapPagingConsoles.begin(); 
        mapPagingConsoles.end() != itLoop; ++itLoop )
    {
        m_mapPagingConsoleStates.insert( MapPagingConsoleState::value_type( itLoop->first, 0u ));
    }
}

void PasZonesState::processPollingUpdate( 
    const TA_Base_Bus::DataBlock<unsigned short>& dbData, const unsigned long ulPolledTimeStamp )
{
    if ( !dbData.isInRange( PAS_POLLING_ZONE_STATE_COUNTER ))
    {
        // Polling data is not cared, return.
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "DataBlock[%d, %d] doesn't contain address %d, return", 
            dbData.start(), dbData.end(), PAS_POLLING_ZONE_STATE_COUNTER );
        return;
    }

    unsigned short usTempCounter = dbData[PAS_POLLING_ZONE_STATE_COUNTER];

    if (( usTempCounter != m_usZoneStateCounter ) || m_bForceUpdateZonesState )
    {
        m_bForceUpdateZonesState = false;
        // Refresh zones state
        readZoneData();
        m_usZoneStateCounter = usTempCounter;
    }
}

void PasZonesState::readZoneData()
{
    PasTransactionManager::getInstance().signalTransaction( *this, TASK_ZONE_INDEX );
	PasTransactionManager::getInstance().signalTransaction( *this, TASK_TRANSACTION_INDEX );//lixiaoxia
}

// Called in one thread, otherwise need to lock m_mapStationZoneIdToZoneState
void PasZonesState::processData( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Process DataBlock[%d, %d] reading", dbData.start(), dbData.end() );

    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        if (( PAS_ADDRESS_OCC_ZONE_STATE_START == dbData.start() ) && 
            ( PAS_ADDRESS_OCC_ZONE_STATE_END == dbData.end() ))
        {
            refreshZoneStates( dbData );
        }
        else if (( PAS_POLLING_OCC_CONSOLE_START == dbData.start() ) && 
            ( PAS_POLLING_OCC_CONSOLE_END == dbData.end() ))
        {
            refreshPagingConsole( dbData );
        }
		//added by lixiaoxia
		else if(( PAS_ADDRESS_OCC_ZONE_TRANSID_STATE_START == dbData.start() ) && 
			( PAS_ADDRESS_OCC_ZONE_TRANSID_STATE_END == dbData.end() ))
		{
			refreshTransationStates(dbData);
		}
        else
        {
            // Data is not valid
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "DataBlock[%d, %d] doesn't match", dbData.start(), dbData.end() );
        }
    }
    else
    {
        if (( PAS_ADDRESS_STN_ZONE_STATE_START == dbData.start() ) && 
            ( PAS_ADDRESS_STN_ZONE_STATE_END == dbData.end() ))
        {
            refreshZoneStates( dbData );
        }
        else if (( PAS_POLLING_STN_CONSOLE_START == dbData.start() ) && 
            ( PAS_POLLING_STN_CONSOLE_END == dbData.end() ))
        {
            refreshPagingConsole( dbData );
        }
		//added by lixiaoxia
		else if(( PAS_ADDRESS_STN_ZONE_TRANSID_STATE_START == dbData.start() ) && 
			( PAS_ADDRESS_STN_ZONE_TRANSID_STATE_END == dbData.end() ))
		{
			refreshTransationStates(dbData);
        }
        else
        {
            // Data is not valid
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "DataBlock[%d, %d] doesn't match", dbData.start(), dbData.end() );
        }
    }
}

void PasZonesState::refreshZoneStates( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    unsigned short usNewState = 0;

    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        unsigned short usGlobalZoneId = 0;
        // For each station
        for ( unsigned short usStationId = TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID; 
            usStationId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_STN_ID; ++usStationId )
        {
            MapStationToZoneState::iterator itFoundStn = m_mapStationZoneIdToZoneState.find( usStationId );
            if ( m_mapStationZoneIdToZoneState.end() == itFoundStn )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station %d is not initialized", usStationId );
                continue;
            }

            // Calculate start address of current station
            int nCurStationAddr = PAS_ADDRESS_OCC_ZONE_STATE_START + 
                static_cast<int>( usStationId - TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID ) * PAS_OCC_ZONE_COUNT_PER_STN;
            int nZoneOffsetInModbus = 0;
            // For each zone
            for ( unsigned int unZoneId = static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID); 
                unZoneId <= static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_ZONE_ID); ++unZoneId )
            {
                nZoneOffsetInModbus = unZoneId - static_cast<int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID);
                usNewState = dbData[nCurStationAddr + nZoneOffsetInModbus];
                ++usGlobalZoneId;
                if (( usNewState != itFoundStn->second[unZoneId] ) || !m_bZonesStateDpInitialized )
                {
                    itFoundStn->second[unZoneId] = usNewState;
                    refreshZoneDatapoint( usGlobalZoneId, usNewState );
                }
            }
        }
    }
    else
    {
        MapStationToZoneState::iterator itFoundStn = m_mapStationZoneIdToZoneState.find( CUR_STATION_ID );

        // Current station should in the map
        if ( m_mapStationZoneIdToZoneState.end() != itFoundStn )
        {
            unsigned int nZoneOffsetInModbus = 0;
            // For each zone
            for ( unsigned int unZoneId = static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_ZONE_ID); 
                unZoneId <= static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MAX_ZONE_ID); ++unZoneId )
            {
                nZoneOffsetInModbus = unZoneId - static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_ZONE_ID);
                usNewState = dbData[PAS_ADDRESS_STN_ZONE_STATE_START + nZoneOffsetInModbus];
                if (( usNewState != itFoundStn->second[unZoneId] ) || !m_bZonesStateDpInitialized )
                {
                    itFoundStn->second[unZoneId] = usNewState;
                    refreshZoneDatapoint( unZoneId, usNewState );
                }
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station is not initialized" );
        }
    }

    m_bZonesStateDpInitialized = true;
}

//added by lixiaoxia
void PasZonesState::refreshTransationStates( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
	unsigned short usNewTransactionState = 0;

	if ( CachedConfig::getInstance()->getIsMaster() )
	{
		unsigned short usGlobalZoneId = 0;
		// For each station
		for ( unsigned short usStationId = TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID; 
			usStationId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_STN_ID; ++usStationId )
		{
			MapStationToTransactionState::iterator itFoundStn = m_mapStationZoneIdToTransactionState.find( usStationId );
			if ( m_mapStationZoneIdToTransactionState.end() == itFoundStn )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station %d is not initialized", usStationId );
				continue;
			}

			// Calculate start address of current station
			int nCurStationAddr = PAS_ADDRESS_OCC_ZONE_TRANSID_STATE_START + 
				static_cast<int>( usStationId - TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID ) * PAS_OCC_ZONE_COUNT_PER_STN;
			int nZoneOffsetInModbus = 0;
			// For each zone
			for ( unsigned int unZoneId = static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID); 
				unZoneId <= static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_ZONE_ID); ++unZoneId )
			{
				nZoneOffsetInModbus = unZoneId - static_cast<int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID);
				usNewTransactionState = dbData[nCurStationAddr + nZoneOffsetInModbus];
				++usGlobalZoneId;
				if (( usNewTransactionState != itFoundStn->second[unZoneId] ) || !m_bZonesStateDpInitialized )
				{
					itFoundStn->second[unZoneId] = usNewTransactionState;
				}
			}
		}
	}
	else
	{
		MapStationToTransactionState::iterator itFoundStn = m_mapStationZoneIdToTransactionState.find( CUR_STATION_ID );

		// Current station should in the map
		if ( m_mapStationZoneIdToTransactionState.end() != itFoundStn )
		{
			unsigned int nZoneOffsetInModbus = 0;
			// For each zone
			for ( unsigned int unZoneId = static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_ZONE_ID); 
				unZoneId <= static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MAX_ZONE_ID); ++unZoneId )
			{
				nZoneOffsetInModbus = unZoneId - static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_ZONE_ID);
				usNewTransactionState = dbData[PAS_ADDRESS_STN_ZONE_TRANSID_STATE_START + nZoneOffsetInModbus];
				if (( usNewTransactionState != itFoundStn->second[unZoneId] ) || !m_bZonesStateDpInitialized )
				{
					itFoundStn->second[unZoneId] = usNewTransactionState;
				}
			}
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station is not initialized" );
		}
	}

	m_ulZoneRefreshTime = ACE_OS::gettimeofday().msec();
}

void PasZonesState::refreshPagingConsole( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    int nZoneOffsetInModbus = 0;

    int nStartAddress = 0;
    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        nStartAddress = PAS_POLLING_OCC_CONSOLE_START;
    }
    else
    {
        nStartAddress = PAS_POLLING_STN_CONSOLE_START;
    }

    // Map order is from small to big
    int nCurAddress = 0;

    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        for ( MapPagingConsoleState::iterator itLoop = m_mapPagingConsoleStates.begin(); 
            m_mapPagingConsoleStates.end() != itLoop; ++itLoop )
        {
            nCurAddress = nStartAddress + nZoneOffsetInModbus;
            if ( !dbData.isInRange( nCurAddress ))
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                    "Current address %d is out of range of DataBlock [%d, %d]", nCurAddress, dbData.start(), dbData.end() );
                continue;
            }

            itLoop->second = dbData[nCurAddress];
            ++nZoneOffsetInModbus;
        }
    }
    else
    {
        MapPagingConsoleState::iterator itPsc = m_mapPagingConsoleStates.find( PAS_PAGING_CONSOLE_PSC );
        if ( itPsc != m_mapPagingConsoleStates.end() )
        {
            if ( dbData.isInRange( nStartAddress ))
            {
                itPsc->second = dbData[nStartAddress];
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                    "Current address %d is out of range of DataBlock [%d, %d]", nStartAddress, dbData.start(), dbData.end() );
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Paging Console Id %d is not found", PAS_PAGING_CONSOLE_PSC );
        }
    }

    // Update the time at last to ensure all states are newest when getting
    m_ulPagingConsoleRefreshTime = ACE_OS::gettimeofday().msec();
}

void PasZonesState::refreshZoneDatapoint( const unsigned int unZoneId, const unsigned short usNewState )
{
    unsigned short usZoneState = usNewState;
    bool bIsLive = false;
    bool bIsMusic = false;
    bool bIsDva = false;

    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        usZoneState &= MASK_OCC_ZONE_BROADCAST_STATE;

        bIsLive = PasHelpers::getInstance()->isLiveBroadcast( usZoneState )  || ( PAS_ZONE_STATE_OCC_LOCAL_LIVE == usZoneState );

        bIsMusic = ( PAS_ZONE_STATE_OCC_CENTRE_MUSIC == usZoneState ) || 
            ( PAS_ZONE_STATE_OCC_LOCAL_MUSIC == usZoneState );

        bIsDva = ((( PAS_ZONE_STATE_OCC_DVA_ID_BASE + TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_DVA_NUM ) <= usZoneState ) && 
            (( PAS_ZONE_STATE_OCC_DVA_ID_BASE + TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_DVA_NUM ) >= usZoneState )) || 
            ( PAS_ZONE_STATE_OCC_LOCAL_DVA == usZoneState );

        // Special DVA broadcast status
        bIsDva |= ( PAS_ZONE_STATE_OCC_FIRE_DVA_BROADCAST == usZoneState );
    }
    else
    {
        usZoneState &= MASK_STN_ZONE_BROADCAST_STATE;

        bIsLive = PasHelpers::getInstance()->isLiveBroadcast( usZoneState ) || ( PAS_ZONE_STATE_STN_CENTRE_LIVE == usZoneState );

        bIsMusic = ( PAS_ZONE_STATE_STN_LOCAL_MUSIC == usZoneState ) || 
            ( PAS_ZONE_STATE_STN_CENTRE_MUSIC == usZoneState );

        bIsDva = ((( PAS_ZONE_STATE_STN_DVA_ID_BASE + TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MIN_DVA_NUM ) <= usZoneState ) && 
            (( PAS_ZONE_STATE_STN_DVA_ID_BASE + TA_Base_Bus::IPAAgentCorbaDef::PA_STN_MAX_DVA_NUM ) >= usZoneState )) || 
            ( PAS_ZONE_STATE_STN_CENTRE_DVA == usZoneState );

        // Special DVA broadcast status
        bIsDva |= ( PAS_ZONE_STATE_STN_FIRE_DVA_BROADCAST == usZoneState );
    }

    TA_Base_Bus::DataPoint* pDataPoint = NULL;
    pDataPoint = TA_IRS_Bus::CachedMaps::getInstance()->getDvaDataPointFromZoneId( unZoneId );
    if ( NULL != pDataPoint )
    {
        // PA Zone DataPoint exists, so we can update it
        TA_Base_Bus::DpValue dvTemp = pDataPoint->getCurrentValue();
        dvTemp.setBoolean( bIsDva );

        // set up DataPointState with the default timestamp and quality
        TA_Base_Bus::DataPointState newState( dvTemp, time( NULL ), pDataPoint->getFieldValueQualityStatus() );
        pDataPoint->updateFieldState( newState );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s to %d for zone %d", 
            pDataPoint->getDataPointName().c_str(), bIsDva, unZoneId );
    }
    else
    {
        // DataPoint may not exist for all capacity range
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "NULL pointer when get dva DataPoint from CachedMap with id %d", unZoneId );
    }

    pDataPoint = TA_IRS_Bus::CachedMaps::getInstance()->getLiveDataPointFromZoneId( unZoneId );
    if ( NULL != pDataPoint )
    {
        // PA Zone DataPoint exists, so we can update it
        TA_Base_Bus::DpValue dvTemp = pDataPoint->getCurrentValue();
        dvTemp.setBoolean( bIsLive );

        // set up DataPointState with the default timestamp and quality
        TA_Base_Bus::DataPointState newState( dvTemp, time( NULL ), pDataPoint->getFieldValueQualityStatus() );
        pDataPoint->updateFieldState( newState );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s to %d for zone %d", 
            pDataPoint->getDataPointName().c_str(), bIsLive, unZoneId );
    }
    else
    {
        // DataPoint may not exist for all capacity range
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "NULL pointer when get live DataPoint from CachedMap with id %d", unZoneId );
    }

    pDataPoint = TA_IRS_Bus::CachedMaps::getInstance()->getMusicDataPointFromZoneId( unZoneId );
    if ( NULL != pDataPoint )
    {
        // PA Zone DataPoint exists, so we can update it
        TA_Base_Bus::DpValue dvTemp = pDataPoint->getCurrentValue();
        dvTemp.setBoolean( bIsMusic );

        // set up DataPointState with the default timestamp and quality
        TA_Base_Bus::DataPointState newState( dvTemp, time( NULL ), pDataPoint->getFieldValueQualityStatus() );
        pDataPoint->updateFieldState( newState );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s to %d for zone %d", 
            pDataPoint->getDataPointName().c_str(), bIsMusic, unZoneId );
    }
    else
    {
        // DataPoint may not exist for all capacity range
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "NULL pointer when get music DataPoint from CachedMap with id %d", unZoneId );
    }
}

void PasZonesState::getDataFailed( int nReason )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Get data from hardware failed, error code %d", nReason );
}

void PasZonesState::getZoneStatesByZoneKey( MapZoneKeyToTransactionState& mapTransactionStates )
{
    for ( MapZoneKeyToTransactionState::iterator itLoop = mapTransactionStates.begin(); mapTransactionStates.end() != itLoop; ++itLoop )
    {
        itLoop->second = *(m_mapZoneKeyToTransactionState[itLoop->first]);
    }
}


unsigned long PasZonesState::getZoneUpdateTime()
{
    return m_ulZoneRefreshTime;
}

bool PasZonesState::isZoneStateFault( const unsigned short usZoneState )
{
    return ( PAS_ZONE_FAULT_STATE == ( usZoneState & MASK_ZONE_FAULT_STATE ));
}

void PasZonesState::enable( const bool bEnable )
{
    // If disable, should reset the force update flag
    if ( !bEnable )
    {
        m_bForceUpdateZonesState = true;
        m_bZonesStateDpInitialized = false;
    }
}

unsigned long PasZonesState::notifyReadPagingConsoleState( const bool bRead )
{
    // Record the update time before communicate with PAS to ensure the refresh time is updated by PAS data read
    unsigned long ulTempRefreshTime = ACE_OS::gettimeofday().msec();
    m_ulPagingConsoleRefreshTime = ulTempRefreshTime;

    PasTransactionManager::getInstance().keepWorkTransaction( *this, TASK_PAGING_CONSOLE_INDEX, bRead );

    return ulTempRefreshTime;
}

bool PasZonesState::isPagingConsoleUpdated( const unsigned long ulSigTime )
{
    return ( m_ulPagingConsoleRefreshTime != ulSigTime );
}

bool PasZonesState::isConsolePlaybacking( const unsigned short usSourceId )
{
    MapPagingConsoleState::iterator itFound = m_mapPagingConsoleStates.find( usSourceId );

    if ( m_mapPagingConsoleStates.end() == itFound )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Source Id %d is not configured in Database table PA_PAGING_CONSOLE", usSourceId );
        return false;
    }

    bool bPlayback = (PAS_CONSOLE_PLAYBACKING == ( itFound->second & PAS_CONSOLE_PLAYBACKING ));

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Current Playback status: %s for paging console %d", bPlayback ? "Active" : "Inactive", usSourceId );

    return bPlayback;
}

bool PasZonesState::isConsoleRecording( const unsigned short usSourceId )
{
    MapPagingConsoleState::iterator itFound = m_mapPagingConsoleStates.find( usSourceId );

    if ( m_mapPagingConsoleStates.end() == itFound )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Source Id %d is not configured in Database table PA_PAGING_CONSOLE", usSourceId );
        return false;
    }

    bool bRecording = (PAS_CONSOLE_RECORDING == ( itFound->second & PAS_CONSOLE_RECORDING ));

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Current ad-hoc recording status: %s for paging console %d", bRecording ? "Active" : "Inactive", usSourceId );

    return bRecording;
}

bool PasZonesState::isConsoleLiving( const unsigned short usSourceId )
{
    MapPagingConsoleState::iterator itFound = m_mapPagingConsoleStates.find( usSourceId );

    if ( m_mapPagingConsoleStates.end() == itFound )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Source Id %d is not configured in Database table PA_PAGING_CONSOLE", usSourceId );
        return false;
    }

    // With chime is 0x0100, without chime is 0x0200, 0x0100 | 0x0200 is 0x0300, then compare with 0 but not PAS_CONSOLE_LIVING
    bool bLiving = (0u != ( itFound->second & PAS_CONSOLE_LIVING_MASK ));

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Current living status: %s for paging console %d", bLiving ? "Live" : "Non-live", usSourceId );

    return bLiving;
}

//added by lixiaoxia
bool PasZonesState::isZoneFree( const unsigned short usTransationState )
{
	return ( PAS_FREE_TRANSACTION == ( usTransationState & MASK_BUSY_TRANSACTION ));
}

bool PasZonesState::areZonesFreeOfTransaction( const std::vector<unsigned long>& vecZones, const unsigned short usTransactionId )
{
	bool bFree = true;

	for ( std::vector<unsigned long>::const_iterator itLoop = vecZones.begin(); vecZones.end() != itLoop; ++itLoop )
	{
		if ( usTransactionId == ( *(m_mapZoneKeyToTransactionState[*itLoop]) & MASK_TRANSACTION_STATE_ID ))
		{
			bFree = false;
			break;
		}
	}

	return bFree;
}

bool PasZonesState::isZoneFreeOfTransaction( const unsigned short usTransactionState,const unsigned short usTransactionId )
{
	return (( usTransactionState & MASK_TRANSACTION_STATE_ID ) != usTransactionId );
}

}
