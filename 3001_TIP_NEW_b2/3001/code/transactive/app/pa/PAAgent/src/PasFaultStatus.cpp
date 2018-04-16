/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasFaultStatus.cpp $
  * @author:  Ripple
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2016/12/27 17:16:50 $
  * Last modified by:  $Author: huang.wenbo $
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
#include "app/pa/PAAgent/src/PasFaultStatus.h"

namespace TA_IRS_App
{

const int PAS_OCC_ZONE_COUNT_PER_STN = TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_ZONE_ID - TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID + 1;

PasFaultStatus::PasFaultStatus() : IPasPollingObserver(), IPasDataProcessor(), ITransactionWorkerUser(),
m_mapStationZoneIdToZoneState(),
m_mapStationIdToSystemFault(),
m_usFaultZoneCounter( 0u ),
m_usSystemFaultCode( 0u ),
m_bForceUpdateFaultZones( true ),
m_bForceUpdateSysFault( true ),
m_bFaultZonesDpInitialized( false )
{
    initZoneState();
    initSystemFault();

    PasTransaction* pNewReading = NULL;
    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        // All DVA message library in OCC
        pNewReading = new PasReading( true, *this, PAS_ADDRESS_OCC_FAULT_ZONE_START, PAS_ADDRESS_OCC_FAULT_ZONE_END );
    }
    else
    {
        // DVA message library in current station
        pNewReading = new PasReading( false, *this, PAS_ADDRESS_STN_FAULT_ZONE_START, PAS_ADDRESS_STN_FAULT_ZONE_END );
    }

    PasTransactionManager::getInstance().bindTransactionWorker( *this, pNewReading, true );
}

PasFaultStatus::~PasFaultStatus()
{
    PasTransactionManager::getInstance().unbindTransactionWorker( *this );
}

void PasFaultStatus::initZoneState()
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

void PasFaultStatus::initSystemFault()
{
    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        // For each station, inert a system fault value
        for ( unsigned short usStationId = TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID;
            usStationId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_STN_ID; ++usStationId )
        {
            m_mapStationIdToSystemFault.insert( MapStationIdToSystemFault::value_type( usStationId, 0u ));
        }
    }
}

void PasFaultStatus::processPollingUpdate(
    const TA_Base_Bus::DataBlock<unsigned short>& dbData, const unsigned long ulPolledTimeStamp )
{
    if ( dbData.isInRange( PAS_POLLING_FAULT_ZONE_COUNTER ))
    {
        unsigned short usTempCounter = dbData[PAS_POLLING_FAULT_ZONE_COUNTER];

        if (( usTempCounter != m_usFaultZoneCounter ) || m_bForceUpdateFaultZones )
        {
            m_bForceUpdateFaultZones = false;
            // Refresh fault zones state
            readFaultZoneData();
            m_usFaultZoneCounter = usTempCounter;
        }
    }

    if ( dbData.isInRange( PAS_POLLING_SYSTEM_FAULT ))
    {
        unsigned short usTempSysCode = dbData[PAS_POLLING_SYSTEM_FAULT];

        if (( usTempSysCode != m_usSystemFaultCode ) || m_bForceUpdateSysFault )
        {
            m_bForceUpdateSysFault = false;
            // Refresh system fault status
            refreshSystemFaultDatapoint( usTempSysCode );
            m_usSystemFaultCode = usTempSysCode;
        }
    }

    if ( dbData.isInRange( PAS_POLLING_SYSTEM_FAULT_START ) && dbData.isInRange( PAS_POLLING_SYSTEM_FAULT_END ))
    {
        refreshSystemFaults( dbData );
    }
}

void PasFaultStatus::readFaultZoneData()
{
    PasTransactionManager::getInstance().signalTransaction( *this );
}

