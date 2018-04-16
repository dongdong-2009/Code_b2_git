/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/MasterDVABroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #11 $
  * Last modification : $DateTime: 2014/02/10 16:54:21 $
  * Last modified by : $Author: qi.huang $
  *
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PasExceptions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/StartDvaBroadcast.h"
#include "app/pa/PAAgent/src/StartDvaPlayback.h"
#include "app/pa/PAAgent/src/CreateScheduleDva.h"
#include "app/pa/PAAgent/src/StopDvaBroadcast.h"
#include "app/pa/PAAgent/src/StopDvaPlayback.h"
#include "app/pa/PAAgent/src/DeleteScheduleDva.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/MasterDVABroadcast.h"



namespace TA_IRS_App
{

MasterDVABroadcast::MasterDVABroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
//AbstractStationBroadcast( broadcastConfigP, sessionId ),
NonTrainDVABroadcast( broadcastConfigP, sessionId )
{
}

MasterDVABroadcast::~MasterDVABroadcast() 
{
}

void MasterDVABroadcast::validateBroadcastConfigSpecifics() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException) 
{
    TA_THREADGUARD( m_lock );

    if ( !m_theSpecifics.isLocalPlayback )
    {
        // Test validity of zone keys in database (Global version)
        for (unsigned int index = 0; index< m_theSpecifics.vecZoneKeys.size(); ++index)
        {
            unsigned long zoneKey = 0;
            unsigned int zoneId = 0;
            unsigned long zoneLocationKey = 0;
            unsigned int stationId = 0;

            try
            {
                zoneKey = m_theSpecifics.vecZoneKeys[index];
                zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(zoneKey);
            }
            catch(TA_Base_Core::CachedMappingNotFoundException&)
            {
                PasHelpers::getInstance()->throwPaErrorException( 
                    TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "A requested zone does not exist in the database" );
            }

            try
            {
                zoneLocationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(zoneKey);
                stationId = TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey(zoneLocationKey);
            }
            catch(TA_Base_Core::CachedMappingNotFoundException&)
            {
                PasHelpers::getInstance()->throwPaErrorException( 
                    TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "An ISCS station location could not be resolved to a PAS location" );
            }
        }
    }
}

int MasterDVABroadcast::writeStartToPas()
{
    if ( m_theSpecifics.bScheduled )
    {
        CreateScheduleDva* pCreateScheduleDva = new CreateScheduleDva( true, this );

        if ( NULL == pCreateScheduleDva )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartStationDvaBroadcast" );
            return TRANSACTION_FAILED;
        }

        pCreateScheduleDva->setPasZoneKeys( m_theSpecifics.vecZoneKeys );
        pCreateScheduleDva->setWithChime( m_theSpecifics.hasChime );
        pCreateScheduleDva->setDvaMessageIds( m_theSpecifics.vecMsgIds );
        pCreateScheduleDva->setDaysInWeek( m_theSpecifics.usDaysInWeek );
        pCreateScheduleDva->setStartTime( m_theSpecifics.usStartTime, m_theSpecifics.usStartSecond );
        pCreateScheduleDva->setEndTime( m_theSpecifics.usEndTime, m_theSpecifics.usEndSecond );
        pCreateScheduleDva->setInterval( m_theSpecifics.usInterval, m_theSpecifics.usIntervalSecond );
        pCreateScheduleDva->setScheduleId( m_theSpecifics.usScheduleId );
		pCreateScheduleDva->setDwellPeriod( m_theSpecifics.dwellInSecs );

        return PasTransactionManager::getInstance().addTransaction( pCreateScheduleDva );
    }
	else if ( !m_theSpecifics.isLocalPlayback ) // normal DVA broadcast
	{
		StartDvaBroadcast* pStartMasterDva = new StartDvaBroadcast( true, this );

		if ( NULL == pStartMasterDva )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartMasterDvaBroadcast" );
			return TRANSACTION_FAILED;
		}

		pStartMasterDva->setPasZoneKeys( m_theSpecifics.vecZoneKeys );
		pStartMasterDva->setWithChime( m_theSpecifics.hasChime );
		pStartMasterDva->setDvaMessageIds( m_theSpecifics.vecMsgIds );
		pStartMasterDva->setTransactionId( m_announceId );//lixiaoxia
		pStartMasterDva->setDwellPeriod(m_theSpecifics.dwellInSecs);

		return PasTransactionManager::getInstance().addTransaction( pStartMasterDva );
	}
	else // DVA Playback
	{
		StartDvaPlayback* pStartMasterDvaPlayback = new StartDvaPlayback( true, this );
		if ( NULL == pStartMasterDvaPlayback )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartMasterDvaPlayback" );
			return TRANSACTION_FAILED;
		}
		pStartMasterDvaPlayback->setDvaMessageId( m_theSpecifics.vecMsgIds[0] );
		pStartMasterDvaPlayback->setSourceId( m_theSpecifics.usSourceId );
		return PasTransactionManager::getInstance().addTransaction( pStartMasterDvaPlayback );
	}
}

int MasterDVABroadcast::writeStopToPas()
{
    if ( m_theSpecifics.bScheduled )
    {
        DeleteScheduleDva* pDeleteScheduleDva = new DeleteScheduleDva( true, this );

        if ( NULL == pDeleteScheduleDva )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartStationDvaBroadcast" );
            return TRANSACTION_FAILED;
        }

        pDeleteScheduleDva->setScheduleId( m_theSpecifics.usScheduleId );

        return PasTransactionManager::getInstance().addTransaction( pDeleteScheduleDva );
    }
	else if ( !m_theSpecifics.isLocalPlayback )
	{
		StopDvaBroadcast* pStopMasterDva = new StopDvaBroadcast( true, this );

		if ( NULL == pStopMasterDva )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartMasterDvaBroadcast" );
			return TRANSACTION_FAILED;
		}

		pStopMasterDva->setPasZoneKeys( m_theSpecifics.vecZoneKeys );
		pStopMasterDva->setDvaMessageIds( m_theSpecifics.vecMsgIds );

		return PasTransactionManager::getInstance().addTransaction( pStopMasterDva );
	} 
	else
	{
		StopDvaPlayback* pStopMasterDVAPlayback = new StopDvaPlayback( true, this );

		if ( NULL == pStopMasterDVAPlayback )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StopMasterDVAPlayback" );
			return TRANSACTION_FAILED;
		}

		pStopMasterDVAPlayback->setSourceId( m_theSpecifics.usSourceId );

		return PasTransactionManager::getInstance().addTransaction( pStopMasterDVAPlayback );
	}
}

} // namespace TA_IRS_App

