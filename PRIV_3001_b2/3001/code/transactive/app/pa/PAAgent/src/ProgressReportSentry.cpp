/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/ProgressReportSentry.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2018/03/14 13:51:20 $
  * Last modified by : $Author: lim.cy $
  *
  *
  */

#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/ProgressReportSentry.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include <time.h>


using namespace TA_Base_Core;

namespace TA_IRS_App
{

// Thread checked
ProgressReportSentry::ProgressReportSentry( const CORBA::String_member& broadcastId, 
                                            const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones)
{
    FUNCTION_ENTRY( "ProgressReportSentry" );

    // Initialise progress report
    m_progressReport.broadcastId = broadcastId;
    m_progressReport.broadcastState = TA_Base_Bus::IPAAgentCorbaDef::Creation;
    m_progressReport.globalState = TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FAILURE;
    m_inhibitAuditMessages = false;
    m_isStationBroadcast = true;


    m_progressReport.localProgresses.length(zones.length());
    for (unsigned int i=0; i<zones.length(); ++i)
    {
        m_progressReport.localProgresses[i].zoneKeyOrTrainId = zones[i];
        resetZoneOrTrain(i);
    }

    FUNCTION_EXIT;
}


// Thread checked
ProgressReportSentry::ProgressReportSentry( const CORBA::String_member& broadcastId, 
                                            const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains)
{
    FUNCTION_ENTRY( "ProgressReportSentry" );

    // Initialise progress report
    m_progressReport.broadcastId = broadcastId;
    m_progressReport.broadcastState = TA_Base_Bus::IPAAgentCorbaDef::Creation;
    m_progressReport.globalState = TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FAILURE;
    m_inhibitAuditMessages = false;
    m_isStationBroadcast = false;

    m_progressReport.localProgresses.length(trains.length());
    for (unsigned int i=0; i<trains.length(); ++i)
    {
        m_progressReport.localProgresses[i].zoneKeyOrTrainId = trains[i];
        resetZoneOrTrain(i);
    }

    FUNCTION_EXIT;
}


// Thread checked
ProgressReportSentry::~ProgressReportSentry() 
{
    FUNCTION_ENTRY( "~ProgressReportSentry" );
    FUNCTION_EXIT;
}


void ProgressReportSentry::setProgressReport(const TA_Base_Bus::IPAAgentCorbaDef::ProgressReport& progressReport)
{
    FUNCTION_ENTRY( "setProgressReport" );

    ThreadGuard guard( m_lock );

    m_progressReport = progressReport;

    FUNCTION_EXIT;
}


// Thread checked
void ProgressReportSentry::setBroadcastId(const std::string& newBroadcastId)
{
    FUNCTION_ENTRY( "setBroadcastId" );

    ThreadGuard guard( m_lock );

    // Note CORBA::String_members will automatically free and reallocate memory for const char* assignment
    m_progressReport.broadcastId = newBroadcastId.c_str();

    setHasProgressReportChanged(true);

    FUNCTION_EXIT;
}


// Thread checked
TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* ProgressReportSentry::getProgressReportCopy()
{
    FUNCTION_ENTRY( "getProgressReportCopy" );

    ThreadGuard guard( m_lock );

    TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* newCopyP = new TA_Base_Bus::IPAAgentCorbaDef::ProgressReport();
    *newCopyP = m_progressReport; // DeepCopy
    FUNCTION_EXIT;
    return newCopyP;
}


// Thread checked
TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState ProgressReportSentry::getBroadcastState()
{
    FUNCTION_ENTRY( "getBroadcastState" );

    ThreadGuard guard( m_lock );

    FUNCTION_EXIT;
    return m_progressReport.broadcastState;
}


// Thread checked
void ProgressReportSentry::setBroadcastState (TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState state)
{
    FUNCTION_ENTRY( "setBroadcastState" );

    ThreadGuard guard( m_lock );

    if ( m_isStationBroadcast )
    {
        submitBroadcastStateAuditMessage( m_progressReport.broadcastState, static_cast<const char*>(m_progressReport.broadcastId) );
    }

    if (m_progressReport.broadcastState == state)
    {
        FUNCTION_EXIT;
        return;
    }

    m_progressReport.broadcastState = state;
    setHasProgressReportChanged(true);

    FUNCTION_EXIT;
}


void ProgressReportSentry::resetGlobalState()
{
    FUNCTION_ENTRY( "resetGlobalState" );

    ThreadGuard guard( m_lock );

    if (m_progressReport.globalState == TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FAILURE)
    {
        FUNCTION_EXIT;
        return;
    }
    m_progressReport.globalState = TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FAILURE;
    setHasProgressReportChanged(true);

    FUNCTION_EXIT;
}


void ProgressReportSentry::setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::EGlobalState globalState)
{
    FUNCTION_ENTRY( "setGlobalState" );

    ThreadGuard guard( m_lock );

    if (m_progressReport.globalState == globalState)
    {
        FUNCTION_EXIT;
        return;
    }

    // We ignore GLOBAL_STATE_NO_FAILURE so that if an error is already encountered the only
    // way to revert back to GLOBAL_STATE_NO_FAILURE is to call resetGlobalState()
    //

    if (globalState == TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FAILURE)
    {
        FUNCTION_EXIT;
        return;
    }

    submitBroadcastStateFailureAuditMessage(
        globalState,
        static_cast<const char*>(m_progressReport.broadcastId) );

    m_progressReport.globalState = globalState;
    setHasProgressReportChanged(true);

    FUNCTION_EXIT;
}