// Called in one thread, otherwise need to lock m_mapStationZoneIdToZoneState
void PasFaultStatus::processData( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        if (( PAS_ADDRESS_OCC_FAULT_ZONE_START != dbData.start() ) || 
            ( PAS_ADDRESS_OCC_FAULT_ZONE_END != dbData.end() ))
        {
            // Data is not valid, return.
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "DataBlock[%d, %d] doesn't match range [%d, %d], return", dbData.start(), dbData.end(), 
                PAS_ADDRESS_OCC_FAULT_ZONE_START, PAS_ADDRESS_OCC_FAULT_ZONE_END );
            return;
        }
    }
    else
    {
        if (( PAS_ADDRESS_STN_FAULT_ZONE_START != dbData.start() ) ||
            ( PAS_ADDRESS_STN_FAULT_ZONE_END != dbData.end() ))
        {
            // Data is not valid, return.
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "DataBlock[%d, %d] doesn't match range [%d, %d], return", dbData.start(), dbData.end(), 
                PAS_ADDRESS_STN_FAULT_ZONE_START, PAS_ADDRESS_STN_FAULT_ZONE_END );
            return;
        }
    }

    refreshFaultZones( dbData );
}

void PasFaultStatus::refreshFaultZones( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
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
            int nCurStationAddr = PAS_ADDRESS_OCC_FAULT_ZONE_START + 
                static_cast<int>( usStationId - TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID ) * PAS_OCC_ZONE_COUNT_PER_STN;
            int nZoneOffsetInModbus = 0;
            // For each zone
            for ( unsigned int unZoneId = static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID); 
                unZoneId <= static_cast<unsigned int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_ZONE_ID); ++unZoneId )
            {
                nZoneOffsetInModbus = unZoneId - static_cast<int>(TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID);
                usNewState = dbData[nCurStationAddr + nZoneOffsetInModbus];
                ++usGlobalZoneId;
                if (( usNewState != itFoundStn->second[unZoneId] ) || !m_bFaultZonesDpInitialized )
                {
                    itFoundStn->second[unZoneId] = usNewState;
                    refreshFaultZoneDatapoint( usGlobalZoneId, usNewState );
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
                usNewState = dbData[PAS_ADDRESS_STN_FAULT_ZONE_START + nZoneOffsetInModbus];
                if (( usNewState != itFoundStn->second[unZoneId] ) || !m_bFaultZonesDpInitialized )
                {
                    itFoundStn->second[unZoneId] = usNewState;
                    refreshFaultZoneDatapoint( unZoneId, usNewState );
                }
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station is not initialized" );
        }
    }

    m_bFaultZonesDpInitialized = true;
}

void PasFaultStatus::refreshFaultZoneDatapoint( const unsigned int unZoneId, const unsigned short usNewState )
{
    TA_Base_Bus::DataPoint* pDataPoint = TA_IRS_Bus::CachedMaps::getInstance()->getStatusDataPointFromZoneId( unZoneId );
    if ( NULL != pDataPoint )
    {
        // PA Zone DataPoint exists, so we can update it
        TA_Base_Bus::DpValue dvTemp = pDataPoint->getCurrentValue();

        const int nNormal = 0;
        const int nFault = 1;
        const int nUnknown = 2;
        // 0: Normal ; 1: Fault; 2 : Unknown
        int nValue = 0;
        if ( CachedConfig::getInstance()->getIsMaster() )
        {
            nValue = (( usNewState & PAS_OCC_FAULT_ZONE ) == PAS_OCC_FAULT_ZONE ) ? nFault : nNormal;
            nValue = (( usNewState & PAS_OCC_UNKNOWN_ZONE ) == PAS_OCC_UNKNOWN_ZONE ) ? nUnknown : nValue;
            dvTemp.setEnum( static_cast<unsigned long>(nValue) );
        }
        else
        {
            nValue = (( usNewState & PAS_STN_FAULT_ZONE ) == PAS_STN_FAULT_ZONE ) ? nFault : nNormal;
            dvTemp.setBoolean( static_cast<bool>(nValue) );
        }

        // set up DataPointState with the default timestamp and quality
        TA_Base_Bus::DataPointState newState( dvTemp, time( NULL ), pDataPoint->getFieldValueQualityStatus() );
        pDataPoint->updateFieldState( newState );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s to %d for zone %d", 
            pDataPoint->getDataPointName().c_str(), nValue, unZoneId );
    }
    else
    {
        // DataPoint may not exist for all capacity range
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "NULL pointer when get status DataPoint from CachedMap with id %d", unZoneId );
    }
}

