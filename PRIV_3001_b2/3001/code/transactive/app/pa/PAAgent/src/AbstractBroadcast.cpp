/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/AbstractBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2018/03/14 13:51:20 $
  * Last modified by : $Author: lim.cy $
  *
  *
  */

#include <time.h>
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "app/pa/PAAgent/src/AbstractBroadcast.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{

const int AbstractBroadcast::MAX_NON_FATAL_RETRIES = 3;


// Thread checked
AbstractBroadcast::AbstractBroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                     const std::string& sessionId)
: m_broadcastConfigV( broadcastConfigP ),
  m_progressReportSentry(NULL),
  m_isTaggedForRemoval(false),
  m_retries(0),
  m_wasExecutedSuccessfully(false),
  m_bTaggedForAborted( false )
{
    m_effectiveCoverageIndices.length(0);
    m_lastUpdate = time(NULL);

    // This parameter is set manually here instead of in the BroadcastManager when the 
    // broadcastConfig structure is created to minimise changes to existing code.
    setSessionId(sessionId, false);
}

// Thread checked
AbstractBroadcast::~AbstractBroadcast() 
{
}

// Thread checked
void AbstractBroadcast::updateRuntimeState()
{
    runtimeSpecifics();

    ThreadGuard guard( m_lock );
    m_lastUpdate = time(NULL);
}

// Thread checked
TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* AbstractBroadcast::getBroadcastConfigCopy()
{
    ThreadGuard guard( m_lock );

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newCopyP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
    *newCopyP = m_broadcastConfigV; // DeepCopy
    return newCopyP;   
}

// Thread checked
std::string AbstractBroadcast::getBroadcastId()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    return m_broadcastConfigV->broadcastId.in();
}

// Thread checked
void AbstractBroadcast::setBroadcastId(const std::string& newBroadcastId)
{
	if(NULL == m_progressReportSentry)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_progressReportSentry is NULL" );
		TA_THROW(TA_Base_Core::TransactiveException("m_progressReportSentry is NULL"));
	}
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    // Note CORBA::String_members will automatically free and reallocate memory for const char* assignment
    m_broadcastConfigV->broadcastId = newBroadcastId.c_str();

    // Do the same for the progress report
    m_progressReportSentry->setBroadcastId(newBroadcastId);
}

TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType AbstractBroadcast::getBroadcastType()
{
    TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

    return m_broadcastConfigV->broadcastType;
}

// Thread checked
bool AbstractBroadcast::getIsEventTriggered()
{
    ThreadGuard guard( m_lock );

    return m_broadcastConfigV->isEventTriggered;
}

// Thread checked
time_t AbstractBroadcast::getLastUpdate()
{
    ThreadGuard guard( m_lock );

    return m_lastUpdate;
}

void AbstractBroadcast::abnormalContinuation(TA_Base_Bus::IPAAgentCorbaDef::EGlobalState fault)
{
    m_progressReportSentry->setGlobalState(fault);
    m_progressReportSentry->flagNonCompletedWithErrors();
}

void AbstractBroadcast::abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::EGlobalState fault)
{
    m_progressReportSentry->setGlobalState(fault);
    m_progressReportSentry->flagNonCompletedWithErrorsAndAborted();

    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
}

void AbstractBroadcast::linklostTermination( TA_Base_Bus::IPAAgentCorbaDef::EGlobalState fault )
{
    m_progressReportSentry->setGlobalState(fault);
    m_progressReportSentry->flagLinkLostWithErrors();

    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
}

// TD17456
void AbstractBroadcast::noneValidZoneTermination(TA_Base_Bus::IPAAgentCorbaDef::EGlobalState fault)
{
	m_progressReportSentry->setGlobalState(fault);
	m_progressReportSentry->flagNonValidWithFailure();

	setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
}
// TD17456

void AbstractBroadcast::operatorAbort()
{
    if ( getIsTaggedForAborted() )
    {
        return;
    }

    setIsTaggedForAborted( true );

    //m_progressReportSentry->flagNonCompletedWithErrorsAndAborted();

    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Aborting);
}


// Thread checked
void AbstractBroadcast::resetRetries()
{
    ThreadGuard guard( m_lock );

    m_retries = 0;
}

void AbstractBroadcast::determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::EGlobalState fault)
{
    ThreadGuard guard( m_lock );

    ++m_retries;

    if (m_retries > MAX_NON_FATAL_RETRIES)
    {
        // Serious error - terminate
        linklostTermination(fault);
    }
    else
    {
        // Try again next iteration
        m_progressReportSentry->setGlobalState(fault);   
    }
}

// Thread checked
void AbstractBroadcast::updateFinalProgressReport( const bool bAbortedIsError )
{
    ThreadGuard guard( m_lock );

    // Update the final progress report
    for (unsigned int indexIndex = 0; indexIndex< m_effectiveCoverageIndices.length(); ++indexIndex)
    {
        bool bError = m_progressReportSentry->getErrorOccurred( m_effectiveCoverageIndices[indexIndex] );
        TA_Base_Bus::IPAAgentCorbaDef::ELocalState eState = m_progressReportSentry->getLocalState( m_effectiveCoverageIndices[indexIndex] );
       
        if ( m_bTaggedForAborted && ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_ABORTED != eState ))
        {
            m_progressReportSentry->setLocalState( m_effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_ABORTED );
            m_progressReportSentry->setErrorOccurred( m_effectiveCoverageIndices[indexIndex], bAbortedIsError );
        }
        else if ( !bError )
        {
            // Only set to completed if no errors were encountered
            m_progressReportSentry->setLocalState( m_effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED);
        }    
    }

    setIsTaggedForAborted( false );
}

bool AbstractBroadcast::isBusy()
{
    int nState = getBroadcastState();

    if (( nState == TA_Base_Bus::IPAAgentCorbaDef::Execution ) || 
        ( nState == TA_Base_Bus::IPAAgentCorbaDef::Creation ) || 
        ( nState == TA_Base_Bus::IPAAgentCorbaDef::Aborting ))
    {
        // Busy
        return true;
    }

    return false;
}

} // namespace TA_IRS_App

