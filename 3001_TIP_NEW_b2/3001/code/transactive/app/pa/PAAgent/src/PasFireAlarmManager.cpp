/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasFireAlarmManager.cpp $
  * @author:  HuangQi
  * @version: $Revision: #9 $
  *
  * Last modification: $DateTime: 2014/05/15 11:04:53 $
  * Last modified by:  $Author: huangjian $
  *
  */

#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/AcknowledgeFireAlarm.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/PasFireAlarmManager.h"

namespace TA_IRS_App
{

	PasFireAlarmManager::PasFireAlarmManager() : 
    m_mapPasFireAlarm(),
    m_mapStationToFireAlarm(),
    m_mapFireAlarmAcked(),
    m_mapFireActiveKeyToStation(),
    m_bForceUpdatePasFireAlarm( true ),
    m_bEnabled( false )
	{
	    FUNCTION_ENTRY("Constructor");

        initFireAlarmMaps();
	    
	    FUNCTION_EXIT;
    }


    // ExceptionChecked
    PasFireAlarmManager::~PasFireAlarmManager()
    {
        FUNCTION_ENTRY("Destructor");

        {
            ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxFireAlarmProxyMapLock );
            for ( EntityKeyToProxyMapIt itLoop = m_mapPasFireAlarm.begin(); m_mapPasFireAlarm.end() != itLoop; ++itLoop )
            {
                delete itLoop->second;
                itLoop->second = NULL;
            }
            m_mapPasFireAlarm.clear();
        }

        if ( getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
        {
            terminateAndWait();
        }

        FUNCTION_EXIT;
    }

    void PasFireAlarmManager::initFireAlarmMaps()
    {
        StationToFireDpsMap mapFireAlarms = CachedConfig::getInstance()->getFireCountdownActiveDPs();
        if ( 0 == mapFireAlarms.size() )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Cannot find FireCountdownActive DataPoint in order to register for acknowledgment notifications.");     
        }

        for ( StationToFireDpsMapIt itLoop = mapFireAlarms.begin(); mapFireAlarms.end() != itLoop; ++itLoop )
        {
            if ( NULL == itLoop->second )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid NULL pointer in Fire count down active DataPoints" );
                continue;
            }

