#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/StationMusicBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2012/06/12 13:35:44 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include <time.h>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "app/pa/PAAgent/src/StationMusicBroadcast.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{

StationMusicBroadcast::StationMusicBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                             const std::string& sessionId)
 : AbstractStationBroadcast( broadcastConfigP, sessionId ),
   m_theSpecifics( broadcastConfigP->theBroadcastConfigSpecifics.theStationMusicConfig() )
{
    TA_ASSERT(!CachedConfig::getInstance()->getIsMaster(), "StationMusicBroadcast is not available for the Master PA Agent");
}

StationMusicBroadcast::~StationMusicBroadcast() 
{
}

void StationMusicBroadcast::validateBroadcastConfig() //throw(TA_Base_Core::BadParameterException) 
{
    // Source Id has been checked by the broadcast manager
    // Zones is not relevant
}

void StationMusicBroadcast::execute() 
{
    ThreadGuard guard( m_lock );

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage nullZones;
    nullZones.length(0);

    setWasExecutedSuccessfully(false);

    // if (m_theSpecifics.musicType == TA_Base_Bus::IPAAgentCorbaDef::NoMusic)
    if ( m_theSpecifics.musicType == TA_Base_Bus::IPAAgentCorbaDef::NoMusic )
    {
        try
        {
            // Only the correct type should be terminated.
            if (m_theSpecifics.musicType == TA_Base_Bus::IPAAgentCorbaDef::NoMusic)
            {
                // Terminate OccMusic:
            }
            else
            {
                // Terminate LocalMusic:
               if (CachedConfig::getInstance()->getAgentLocationKey() == LocationAccessFactory::getInstance().getDepotLocationKey())
               {
                   // TBD
               }
               else
               {
                   // TBD
               }
            }
        }
        catch(TA_Base_Core::PasWriteErrorException& e)
        {
            // Serious error - terminate
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
            return;
        }
        catch(TA_Base_Core::PasConnectionException&)
        {
            // Serious error - terminate
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
            return;
        }   
    }
    else
    {
        // Local or OCC music
        
        try
        {
            setAnnounceId(AbstractBroadcastCircularList::getInstance()->getNextAvailableStationAnnounceId());
        }
        catch(TA_Base_Core::NoFreeAnnounceIdException&)
        {
            // Serious error - terminate
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_ANNOUNCE_ID);
            return;
        }

        try
        {
            // TBD
        }
        catch(TA_Base_Core::PasWriteErrorException& e)
        {
            // Serious error - terminate
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
            return;
        }
        catch(TA_Base_Core::PasConnectionException&)
        {
            // Serious error - terminate
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
            return;
        }

    }

    // We can skip to final state as this is a once off request with no runtime functionality
    // but we go with the flow to follow the other broadcast types
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Runtime);

    // Clear any past errors only for the transition from Execution to Runtime
    // In the case of the transition from Execution to Termination to PendingRemoval,
    // we want to retain the GlobalState so that the PA Manager can see the
    // errors encountered.

    m_progressReportSentry->resetGlobalState();

    setWasExecutedSuccessfully(true);
}

void StationMusicBroadcast::terminate() 
{
    // Does not apply
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval);
}

void StationMusicBroadcast::runtimeSpecifics() 
{
    // Does not apply
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
}

} // namespace TA_IRS_App