void ProgressReportSentry::setLocalState( unsigned int index, TA_Base_Bus::IPAAgentCorbaDef::ELocalState localState )
{
    FUNCTION_ENTRY( "setLocalState" );

    TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq = m_progressReport.localProgresses;

    ThreadGuard guard( m_lock );

	if (index >= theLocalProgressSeq.length())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "theLocalProgressSeq index out of bounds" );
        return;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "index:%d, currentlocalstate:%d, updatelocalstate:%d", index, theLocalProgressSeq[index].localState, localState);

    if ( localState == theLocalProgressSeq[index].localState )
    {
        // No change
        FUNCTION_EXIT;
        return;
    }

    if ( theLocalProgressSeq[index].localState == TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_ABORTED )
    {
        // Already aborted.  This overrides all other states until the zone/train is reset
        FUNCTION_EXIT;
        return;
    }

    switch (localState)
    {
    // DTL-340 delete start
    //case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_ABORTED:
    // DTL-340 delete end
    // DTL-735 add start
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_NO_RESPONSE_FROM_TRAIN:
    // DTL-735 add end
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CYCLE:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_ZONE:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_OVERRIDDEN:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_TRAIN:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONNECTION_RESET:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_INTERRUPTED:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_VERSION_CONFLICT:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_INVALID_REQUEST:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BUSY:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BUSY_WITH_OVERRIDE_OPTION:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_NO_PERMISSION:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BAD_COMMAND:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DIFFERENT_ORIGIN:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DIFFERENT_VERSION:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_MPU_CHANGEOVER:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_LINK_LOST:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DUE_TO_NO_AUDIO:
        theLocalProgressSeq[index].errorOccurred = true;
        theLocalProgressSeq[index].localState = localState;
        setHasProgressReportChanged(true);
        break;
    // DTL-340 add start
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_ABORTED:
    // DTL-340 add end
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_FIRST_CYCLE_CANCELLED: 
        theLocalProgressSeq[index].errorOccurred = false;
        theLocalProgressSeq[index].localState = localState;
        setHasProgressReportChanged(true);
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS:
	case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_BROADCASTING:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS_OVERRIDDEN:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS_DWELL_TIME:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONTINUE_LIVE_ANNOUNCEMENT_TIMEOUT: // this indicates a possible failure only
        if (!theLocalProgressSeq[index].errorOccurred)
        {
            theLocalProgressSeq[index].localState = localState;
            setHasProgressReportChanged(true);
        }
        else
        {
            // This zone/train has been flagged with an error, do not change it.
        }
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_PTT_ACTIVATED:
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED:
        // Special case for this one, display this regardless of errorflag.
        theLocalProgressSeq[index].localState = localState;
        setHasProgressReportChanged(true);
        break;
    default:
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError , "Unrecognized ELocalState %d", localState );
        theLocalProgressSeq[index].localState = localState;
        setHasProgressReportChanged(true);
        break;
    }

    // raise an audit message if there was an error
    if (theLocalProgressSeq[index].errorOccurred)
    {
        submitZoneBroadcastFailureAuditMessage(index, localState);
    }

    FUNCTION_EXIT;
}