            try
            {
                TA_Base_Bus::DataPointProxySmartPtr* ptrTemp = new TA_Base_Bus::DataPointProxySmartPtr();
                unsigned long ulEntityKey = itLoop->second->getEntityKey();
                unsigned short usStationId = itLoop->first;

                TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy( 
                    ulEntityKey, *this, *ptrTemp );

                {
                    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxFireAlarmProxyMapLock );
                    m_mapPasFireAlarm.insert( EntityKeyToProxyMap::value_type(ulEntityKey, ptrTemp) );
                }
                m_mapFireAlarmAcked.insert( EntityKeyToFireAckedMap::value_type(ulEntityKey, false) );
                m_mapFireActiveKeyToStation.insert( EntityKeyToStationIdMap::value_type(ulEntityKey, usStationId) );
                m_mapStationToFireAlarm.insert( StationToFireAlarmMap::value_type(usStationId, 0u) );
            }
            catch ( TA_Base_Core::ScadaProxyException& ) 
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException", 
                    "Caught a ScadaProxyException when calling createDataPointProxy.");
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", 
                    "Caught an Unknown Exception when calling createDataPointProxy.");
            }
        }
    }


	void PasFireAlarmManager::enable( bool bEnable )
	{
	    m_bEnabled = bEnable;

        // If disable, should reset the force update flag
        if ( !bEnable )
        {
            m_bForceUpdatePasFireAlarm = true;
        }
	}

    void PasFireAlarmManager::processEvent( TA_Base_Bus::DataPointWriteRequest* newEvent )
    {
        if ( NULL == newEvent )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "DataPointWriteRequest is NULL pointer" );
            return;
        }

        TA_Base_Bus::DataPoint* dp = newEvent->getDataPoint();

        if ( NULL == dp )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "DataPointWriteRequest return NULL DataPoint" );
            return;
        }

        // We are only expecting a value of false from the Fire Countdown DIO DataPoint.

        try
        {
            std::string strCachedDpName = CachedConfig::getInstance()->getFireCountDownOutputToken();
            std::string strEventDpName = dp->getDataPointName();

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DataPoint %s write request found.", strEventDpName.c_str());

            if ( std::string::npos != strEventDpName.find( strCachedDpName ))
            {
                if (newEvent->getValue().getBoolean())
                {
                    unsigned short usStationId = CachedConfig::getInstance()->getStationIdFromFireAbortName( strEventDpName );

                    acknowledgeFireAlarm( usStationId );

                    PasHelpers::getInstance()->submitTerminateFireCountdownAuditMessage( usStationId, dp->getLastOperatorSession() );
                }
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                    "Cached DpName [%s] is not the same as event DpName [%s]", strCachedDpName.c_str(), strEventDpName.c_str() );
            }
        }
        catch ( ... )
        {
            // do nothing
        }
    }

	void PasFireAlarmManager::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
	    if (!m_bEnabled)
		{
	        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode" );
			return;
	    }

	    if ( updateType != TA_Base_Bus::ValueStateUpdate &&
             updateType != TA_Base_Bus::AlarmSummaryUpdate &&
             updateType != TA_Base_Bus::AckSummaryUpdate )
	    {
	        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
	            "Received a proxy update, but it is not value update, ignored.");
	        return;
	    }

        bool isAcked = false;

        {
            ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxFireAlarmProxyMapLock );
            EntityKeyToProxyMapIt itFound = m_mapPasFireAlarm.find( entityKey );
            if (( m_mapPasFireAlarm.end() == itFound ) || ( NULL == itFound->second ))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Received a proxy update, but it is not Fire Countdown proxy, ignored.");
                return;
            }

            TA_Base_Bus::DataPointProxySmartPtr& refProxy = *(itFound->second);

	        try
	        {
	            isAcked = (( refProxy->getAlarmAckState() == TA_Base_Bus::ScadaAlarmAck ) ||
                    ( refProxy->getAlarmAckState() == TA_Base_Bus::ScadaAlarmNotPresent ));
	        }
	        catch (TA_Base_Core::ScadaProxyException&) 
	        {
	            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException", 
	                "Caught a ScadaProxyException when calling getAlarmAckState.");
	        }
	        catch(...)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", 
	                "Caught an Unknown Exception when calling getAlarmAckState.");
	        }
        }

        EntityKeyToFireAckedMapIt itAckFound = m_mapFireAlarmAcked.find( entityKey );
        EntityKeyToStationIdMapIt itStnIdFound = m_mapFireActiveKeyToStation.find( entityKey );
        if (( m_mapFireAlarmAcked.end() == itAckFound ) || ( m_mapFireActiveKeyToStation.end() == itStnIdFound ))
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to find entity %d", entityKey );
            return;
        }

	    if ( itAckFound->second == isAcked )
	    {
	        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
	            "Received a DataPoint value update, but alarm ack state is not changed, ignored.");        
	        return;
	    }

	    itAckFound->second = isAcked;

	    if ( !isAcked )
	    {
	        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Ignoring alarm which is not acknowledged");        
	        return;    
	    }

	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
	        "Received FireCountdownActive alarm acknowledgment for EntityKey %lu", entityKey);

        const unsigned short usStationId = itStnIdFound->second;

        StationToFireAlarmMapIt itFireState = m_mapStationToFireAlarm.find( usStationId );
        if ( m_mapStationToFireAlarm.end() == itFireState )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Could not find fire state for Station %d", usStationId );
            return;
        }

		if ( !isOnFired( itFireState->second ))
		{
	        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "PAS is not on Fire Alarm state, ignored.");        
			return;
		}

        acknowledgeFireAlarm( usStationId );
	}

    void PasFireAlarmManager::processPollingUpdate( const TA_Base_Bus::DataBlock<unsigned short>& dbData, const unsigned long ulPolledTimeStamp )
    {
        if ( !m_bEnabled )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode" );
            return;
        }

        if ( CachedConfig::getInstance()->getIsMaster() )
        {
            if ( !dbData.isInRange( PAS_POLLING_OCC_FIREALARM_START ) || 
                 !dbData.isInRange( PAS_POLLING_OCC_FIREALARM_END ))
            {
                // Polling data is not cared, return.
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "DataBlock[%d, %d] doesn't contain address [%d, %d], return", dbData.start(), dbData.end(), 
                    PAS_POLLING_OCC_FIREALARM_START, PAS_POLLING_OCC_FIREALARM_END );
                return;
            }

            if ( m_bForceUpdatePasFireAlarm )
            {
                m_bForceUpdatePasFireAlarm = false;

                for ( int nStationOffset = 0; nStationOffset < dbData.length(); ++nStationOffset )
                {
                    const unsigned short usTempFireAlarm = dbData[PAS_POLLING_OCC_FIREALARM_START + nStationOffset];
                    const unsigned short usCurStation = TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID + static_cast<unsigned short>(nStationOffset);
                    updatePasFireAlarm( usTempFireAlarm, usCurStation );
                    m_mapStationToFireAlarm[usCurStation] = usTempFireAlarm;
                }
            }
            else
            {
                for ( int nStationOffset = 0; nStationOffset < dbData.length(); ++nStationOffset )
                {
                    const unsigned short usTempFireAlarm = dbData[PAS_POLLING_OCC_FIREALARM_START + nStationOffset];
                    const unsigned short usCurStation = TA_Base_Bus::IPAAgentCorbaDef::PA_MIN_STN_ID + static_cast<unsigned short>(nStationOffset);
                    if (( usTempFireAlarm != m_mapStationToFireAlarm[usCurStation] ))
                    {
                        updatePasFireAlarm( usTempFireAlarm, usCurStation );
                        m_mapStationToFireAlarm[usCurStation] = usTempFireAlarm;
                    }
                }
            }
        }
        else
        {
            if ( !dbData.isInRange( PAS_POLLING_STN_FIREALARM ))
            {
                // Polling data is not cared, return.
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "DataBlock[%d, %d] doesn't contain address %d, return", dbData.start(), dbData.end(), PAS_POLLING_STN_FIREALARM );
                return;
            }

            const unsigned short usTempFireAlarm = dbData[PAS_POLLING_STN_FIREALARM];

            if ( m_bForceUpdatePasFireAlarm )
            {
                m_bForceUpdatePasFireAlarm = false;

                for ( StationToFireAlarmMapIt itLoop = m_mapStationToFireAlarm.begin(); m_mapStationToFireAlarm.end() != itLoop; ++itLoop )
                {
                    updatePasFireAlarm( usTempFireAlarm, itLoop->first );
                    itLoop->second = usTempFireAlarm;
                }
            }
            else
            {
                for ( StationToFireAlarmMapIt itLoop = m_mapStationToFireAlarm.begin(); m_mapStationToFireAlarm.end() != itLoop; ++itLoop )
                {
                    if ( usTempFireAlarm != itLoop->second )
                    {
                        updatePasFireAlarm( usTempFireAlarm, itLoop->first );
                        itLoop->second = usTempFireAlarm;
                    }
                }
            }
        }
    }

    void PasFireAlarmManager::updatePasFireAlarm( const unsigned short usAlarm, const unsigned short usStationId )
    {
        TA_Base_Bus::DataPoint* pFireAlarm = CachedConfig::getInstance()->getFireCountdownActiveDP( usStationId );
        if ( NULL == pFireAlarm )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No Fire Alarm DataPoint when updatePasFireAlarm" );
            return;
        }

        TA_Base_Bus::DpValue dvTemp = pFireAlarm->getCurrentValue();
        bool bOnFire = isOnFired( usAlarm );
        dvTemp.setBoolean( bOnFire );

        // set up DataPointState with the default timestamp and quality
        TA_Base_Bus::DataPointState dpsNewState( dvTemp, time( NULL ), pFireAlarm->getFieldValueQualityStatus() );
        pFireAlarm->updateFieldState( dpsNewState );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[DataPoint] Update DataPoint %s to %d for alarm code %d", 
            pFireAlarm->getDataPointName().c_str(), bOnFire, usAlarm );
    }

    void PasFireAlarmManager::acknowledgeFireAlarm( const unsigned short usStationId )
    {
        AcknowledgeFireAlarm* pAcknowledge = new AcknowledgeFireAlarm( CachedConfig::getInstance()->getIsMaster() );

        pAcknowledge->setStationId( usStationId );

        PasTransactionManager::getInstance().addTransaction( pAcknowledge );

        // [TBD] What to do next if failed to send the command to PAS
    }

    bool PasFireAlarmManager::isOnFired( const unsigned short usFireState )
    {
        bool bOnFired = false;

        if ( CachedConfig::getInstance()->getIsMaster() )
        {
            bOnFired = (( usFireState & MASK_OCC_FIRE_ALARM ) == MASK_OCC_FIRE_ALARM ) || 
                (( usFireState & MASK_OCC_FIRE_ALARM_WITH_DVA ) == MASK_OCC_FIRE_ALARM_WITH_DVA );
        }
        else
        {
            bOnFired = (( usFireState & MASK_STN_FIRE_ALARM ) == MASK_STN_FIRE_ALARM ) || 
                (( usFireState & MASK_STN_FIRE_ALARM_WITH_DVA ) == MASK_STN_FIRE_ALARM_WITH_DVA );
        }

        return bOnFired;
    }
};