void PasFaultStatus::refreshSystemFaults( const TA_Base_Bus::DataBlock<unsigned short>& dbData )
{
    unsigned short usFaultCode = 0;

    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        // For each station
        for ( unsigned short usStationId = TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID;
            usStationId <= TA_Base_Bus::IPAAgentCorbaDef::PA_MAX_STN_ID; ++usStationId )
        {
            MapStationIdToSystemFault::iterator itFoundStn = m_mapStationIdToSystemFault.find( usStationId );
            if ( m_mapStationIdToSystemFault.end() == itFoundStn )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station %d is not initialized", usStationId );
                continue;
            }

            // Calculate start address of current station
            int nCurStationAddr = PAS_POLLING_SYSTEM_FAULT_START + static_cast<int>(usStationId - TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID);

            usFaultCode = dbData[nCurStationAddr];
            if (( usFaultCode != itFoundStn->second ) || !m_bFaultZonesDpInitialized )
            {
                bool bMajorChanged = ((usFaultCode & PAS_OCC_MAJOR_ALARM) != (itFoundStn->second & PAS_OCC_MAJOR_ALARM));
                bool bMinorChanged = ((usFaultCode & PAS_OCC_MINOR_ALARM) != (itFoundStn->second & PAS_OCC_MINOR_ALARM));

                itFoundStn->second = usFaultCode;

                if ( bMajorChanged )
                {
                    refreshMajorAlarmDatapoint( usStationId, usFaultCode );
                }

                if ( bMinorChanged )
                {
                    refreshMinorAlarmDatapoint( usStationId, usFaultCode );
                }
            }
        }
    }
}

void PasFaultStatus::refreshSystemFaultDatapoint( const unsigned short usFaultCode )
{
    bool bMinorAlarm = false;
    bool bMajorAlarm = false;

    if ( CachedConfig::getInstance()->getIsMaster() )
    {
        bMinorAlarm = (( usFaultCode & PAS_OCC_MINOR_ALARM ) == PAS_OCC_MINOR_ALARM );
        bMajorAlarm = (( usFaultCode & PAS_OCC_MAJOR_ALARM ) == PAS_OCC_MAJOR_ALARM );
    }
    else
    {
        bMinorAlarm = (( usFaultCode & PAS_STN_MINOR_ALARM ) == PAS_STN_MINOR_ALARM );
        bMajorAlarm = (( usFaultCode & PAS_STN_MAJOR_ALARM ) == PAS_STN_MAJOR_ALARM );
    }

    TA_Base_Bus::DataPoint* pDataPoint = NULL;

    pDataPoint = CachedConfig::getInstance()->getMinorSummaryAlarmDP();
    if ( NULL != pDataPoint )
    {
        // PA Minor DataPoint exists, so we can update it
        TA_Base_Bus::DpValue dvTemp = pDataPoint->getCurrentValue();
        dvTemp.setBoolean( bMinorAlarm );

        // set up DataPointState with the default timestamp and quality
        TA_Base_Bus::DataPointState newState( dvTemp, time( NULL ), pDataPoint->getFieldValueQualityStatus() );
        pDataPoint->updateFieldState( newState );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s to %d for fault code %d", 
            pDataPoint->getDataPointName().c_str(), bMinorAlarm, usFaultCode );
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer when get Minor DataPoint from CachedConfig" );
    }

    pDataPoint = CachedConfig::getInstance()->getMajorSummaryAlarmDP();
    if ( NULL != pDataPoint )
    {
        // PA Major DataPoint exists, so we can update it
        TA_Base_Bus::DpValue dvTemp = pDataPoint->getCurrentValue();
        dvTemp.setBoolean( bMajorAlarm );

        // set up DataPointState with the default timestamp and quality
        TA_Base_Bus::DataPointState newState( dvTemp, time( NULL ), pDataPoint->getFieldValueQualityStatus() );
        pDataPoint->updateFieldState( newState );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s to %d for fault code %d", 
            pDataPoint->getDataPointName().c_str(), bMajorAlarm, usFaultCode );
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer when get Major DataPoint from CachedConfig" );
    }
}