TA_Base_Bus::IPAAgentCorbaDef::ELocalState ProgressReportSentry::getLocalState( unsigned int index )
{
    FUNCTION_ENTRY( "getLocalState" );

    TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq =
        m_progressReport.localProgresses;

    ThreadGuard guard( m_lock );

	if (index >= theLocalProgressSeq.length())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "theLocalProgressSeq index out of bounds" );
		return TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_INVALID_REQUEST;
	}

    FUNCTION_EXIT;
    return theLocalProgressSeq[index].localState;
}


void ProgressReportSentry::setOverridingSourcePriority( unsigned int index,
                                                        CORBA::Octet overridingSourcePriority )
{
    FUNCTION_ENTRY( "setOverridingSourcePriority" );

    TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq =
        m_progressReport.localProgresses;

    ThreadGuard guard( m_lock );

	if (index >= theLocalProgressSeq.length())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "theLocalProgressSeq index out of bounds" );
		return;
	}

    if ( theLocalProgressSeq[index].overridingSourcePriorityForZonesOnly != overridingSourcePriority )
    {
        theLocalProgressSeq[index].overridingSourcePriorityForZonesOnly = overridingSourcePriority;
        setHasProgressReportChanged(true);
    }

    FUNCTION_EXIT;
}


void ProgressReportSentry::setErrorOccurred( const unsigned int index, const bool bError )
{
    FUNCTION_ENTRY( "getErrorOccurred" );

    // Implicitly threadsafe
    TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq = m_progressReport.localProgresses;

	if (index >= theLocalProgressSeq.length())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "theLocalProgressSeq index out of bounds" );
		return;
	}

    theLocalProgressSeq[index].errorOccurred = static_cast<CORBA::Boolean>(bError);
    FUNCTION_EXIT;
}


bool ProgressReportSentry::getErrorOccurred( const unsigned int index )
{
    FUNCTION_ENTRY( "getErrorOccurred" );

    // Implicitly threadsafe
    TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq = m_progressReport.localProgresses;

	if (index >= theLocalProgressSeq.length())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "theLocalProgressSeq index out of bounds" );
		return true;
	}

    FUNCTION_EXIT;
    return theLocalProgressSeq[index].errorOccurred;
}


void ProgressReportSentry::resetZoneOrTrain(unsigned int index, 
    TA_Base_Bus::IPAAgentCorbaDef::ELocalState eState )
{
    FUNCTION_ENTRY( "resetZoneOrTrain" );

    TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq = m_progressReport.localProgresses;

    ThreadGuard guard( m_lock );

	if (index >= theLocalProgressSeq.length())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "theLocalProgressSeq index out of bounds" );
		return;
	}

    theLocalProgressSeq[index].localState = eState;
    theLocalProgressSeq[index].overridingSourcePriorityForZonesOnly = 0;
    theLocalProgressSeq[index].errorOccurred = false;

    FUNCTION_EXIT;
}


void ProgressReportSentry::flagNonCompletedWithErrors()
{
    FUNCTION_ENTRY( "flagNonCompletedWithErrors" );

    TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq = m_progressReport.localProgresses;

    ThreadGuard guard( m_lock );

    for (unsigned int i=0; i<theLocalProgressSeq.length(); ++i)
    {
        if ( theLocalProgressSeq[i].localState != TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED )
        {
            theLocalProgressSeq[i].errorOccurred = true;
        }
    }

    FUNCTION_EXIT;
}


