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
#include "app/pa/PAAgent/src/DVABroadcastStart.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/ScheduleBroadcastWorker.h"
#include "app/pa/PAAgent/src/ScheduleBroadcast.h"

namespace TA_IRS_App
{

ScheduleBroadcast::ScheduleBroadcast( const TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& refConfig ) : 
m_pConfig( new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig(refConfig) ),
m_bEnable( true )
{
}

ScheduleBroadcast::~ScheduleBroadcast()
{
    if ( 0 != m_pConfig )
    {
        delete m_pConfig;
        m_pConfig = 0;
    }
}

void ScheduleBroadcast::executeBroadcast() const
{
    if ( 0 == m_pConfig )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "executeBroadcast while config is null" );
        return;
    }

    std::string broadcastId = m_pConfig->broadcastId._ptr;
    DVABroadcastStart newBroadcast( broadcastId );

    bool bSendToHardware = true;
    try
    {
        // update parameters
        std::vector<unsigned long> lstZoneKeys;
        PasHelpers::getInstance()->coverageToZoneKeys( m_pConfig->theBroadcastConfigSpecifics.theStationDvaConfig().zones, lstZoneKeys );
        newBroadcast.updateDVAMsgIdByDVAMsgKey( m_pConfig->theBroadcastConfigSpecifics.theStationDvaConfig().messages[0] );
        newBroadcast.updateCylicTimes( m_pConfig->theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.usCyclicTimes );
        newBroadcast.updatePriorityId( m_pConfig->theBroadcastConfigSpecifics.theStationDvaConfig().priorityGroup );
        newBroadcast.updateStationZoneIdsByZoneKeys( lstZoneKeys );

        if ( newBroadcast.isParameterInvalid() )
        {
            bSendToHardware = false;
            std::string strBadParams = newBroadcast.getInvalidParameter();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Triggered broadcast can't be sent in case of bad parameter(s): %s", strBadParams.c_str() );
        }
    }
    catch(...)
    {
        bSendToHardware = false;
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
        throw;
    }

    if ( bSendToHardware )
    {
        try
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "-------------------writeToHardwareImmediately--------------------" );
            PASConnection::getInstance()->writeToHardwareImmediately( &newBroadcast );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason for PASConnection::writeToHardwareImmediately" );
            throw;
        }
    }
}

time_t ScheduleBroadcast::getStartTime() const
{
    if ( 0 == m_pConfig )
    {
        return 0;
    }

    return m_pConfig->theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.startTime;
}

unsigned long ScheduleBroadcast::getPkey() const
{
    if ( 0 == m_pConfig )
    {
        return 0;
    }

    return m_pConfig->theBroadcastConfigSpecifics.theStationDvaConfig().pKey;
}

void ScheduleBroadcast::setToEnable()
{
    m_bEnable = true;
}

void ScheduleBroadcast::setToDisable()
{
    m_bEnable = false;
}

bool ScheduleBroadcast::isEnable()
{
    return m_bEnable;
}

TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& ScheduleBroadcast::getConfig() const
{
    return (*m_pConfig);
}

}
