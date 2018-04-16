/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/MasterLiveBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2018/03/14 13:51:20 $
  * Last modified by : $Author: lim.cy $
  *
  *
  */


#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PasExceptions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/MasterLiveBroadcast.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/StartLiveBroadcast.h"



namespace TA_IRS_App
{

MasterLiveBroadcast::MasterLiveBroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
NonTrainLiveBroadcast( broadcastConfigP, sessionId )
{
}

MasterLiveBroadcast::~MasterLiveBroadcast() 
{
}

void MasterLiveBroadcast::validateBroadcastConfigSpecifics() //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException) 
{
    TA_THREADGUARD( m_lock );

    // Test validity of zone keys
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
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "A requested zone does not exist in the database");
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
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "An ISCS station location could not be resolved to a PAS location");
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "An ISCS station location could not be resolved to a PAS location" );
        }
    }
}

int MasterLiveBroadcast::writeStartToPas()
{
	StartLiveBroadcast* pStartMasterLiveBroadcast = new StartLiveBroadcast( true, this );

	if ( NULL == pStartMasterLiveBroadcast )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create instance of StartMasterLiveBroadcast" );
		return TRANSACTION_FAILED;
	}

	pStartMasterLiveBroadcast->setPasZoneKeys( m_theSpecifics.vecZoneKeys );
	pStartMasterLiveBroadcast->setWithChime( m_theSpecifics.bWithChime );
	pStartMasterLiveBroadcast->setSourceId( m_theSpecifics.usSourceId );
	pStartMasterLiveBroadcast->setTransactionId(m_announceId);//lixiaoxia

	return PasTransactionManager::getInstance().addTransaction( pStartMasterLiveBroadcast );
}

int MasterLiveBroadcast::writeStopToPas()
{
    return 0;
}

} // namespace TA_IRS_App

