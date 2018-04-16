/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StationDVABroadcast.cpp $
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
#include "app/pa/PAAgent/src/StationDVABroadcast.h"
#include "app/pa/PAAgent/src/StartDvaBroadcast.h"
#include "app/pa/PAAgent/src/StartDvaPlayback.h"
#include "app/pa/PAAgent/src/CreateScheduleDva.h"
#include "app/pa/PAAgent/src/StopDvaBroadcast.h"
#include "app/pa/PAAgent/src/StopDvaPlayback.h"
#include "app/pa/PAAgent/src/DeleteScheduleDva.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"



namespace TA_IRS_App
{

StationDVABroadcast::StationDVABroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
//AbstractStationBroadcast( broadcastConfigP, sessionId ),
NonTrainDVABroadcast( broadcastConfigP, sessionId )
{
}

StationDVABroadcast::~StationDVABroadcast() 
{
}

void StationDVABroadcast::validateBroadcastConfigSpecifics() 
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    TA_THREADGUARD( m_lock );

    unsigned long myLocationKey = CachedConfig::getInstance()->getAgentLocationKey();

    // Zones can only be from this location
    if ( !m_theSpecifics.isLocalPlayback )
    {
        for (unsigned int i=0; i<m_theSpecifics.vecZoneKeys.size(); ++i)
        {
            unsigned long locationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(m_theSpecifics.vecZoneKeys[i]);
            if (locationKey != myLocationKey)
            {
                PasHelpers::getInstance()->throwPaErrorException( 
                    TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "A requested zone is not within this station's scope" );
            }
        }
    }

    // Messages can only be from this location 
	for ( std::vector<unsigned long>::iterator itLoop = m_theSpecifics.vecMsgKeys.begin(); itLoop != m_theSpecifics.vecMsgKeys.end(); ++itLoop )
	{
        unsigned long locationKey = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageLocationKeyFromKey( *itLoop );
        if (locationKey != myLocationKey)
        {
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "A requested message is not within this station's scope" );
		}
    }
}

int StationDVABroadcast::writeStartToPas()
{
    if ( m_theSpecifics.bScheduled )
    {
        CreateScheduleDva* pCreateScheduleDva = new CreateScheduleDva( false, this );

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
	else if (!m_theSpecifics.isLocalPlayback)
	{
		StartDvaBroadcast* pStartStationDva = new StartDvaBroadcast( false, this );

		if ( NULL == pStartStationDva )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartStationDvaBroadcast" );
			return TRANSACTION_FAILED;
		}

		pStartStationDva->setPasZoneKeys( m_theSpecifics.vecZoneKeys );
		pStartStationDva->setWithChime( m_theSpecifics.hasChime );
		pStartStationDva->setDvaMessageIds( m_theSpecifics.vecMsgIds );
		pStartStationDva->setTransactionId( m_announceId );//added by lixiaoxia
		pStartStationDva->setDwellPeriod( m_theSpecifics.dwellInSecs );

		return PasTransactionManager::getInstance().addTransaction( pStartStationDva );
    }
	else
	{
		StartDvaPlayback* pStartStationDvaPlayback = new StartDvaPlayback( false, this );

		if ( NULL == pStartStationDvaPlayback )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartStationDvaPlayback" );
			return TRANSACTION_FAILED;
		}

		pStartStationDvaPlayback->setDvaMessageId( m_theSpecifics.vecMsgIds[0] );
		pStartStationDvaPlayback->setSourceId( m_theSpecifics.usSourceId );

		return PasTransactionManager::getInstance().addTransaction( pStartStationDvaPlayback );
	}
}

int StationDVABroadcast::writeStopToPas()
{
    if ( m_theSpecifics.bScheduled )
    {
        DeleteScheduleDva* pDeleteScheduleDva = new DeleteScheduleDva( false, this );

        if ( NULL == pDeleteScheduleDva )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartStationDvaBroadcast" );
            return TRANSACTION_FAILED;
        }

        pDeleteScheduleDva->setScheduleId( m_theSpecifics.usScheduleId );

        return PasTransactionManager::getInstance().addTransaction( pDeleteScheduleDva );
    }
	else if (!m_theSpecifics.isLocalPlayback)
	{
		StopDvaBroadcast* pStopStationDvaBroadcast = new StopDvaBroadcast( false, this );

		if ( NULL == pStopStationDvaBroadcast )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StopStationDvaBroadcast" );
			return TRANSACTION_FAILED;
		}

		pStopStationDvaBroadcast->setDvaMessageIds(m_theSpecifics.vecMsgIds);
		pStopStationDvaBroadcast->setPasZoneKeys(m_theSpecifics.vecZoneKeys);

		return PasTransactionManager::getInstance().addTransaction( pStopStationDvaBroadcast );
	} 
	else
	{
		StopDvaPlayback* pStopStationDVAPlayback = new StopDvaPlayback( false, this );

		if ( NULL == pStopStationDVAPlayback )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StopStationDVAPlayback" );
			return TRANSACTION_FAILED;
		}

		pStopStationDVAPlayback->setSourceId( m_theSpecifics.usSourceId );

		return PasTransactionManager::getInstance().addTransaction( pStopStationDVAPlayback );
	}
}

} // namespace TA_IRS_App

