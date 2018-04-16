/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/GenericStatusModel.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * GenericStatusModel controls the shared business logic for broadcast status dialog screens
  */

#include "app/trains/TrainBorne_PA/src/StdAfx.h"

//#include "app/trains/TrainBorne_PA/src/CachedConfig.h"
#include "app/trains/TrainBorne_PA/src/GenericStatusModel.h"
//#include "app/trains/TrainBorne_PA/src/PAManagerModel.h"
//#include "app/trains/TrainBorne_PA/src/StationStatusHelper.h"
#include "app/trains/TrainBorne_PA/src/TrainStatusHelper.h"
#include "app/trains/TrainBorne_PA/src/PAErrorHandler.h"
#include "app/trains/TrainBorne_PA/src/CachedConfig.h"

#include "app/trains/TrainBorne_PA/src/PaRightsManager.h"

#include "core/utilities/src/DebugUtil.h"

#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


GenericStatusModel::GenericStatusModel(GenericStatusPage& page, 
                                       const std::string& broadcastId,
                                       const EModelSubType& subType)
:
m_pageView(page),
m_progressReportSubscriber(page, broadcastId),
m_broadcastId(broadcastId),
m_modelSubType(subType)
{
    FUNCTION_ENTRY( "GenericStatusModel" );
    FUNCTION_EXIT;
}


GenericStatusModel::~GenericStatusModel()
{
    FUNCTION_ENTRY( "~GenericStatusModel" );
    FUNCTION_EXIT;
}


TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState GenericStatusModel::processProgressUpdate()
{
    FUNCTION_ENTRY( "processProgressUpdate" );

    // Obtain the packet we need to access for processing - ValueNotSetException
    // may propogate from this call
    const LockedPacketProgressReport lockedProgressReport = m_progressReportSubscriber.getProgressReport();

    // Obtain direct reference to data
    const TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData& lockedData = lockedProgressReport.getLockedData();

    processProgressUpdateData(lockedData);

    FUNCTION_EXIT;
    return lockedData.broadcastState;
}


void GenericStatusModel::processProgressUpdateData(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData& lockedData)
{
    FUNCTION_ENTRY( "processProgressUpdateData" );

    try
    {
        // Update the page view model as required
        m_pageView.updateReportLists(lockedData);
    }
    catch (const TA_Base_Core::TransactiveException&)
    {
        // Some of the items in the list won't have been updated properly
        PaErrorHandler::displayError(PaErrorHandler::DATABASE_ERROR);
    }

    FUNCTION_EXIT;
}


// AZ++ TD10377 -- added parameter to optionally display error message dialog or
// throw exception
void GenericStatusModel::forceProgressReportUpdate(bool pShowErrorMessageDialog)
{
    FUNCTION_ENTRY( "forceProgressReportUpdate" );

    try
    {
        m_progressReportSubscriber.forceProgressUpdate();
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );

        if (!pShowErrorMessageDialog)
        {
            throw;
        }

        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastIdNotFound:
            PaErrorHandler::displayError(PaErrorHandler::BROADCAST_ID_NOT_FOUND);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied:
            PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
            break;
        default:
            break;
        }
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        // Progress report unable to be updated
        if (pShowErrorMessageDialog)
        {
            PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
        }
        else
        {
            FUNCTION_EXIT;
            throw;
        }
    }
    catch (const CORBA::Exception&)
    {
        // Comms error
        if (pShowErrorMessageDialog)
        {
            PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
        }
        else
        {
            FUNCTION_EXIT;
            throw;
        }
    }

    FUNCTION_EXIT;
}


// ++AZ TD10377


void GenericStatusModel::refreshSubscription()
{
    FUNCTION_ENTRY( "refreshSubscription" );

    m_progressReportSubscriber.resetSubscription(getBroadcastId());

    FUNCTION_EXIT;
}