// DTL-340 modify start
//void ProgressReportSentry::flagNonCompletedWithErrorsAndAborted()
void ProgressReportSentry::flagNonCompletedWithErrorsAndAborted( const bool bError )
// DTL-340 modify end
{
    FUNCTION_ENTRY( "flagNonCompletedWithErrorsAndAborted" );

    TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq = m_progressReport.localProgresses;

    ThreadGuard guard( m_lock );

    for (unsigned int i=0; i<theLocalProgressSeq.length(); ++i)
    {
        if ( theLocalProgressSeq[i].localState != TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED )
        {
            theLocalProgressSeq[i].localState = TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_ABORTED;
            // DTL-340 modify start
            //theLocalProgressSeq[i].errorOccurred = true;
            theLocalProgressSeq[i].errorOccurred = bError;
            // DTL-340 modify end
        }
    }

    FUNCTION_EXIT;
}


void ProgressReportSentry::flagLinkLostWithErrors( const bool bError )
{
    FUNCTION_ENTRY( "flagLinkLostWithErrors" );

    TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq = m_progressReport.localProgresses;

    ThreadGuard guard( m_lock );

    for (unsigned int i=0; i<theLocalProgressSeq.length(); ++i)
    {
        if ( theLocalProgressSeq[i].localState != TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED )
        {
            theLocalProgressSeq[i].localState = TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_LINK_LOST;
            theLocalProgressSeq[i].errorOccurred = bError;
        }
    }

    FUNCTION_EXIT;
}


// TD17456
void ProgressReportSentry::flagNonValidWithFailure()
{
    FUNCTION_ENTRY( "flagNonValidWithFailure" );

    TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq = 
        m_progressReport.localProgresses;
	
    ThreadGuard guard( m_lock );
	
    for (unsigned int i=0; i<theLocalProgressSeq.length(); ++i)
    {
		theLocalProgressSeq[i].localState = TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_ZONE;
		theLocalProgressSeq[i].errorOccurred = true;
    }

    FUNCTION_EXIT;
}


void ProgressReportSentry::submitBroadcastStateAuditMessage( const TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState& newState,
                                                             const std::string& broadcastId ) const
{
    FUNCTION_ENTRY( "submitBroadcastStateAuditMessage" );

    if (!m_inhibitAuditMessages)
    {
        PasHelpers::getInstance()->submitPaAgentBroadcastStateAuditMessage(
            broadcastId,
            newState );
    }

    FUNCTION_EXIT;
}


void ProgressReportSentry::submitBroadcastStateFailureAuditMessage( const TA_Base_Bus::IPAAgentCorbaDef::EGlobalState& newStateFailure,
                                                                    const std::string& broadcastId ) const
{
    FUNCTION_ENTRY( "submitBroadcastStateFailureAuditMessage" );

    if (!m_inhibitAuditMessages)
    {
        PasHelpers::getInstance()->submitPaAgentBroadcastStateFailureAuditMessage(
            broadcastId,
            newStateFailure );
    }

    FUNCTION_EXIT;
}

void ProgressReportSentry::submitZoneBroadcastFailureAuditMessage( unsigned long index,
                                                                   TA_Base_Bus::IPAAgentCorbaDef::ELocalState localState)
{
    FUNCTION_ENTRY( "submitZoneBroadcastFailureAuditMessage" );

    if (!m_inhibitAuditMessages)
    {
        //std::string reason = PasHelpers::getInstance()->formatFailReasonForAudit( localState );

        // already locked by calling method
        //TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& theLocalProgressSeq = 
        //    m_progressReport.localProgresses;

        //TA_ASSERT( index < theLocalProgressSeq.length(), "index out of bounds");

        // if its a station broadcast
        if (m_isStationBroadcast)
        {
            //PasHelpers::getInstance()->submitPaZoneBroadcastFailureAuditMessage(
            //    theLocalProgressSeq[index].zoneKeyOrTrainId, reason );
        }
        else
        {
            // if its a train broadcast
            //PasHelpers::getInstance()->submitPaTrainBroadcastFailureAuditMessage(
            //    theLocalProgressSeq[index].zoneKeyOrTrainId,
            //    CachedConfig::getInstance()->getAgentLocationKey(),
            //    reason );
        }
    }

    FUNCTION_EXIT;
}

}