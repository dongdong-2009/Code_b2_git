#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/StationMusicBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include <time.h>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "app/pa/PAAgent/src/StationMusicBroadcast.h"
#include "app/pa/PAAgent/src/Table201.h"
#include "app/pa/PAAgent/src/Table302.h"
#include "app/pa/PAAgent/src/Table202.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{

StationMusicBroadcast::StationMusicBroadcast( Table201& table201,
                                              Table202& table202,
                                              Table301& table301,
                                              Table302& table302,
                                              TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                             const std::string& sessionId)
 : AbstractStationBroadcast( broadcastConfigP, sessionId ),
   m_theSpecifics( broadcastConfigP->theBroadcastConfigSpecifics.theStationMusicConfig() ),
   m_table201(table201),
   m_table202(table202),
   m_table301(table301),
   m_table302(table302)
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

    // liuyu++ TD8857
    // if (m_theSpecifics.musicType == TA_Base_Bus::IPAAgentCorbaDef::NoMusic)
    if ( (m_theSpecifics.musicType == TA_Base_Bus::IPAAgentCorbaDef::NoOccMusic) || 
         (m_theSpecifics.musicType == TA_Base_Bus::IPAAgentCorbaDef::NoLocalMusic) )
    // ++liuyu TD8857
    {
        try
        {
            // liuyu++ TD8857
            // Only the correct type should be terminated.
            if (m_theSpecifics.musicType == TA_Base_Bus::IPAAgentCorbaDef::NoOccMusic)
            {
                // Terminate OccMusic:
                m_table201.setTableDataAndWrite(TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_OCR_MUSIC, 0, nullZones);
            }
            else
            {
                // Terminate LocalMusic:
               if (CachedConfig::getInstance()->getAgentLocationKey() == LocationAccessFactory::getInstance().getDepotLocationKey())
               {
                    m_table201.setTableDataAndWrite(TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_DEPOT_MUSIC, 0, nullZones);
               }
               else
               {
                   m_table201.setTableDataAndWrite(TA_Base_Bus::IPAAgentCorbaDef::PA_SRC_STATION_MUSIC, 0, nullZones);
               }
            }
            // ++liuyu TD8857
        }
        catch(TA_Base_Core::PasWriteErrorException& e)
        {
            if (e.getErrorValue() == AbstractPASTable::PAS_ERR_INVALID_ANNOUNCE)
            {
                // Not expected to occur
                abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
                return;
            }
            else
            {
                // PAS_ERR_GENERIC or anything else

                // Serious error - terminate
                abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
                return;
            }
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
            m_table201.setTableDataAndWrite(m_theSpecifics.sourceId, getAnnounceId(), nullZones);
        }
        catch(TA_Base_Core::PasWriteErrorException& e)
        {
            if (e.getErrorValue() == AbstractPASTable::PAS_ERR_INVALID_ANNOUNCE)
            {

                // Somehow this announceId has been tied up in a dead broadcast and 
                // this Agent instance has no knowledge of it.  We will endeavour to
                // kill it and retry the broadcast

                try
                {
                    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage nullZones;
                    nullZones.length(0);

                    m_table201.setTableDataAndWrite( m_table202.getSourceId(getAnnounceId()),
                                                     getAnnounceId(),
                                                     nullZones );
                    m_table301.setTableDataAndWrite( m_table302.getSequenceId(getAnnounceId()),
                                                     0,
                                                     0,
                                                     0,
                                                     0,
                                                     false,
                                                     nullZones );
                }
                catch(TA_Base_Core::PasConnectionException&)
                {
                    // Serious error - terminate
                    abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
                    return;
                }


                setAnnounceId(0);

                determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_ANNOUNCE_ID);
                return;
            }
            else
            {
                // PAS_ERR_GENERIC or anything else

                // Serious error - terminate
                abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
                return;
            }
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