std::auto_ptr<TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig> GenericStatusModel::getBroadcastConfig()
{
    FUNCTION_ENTRY( "getBroadcastConfig" );

    CWaitCursor hourGlassCursor;

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* result = NULL;//limin
    CORBA_CALL_RETURN( result, CachedConfig::getInstance()->getAgentObject(), getBroadcastConfig, ( getBroadcastId().c_str() ) );//limin

    FUNCTION_EXIT;
    return std::auto_ptr<TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig>(result);
}


CString GenericStatusModel::formatDateIntoString(time_t time)
{
    FUNCTION_ENTRY( "formatDateIntoString" );

    // In format "17:30 - 8 Sep"
    CString dateFormatStr("%H:%M - %d %b");

    FUNCTION_EXIT;
    return CTime(time).Format(dateFormatStr);
}


CString GenericStatusModel::formatTimeOfDayIntoString(int hour, int min)
{
    FUNCTION_ENTRY( "formatTimeOfDayIntoString" );

    CString result;
    // In format "08:05"
    result.Format("%02d:%02d", hour, min);

    FUNCTION_EXIT;
    return result;
}


void GenericStatusModel::retryFailedBroadcast()
{
    FUNCTION_ENTRY( "retryFailedBroadcast" );

    retryBroadcast(FAILED);

    FUNCTION_EXIT;
}


void GenericStatusModel::retryAllBroadcast()
{
    FUNCTION_ENTRY( "retryAllBroadcast" );

    retryBroadcast(ALL);

    FUNCTION_EXIT;
}


void GenericStatusModel::retryBroadcast(const GenericStatusModel::ERetryMode& mode)
{
    FUNCTION_ENTRY( "retryBroadcast" );

    // Ensure progress report exists and is up to date, required
    // for retrieval of station zones or train IDs (obtained from the progress report)
    forceProgressReportUpdate(true);

    try
    {
        // We access the cached report data from m_progressReportSubscriber
        // as there isn't a direct query to collect zones in a specific broadcast group
        // Obtain the packet we need to access for processing (but don't purge it from queue)
        const LockedPacketProgressReport lockedProgressReport =
                            m_progressReportSubscriber.getProgressReport(true);

        if (isStationModel())
        {
            //StationStatusHelper::retryStationBroadcast(
            //                                getBroadcastId(),
            //                                lockedProgressReport,
            //                                mode);
        }
        else if (isTrainModel())
        {
            TrainStatusHelper::retryTrainBroadcast(
                                    getBroadcastId(),
                                    lockedProgressReport,
                                    mode,
                                    getHasOverrideOption());
        }
    }
    catch (const TA_Base_Core::ValueNotSetException&)
    {
        // Progress report data not set, operation must be aborted (request ignored)
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }

    // Prompt for an immediate update of progress report (in an effort to become
    // up to date immediately)
    forceProgressReportUpdate(true);

    FUNCTION_EXIT;
}


bool GenericStatusModel::getHasOverrideOption()
{
    FUNCTION_ENTRY( "getHasOverrideOption" );

    if (!isTrainModel())
    {
        // Only the train models have an override option
        TA_ASSERT(FALSE, "Invalid context");

        FUNCTION_EXIT;
        return false;
    }
    else
    {
        // All train models need to define this function
        TA_ASSERT(FALSE, "No implementation for getHasOverrideOption");

        FUNCTION_EXIT;
        return false;
    }
}


void GenericStatusModel::terminateBroadcast()
{
    FUNCTION_ENTRY( "terminateBroadcast" );

    CWaitCursor hourGlassCursor;

    // Allow all exceptions to propogate upwards
    CORBA_CALL( CachedConfig::getInstance()->getAgentObject(), terminateBroadcast, (
                                            getBroadcastId().c_str(),
                                            CachedConfig::getInstance()->getSessionId().c_str() ) );

    FUNCTION_EXIT;
}