void PasFaultStatus::refreshMajorAlarmDatapoint( const unsigned short usStationId, const unsigned short usFaultCode )
{
    const int nNormal = 0;
    const int nAlarm = 1;

    // 0: Normal ; 1: Fault;
    int nValue = 0;

    TA_Base_Bus::DataPoint* pDataPoint = NULL;

    pDataPoint = TA_IRS_Bus::CachedMaps::getInstance()->getMajorAlarmDataPointFromStationId( usStationId );
    if ( NULL != pDataPoint )
    {
        // PA Minor DataPoint exists, so we can update it
        TA_Base_Bus::DpValue dvTemp = pDataPoint->getCurrentValue();
        nValue = ((usFaultCode & PAS_OCC_MAJOR_ALARM) == PAS_OCC_MAJOR_ALARM) ? nAlarm : nNormal;
        dvTemp.setEnum( static_cast<unsigned long>(nValue) );

        // set up DataPointState with the default timestamp and quality
        TA_Base_Bus::DataPointState newState( dvTemp, time( NULL ), pDataPoint->getFieldValueQualityStatus() );
        pDataPoint->updateFieldState( newState );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s to %d for fault code %d",
            pDataPoint->getDataPointName().c_str(), nValue, usFaultCode );
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer when get Minor DataPoint from CachedMaps" );
    }
}

void PasFaultStatus::refreshMinorAlarmDatapoint( const unsigned short usStationId, const unsigned short usFaultCode )
{
    const int nNormal = 0;
    const int nAlarm = 1;

    // 0: Normal ; 1: Fault;
    int nValue = 0;

    TA_Base_Bus::DataPoint* pDataPoint = NULL;

    pDataPoint = TA_IRS_Bus::CachedMaps::getInstance()->getMinorAlarmDataPointFromStationId( usStationId );
    if ( NULL != pDataPoint )
    {
        // PA Major DataPoint exists, so we can update it
        TA_Base_Bus::DpValue dvTemp = pDataPoint->getCurrentValue();
        nValue = ((usFaultCode & PAS_OCC_MINOR_ALARM) == PAS_OCC_MINOR_ALARM) ? nAlarm : nNormal;
        dvTemp.setEnum( static_cast<unsigned long>(nValue) );

        // set up DataPointState with the default timestamp and quality
        TA_Base_Bus::DataPointState newState( dvTemp, time( NULL ), pDataPoint->getFieldValueQualityStatus() );
        pDataPoint->updateFieldState( newState );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s to %d for fault code %d",
            pDataPoint->getDataPointName().c_str(), nValue, usFaultCode );
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer when get Major DataPoint from CachedMaps" );
    }
}

void PasFaultStatus::getDataFailed( int nReason )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Get data from hardware failed, error code %d", nReason );
}

void PasFaultStatus::enable( const bool bEnable )
{
    // If disable, should reset the force update flag
    if ( !bEnable )
    {
        m_bForceUpdateFaultZones = true;
        m_bForceUpdateSysFault = true;
        m_bFaultZonesDpInitialized = false;
    }
}

}
