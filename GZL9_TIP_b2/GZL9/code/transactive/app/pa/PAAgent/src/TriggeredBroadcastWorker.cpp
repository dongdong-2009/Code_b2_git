/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "bus/pa/pa_agent/IDL/src/IPAAgentCorbaDef.h"

#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/TriggeredDVABroadcast.h"
#include "app/pa/PAAgent/src/TriggeredBroadcastWorker.h"

namespace TA_IRS_App
{

TriggeredBroadcastWorker::TriggeredBroadcastWorker() 
: 
m_thdTriggeredBroadcastTimer(),
m_bIsControl( false )
{
    m_thdTriggeredBroadcastTimer.activate();
}

TriggeredBroadcastWorker::~TriggeredBroadcastWorker()
{
    m_thdTriggeredBroadcastTimer.deactivate();
    m_thdTriggeredBroadcastTimer.wait();
}

void TriggeredBroadcastWorker::setToControl()
{
    m_bIsControl = true;
}

void TriggeredBroadcastWorker::setToMonitor()
{
    m_bIsControl = false;
}

int TriggeredBroadcastWorker::handle_timeout( const ACE_Time_Value& current_time, const void *act )
{
    TriggeredDVABroadcast* pSigBroadcast = 
        const_cast<TriggeredDVABroadcast*>(static_cast<const TriggeredDVABroadcast*>(act));

    if ( 0 == pSigBroadcast )
    {
        return 0;
    }

    if ( m_bIsControl )
    {
        try
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "ACE handle and execute SIG triggered broadcast : %s", pSigBroadcast->getBroadcastId().c_str() );
            pSigBroadcast->executeBroadcast();
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason for handle timeout broadcast" );
        }
    }

    delete pSigBroadcast;
    pSigBroadcast = 0;

    return 0;
}

void TriggeredBroadcastWorker::addSigTriggeredBroadcast( 
    time_t tmStartTime, const std::string& strBroadcastId, unsigned long ulDvaMsgKey, 
    const std::vector<unsigned long>& lstZoneKeys, unsigned long ulCyclicTimes )
{
    TriggeredDVABroadcast* pSigDvaBroadcast = new TriggeredDVABroadcast( 
        tmStartTime, SIG_TRIGGER_BROADCAST, strBroadcastId, ulDvaMsgKey, lstZoneKeys, 0 );

    if ( 0 == pSigDvaBroadcast )
    {
        return;
    }

    ACE_Time_Value atmScheduleTime( tmStartTime, 0 );
    m_thdTriggeredBroadcastTimer.schedule( this, static_cast<void*>(pSigDvaBroadcast), atmScheduleTime );
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Ace Timer scheduled SIG broadcast: %s", strBroadcastId.c_str() );

    bool bNeedSync = ( tmStartTime > time(0) );
    if ( m_bIsControl && bNeedSync )
    {
        TA_Base_Bus::IPAAgentCorbaDef::TriggeredBroadcastConfig updateData;
        updateData.strBroadcastId = strBroadcastId.c_str();
        updateData.tmStartTime = tmStartTime;
        updateData.ulCyclicTimes = ulCyclicTimes;
        updateData.ulDvaMsgKey = ulDvaMsgKey;
        PasHelpers::getInstance()->zoneKeysToCoverage( lstZoneKeys, updateData.lstZoneKeys );
        PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate( updateData );
    }
}

void TriggeredBroadcastWorker::executeSigTriggeredBroadcast(unsigned long ulSigEventType, unsigned long ulPlatFormType)
{
	TriggeredDVABroadcast oSigDvaBroadcast(ulSigEventType, ulPlatFormType);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "execute SIG triggered broadcast Event Type [%lu] and Platform Type [%lu]", ulSigEventType, ulPlatFormType);

	try
	{
        oSigDvaBroadcast.executeSigBroadcast();
	}
	catch (std::exception& expWhat)
	{
		LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason for execute SIG triggered broadcast" );
	}
}

void TriggeredBroadcastWorker::executeFasTriggeredBroadcast( const std::string& strBroadcastId,
    unsigned long ulDvaMsgKey, const std::vector<unsigned long>& lstZoneKeys, unsigned long ulCyclicTimes )
{
    TriggeredDVABroadcast oFasDvaBroadcast( 0, FAS_TRIGGER_BROADCAST, strBroadcastId, ulDvaMsgKey, lstZoneKeys, ulCyclicTimes );

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Execute FAS triggered broadcast : %s", strBroadcastId.c_str() );

    // Will throw exception if failed
    try
    {
        oFasDvaBroadcast.executeBroadcast();
    }
    catch (...)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to execute FAS triggered broadcast : %s", strBroadcastId.c_str() );
        throw;
    }
}

void TriggeredBroadcastWorker::executeFasTriggeredBroadcast(const bool bOnFire)
{
    TriggeredDVABroadcast oFasDvaBroadcast(bOnFire);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Execute FAS triggered broadcast : %s", (bOnFire ? "Fire" : "Normal") );

    try
    {
        oFasDvaBroadcast.executeFasBroadcast();
    }
    catch (std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason for execute FAS triggered broadcast");
    }
}

}