bool GenericStatusModel::terminateAndRemoveBroadcast(EBroadcastOrigin bcc)
{
    FUNCTION_ENTRY( "terminateAndRemoveBroadcast" );

    std::vector<std::string> broadcastIds;
    broadcastIds.push_back(getBroadcastId());

    //int result = PaManagerModel::terminateAndRemoveBroadcastIds(
    //                                                broadcastIds,
    //                                                CachedConfig::getInstance()->getAgentObject(),
    //                                                bcc);

    FUNCTION_EXIT;
    //return result > 0;
    return true;
}


const std::string GenericStatusModel::getStatusString(const TA_Base_Bus::IPAAgentCorbaDef::ELocalState& status)
{
    FUNCTION_ENTRY( "getStatusString" );

    switch (status)
    {
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS:
        FUNCTION_EXIT;
        return "In progress";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS_DWELL_TIME:
        FUNCTION_EXIT;
        return "In progress (dwell time)";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING:
        FUNCTION_EXIT;
        return "Connecting";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_READY:
        FUNCTION_EXIT;
        return "Train ready";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONTINUE_LIVE_ANNOUNCEMENT_REQUIRED:
        FUNCTION_EXIT;
        return "Continue required";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED:
        FUNCTION_EXIT;
        return "Completed";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_ABORTED:
        FUNCTION_EXIT;
        return "Aborted";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CYCLE:
        FUNCTION_EXIT;
        return "Failed cycle";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_ZONE:
        FUNCTION_EXIT;
        return "Failed zone";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_OVERRIDDEN:
        FUNCTION_EXIT;
        return "Overridden";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_TRAIN:
        FUNCTION_EXIT;
        return "Failed Train";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONNECTION_RESET:
        FUNCTION_EXIT;
        return "Connection reset";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_INTERRUPTED:
        FUNCTION_EXIT;
        return "Interrupted";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_VERSION_CONFLICT:
        FUNCTION_EXIT;
        return "Version conflict";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_INVALID_REQUEST:
        FUNCTION_EXIT;
        return "Invalid request";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BUSY:
        FUNCTION_EXIT;
        return "Busy";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BUSY_WITH_OVERRIDE_OPTION:
        FUNCTION_EXIT;
        return "Busy with override option";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CONTINUE_LIVE_ANNOUNCEMENT_TIMEOUT:
        FUNCTION_EXIT;
        return "Live timeout";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_TRAIN_FIRST_CYCLE_CANCELLED:
        FUNCTION_EXIT;
        return "First Cycle Cancelled";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_NO_PERMISSION:
        FUNCTION_EXIT;
        return "No Permission";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_BAD_COMMAND:
        FUNCTION_EXIT;
        return "Bad Command";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DIFFERENT_ORIGIN:
        FUNCTION_EXIT;
        return "Different Origin";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DIFFERENT_VERSION:
        FUNCTION_EXIT;
        return "Version Mismatch";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_MPU_CHANGEOVER:
        FUNCTION_EXIT;
        return "Failed due to changeover";
    // DTL-735 add start
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_NO_RESPONSE_FROM_TRAIN:
        FUNCTION_EXIT;
        return "Time-out";
    case TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_DUE_TO_NO_AUDIO:
        FUNCTION_EXIT;
        return "Failed due to no audio";
    // DTL-735 add end
    default:
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
             "GenericStatusModel::getStatusString: Unrecognized status (%d)", status );

        //TA_THROW(TA_Base_Core::TransactiveException("Unrecognized local status"));
        FUNCTION_EXIT;
        return "???";
    }
}

