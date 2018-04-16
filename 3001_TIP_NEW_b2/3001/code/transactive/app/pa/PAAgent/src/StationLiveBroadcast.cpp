/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StationLiveBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #8 $
  * Last modification : $DateTime: 2014/02/10 16:54:21 $
  * Last modified by : $Author: qi.huang $
  *
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PasExceptions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/StationLiveBroadcast.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/StartLiveBroadcast.h"


namespace TA_IRS_App
{

StationLiveBroadcast::StationLiveBroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
NonTrainLiveBroadcast( broadcastConfigP, sessionId )
{
}

StationLiveBroadcast::~StationLiveBroadcast() 
{
}

void StationLiveBroadcast::validateBroadcastConfigSpecifics() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException) 
{
    TA_THREADGUARD( m_lock );

    unsigned long myLocationKey = CachedConfig::getInstance()->getAgentLocationKey();

    for (unsigned int index = 0; index< m_theSpecifics.vecZoneKeys.size(); ++index)
    {
        // Test validity of zone keys in database
        try
        {
            unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(m_theSpecifics.vecZoneKeys[index]);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "A requested zone does not exist in the database" );
        }

        // Zones can only be from this location
        unsigned long locationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(m_theSpecifics.vecZoneKeys[index]);
        if (locationKey != myLocationKey)
        {
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "A requested zone is not within this station's scope" );
        }
    }
}

int StationLiveBroadcast::writeStartToPas()
{
	StartLiveBroadcast* pStartStationLiveBroadcast = new StartLiveBroadcast( false, this );

	if ( NULL == pStartStationLiveBroadcast )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartStationLiveBroadcast" );
		return TRANSACTION_FAILED;
	}

	pStartStationLiveBroadcast->setPasZoneKeys( m_theSpecifics.vecZoneKeys );
	pStartStationLiveBroadcast->setWithChime( m_theSpecifics.bWithChime );
	pStartStationLiveBroadcast->setSourceId( m_theSpecifics.usSourceId );
	pStartStationLiveBroadcast->setTransactionId(m_announceId);//added by lixiaoxia

	return PasTransactionManager::getInstance().addTransaction( pStartStationLiveBroadcast );
}

int StationLiveBroadcast::writeStopToPas()
{
    return 0;
}

} // namespace TA_IRS_App