std::vector<std::string> GenericStatusModel::getDvaBroadcastZones( PAAgentNamedObject& paAgent, const TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType& filter, ERightsCheckingMode filterByRightToView /*= USE_RIGHTS_CHECKING*/ )
{
	CWaitCursor hourGlassCursor;

	std::vector<std::string>    result;
	TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* broadcastIds = NULL;

	try
	{
		CORBA_CALL_RETURN( broadcastIds, paAgent, getBroadcastIdTypes, () );//limin
	}
	catch (const TA_Base_Core::ObjectResolutionException&)
	{
		throw;
	}
	catch(const CORBA::Exception&)
	{
		// Some unspecific error occurred with CORBA servant, just means an empty array
		// (reduced functionality error)
		PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
		return result;
	}

	TA_ASSERT(broadcastIds != NULL, "broadcastIds NULL");


	for (unsigned int i = 0; i < broadcastIds->length(); i ++)
	{
		const TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdType& item = (*broadcastIds)[i];

		if (filter == item.broadcastType)
		{
			bool placeInOutput = false;

			// We can only add those items we have a right to display - unless rights checking has been override
			if (IGNORE_RIGHTS_CHECKING == filterByRightToView)
			{
				placeInOutput = true;   
			}
			else if (PaRightsManager::getInstance().hasRightToDisplayBroadcastWithSessionId(
				std::string(item.sessionId.in()) ) )
			{
				// Else not ignoring rights checking, only place in output if appropriate rights present
				placeInOutput = true;
			}   

			if (placeInOutput)
			{
				result.push_back(std::string(item.broadcastId.in()));
			}     
		}
	}

	// Clean up
	delete broadcastIds;
	broadcastIds = 0;

	return result;
}

int GenericStatusModel::terminateAndRemoveBroadcastIds( const std::vector<std::string>& broadcastIds, PAAgentNamedObject& paAgent, EBroadcastOrigin origin /*= BO_UNKNOWN*/ )
{
	CWaitCursor hourGlassCursor;

	int numberOfNonRemovals = 0;

	// Progressively go through and kill each specified broadcast id
	for (int i = 0; i < broadcastIds.size(); i ++)
	{
		try
		{             
			bool ignoreRightsChecking = false;

			// TD #3152
			// There is a subset of broadcast types that can be cleared without
			// regard for operator priveleges (because they were created locally, and
			// are effectively internal to the system - transparent to user) - don't
			// want such broadcasts to pop up errors saying insufficient priveleges
			switch (origin)
			{
			case BO_RECORDING_BROADCAST:    // FALLTHROUGH
			case BO_LIVE_BROADCAST:         // FALLTHROUGH
			case BO_PLAYBACK_BROADCAST: 
				ignoreRightsChecking = true;
				break;
			};

			// Technically if we don't have a right to clear a broadcast, we are
			// still allowed to terminate it (just not remove it) - but it makes
			// sense for this function to do neither if that privelege is not present

			// TES #703: Cannot terminate broadcasts that are currently in progress
			bool allowedToClearBroadcast = PaRightsManager::getInstance().hasRightToClearBroadcast(
				broadcastIds[i].c_str(), paAgent) &&
				!PaRightsManager::getInstance().isBroadcastInProgress(
				broadcastIds[i].c_str(), paAgent);

			if (ignoreRightsChecking || allowedToClearBroadcast)                    
			{
				CORBA_CALL( paAgent, terminateBroadcast, ( broadcastIds[i].c_str(), CachedConfig::getInstance()->getSessionId().c_str() ) );
				CORBA_CALL( paAgent, removeBroadcast, ( broadcastIds[i].c_str(), CachedConfig::getInstance()->getSessionId().c_str() ) );
			}
			else
			{
				PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
			}
        }
        catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
            switch ( expWhat.eErrorId )
            {
            case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastIdNotFound:
                // This broadcast ID already terminated, ignore exception & continue
                // (this is not an error)
                break;
            case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
                // The removeBroadcast failed, which indicates terminateBroadcast
                // didn't work correctly (perhaps another user initiated
                // the broadcast again) - we just don't remove it from the list

                // There is a known issue where broadcast does not complete immediately
                // so it's quite likely for this error to occur (at this point in time)
            case TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied:
                // No privilege to remove this one
                numberOfNonRemovals ++;
                break;
            default:
                break;
            }
        }
	}

	return numberOfNonRemovals;

}
