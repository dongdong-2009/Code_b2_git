#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/AbstractTrainBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2012/06/12 13:35:44 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"
#include "app/pa/PAAgent/src/CachedConfig.h"

#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"


using namespace TA_Base_Core;

namespace TA_IRS_App
{

    // Thread checked
    AbstractTrainBroadcast::AbstractTrainBroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                                   const std::string& sessionId)
    : AbstractBroadcast( broadcastConfigP, sessionId ),
      m_trainAnnouncementId(0)
    {
        FUNCTION_ENTRY( "AbstractTrainBroadcast" );

        m_trainPaCorbaProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainPaCorbaProxy();

        switch( broadcastConfigP->broadcastType )
        {

            case TA_Base_Bus::IPAAgentCorbaDef::TrainLive:
                m_progressReportSentry = new ProgressReportSentry( broadcastConfigP->broadcastId,
                                                                   broadcastConfigP->theBroadcastConfigSpecifics.theTrainLiveConfig().trains);
                break;
            case TA_Base_Bus::IPAAgentCorbaDef::TrainDva:
                m_progressReportSentry = new ProgressReportSentry( broadcastConfigP->broadcastId,
                                                                   broadcastConfigP->theBroadcastConfigSpecifics.theTrainDvaConfig().trains);
                break;
            case TA_Base_Bus::IPAAgentCorbaDef::StationMusic:
            case TA_Base_Bus::IPAAgentCorbaDef::StationLive:
            case TA_Base_Bus::IPAAgentCorbaDef::StationDva:
            case TA_Base_Bus::IPAAgentCorbaDef::StationRecording:
            default:
                TA_THROW(CorruptInternalStateDataException("Unexpected broadcastType"));
        } // switch

        FUNCTION_EXIT;
    }


    // Thread checked
    AbstractTrainBroadcast::~AbstractTrainBroadcast()
    {
        FUNCTION_ENTRY( "~AbstractTrainBroadcast" );

        if (m_progressReportSentry)
        {
            delete m_progressReportSentry;
            m_progressReportSentry = NULL;
        }

        FUNCTION_EXIT;
    }


    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices AbstractTrainBroadcast::calculateEffectiveCoverageIndices(
        const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& allTrains,
        const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& newTrains )
        //throw(TA_Base_Core::BadParameterException)
    {
        FUNCTION_ENTRY( "calculateEffectiveCoverageIndices" );

        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices indices;
        indices.length( newTrains.length() );

        // Locate matching trains
        int index=0;
        for (; index<newTrains.length(); ++index)
        {
            int j=0;
            for (;j<allTrains.length(); ++j)
            {
                if (newTrains[index] == allTrains[j])
                {
                    // A matched train
                    indices[index] = j;
                    break; // j loop
                }
            } // for j

            if (j == allTrains.length())
            {
                // log it and throw the exception
                // the calling method should catch this and log an audit message in one point
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Station broadcast failed - The specified train is not configured for this broadcast" );

                FUNCTION_EXIT;
                throw BadParameterException("The specified train is not configured for this broadcast");
            }
        } // for index
        FUNCTION_EXIT;
        return indices;
    }


    TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage AbstractTrainBroadcast::getEffectiveTrainCoverage( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& allTrains )
    {
        FUNCTION_ENTRY( "getEffectiveTrainCoverage" );

        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage effectiveTrainCoverage;
        effectiveTrainCoverage.length( m_effectiveCoverageIndices.length() );


        for (int index=0; index<m_effectiveCoverageIndices.length(); ++index)
        {
            effectiveTrainCoverage[index] = allTrains[m_effectiveCoverageIndices[index]];
        }

        FUNCTION_EXIT;
        return effectiveTrainCoverage;
    }


    void AbstractTrainBroadcast::retry( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
                                        bool hasOverrideOption,
                                        const std::string& sessionId )
        //throw(TA_Base_Core::BadParameterException)
    {
        FUNCTION_ENTRY( "retry" );

        if (getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval)
        {
            // log it and throw the exception
            // the calling method should catch this and log an audit message in one point
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Train broadcast retry failed - Broadcast must be terminated first" );

            FUNCTION_EXIT;
            throw BadParameterException("Broadcast must be terminated first");
        }

        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices coverageIndices =
            calculateEffectiveCoverageIndices(getTrainCoverage(), trains);

        // Reset selected zones
        for (int index=0; index<coverageIndices.length(); ++index)
        {
            m_progressReportSentry->resetZoneOrTrain(coverageIndices[index]);
        }

        // Update the list of effective trains to broadcast with.  This time only contains
        // the ones specified
        setEffectiveCoverageIndices(coverageIndices);

        // Reset hasOverrideOption
        setHasOverrideOption(hasOverrideOption);

        // Reset sessionId
        setSessionId(sessionId);

        // Revalidate broadcast in case parameters are no longer valid
        validateBroadcastConfig();

        // Reset broadcast
        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Execution);
        m_progressReportSentry->resetGlobalState();

        FUNCTION_EXIT;
    }


    // Thread checked in parent
    void AbstractTrainBroadcast::updateFinalProgressReport()
    {
        FUNCTION_ENTRY( "updateFinalProgressReport" );

        AbstractBroadcast::updateFinalProgressReport();

        // go through each zone and get the ones with failed states
        // all zones that did not fail will be set to completed int the parent class
        TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage failedTrains;
        int failedIndex = 0;
        for (int indexIndex = 0; indexIndex< m_effectiveCoverageIndices.length(); ++indexIndex)
        {
            if ( m_progressReportSentry->getLocalState( m_effectiveCoverageIndices[indexIndex] ) !=
                 TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_COMPLETED )
            {
                // this is a failed zone
                failedTrains.length(failedTrains.length() + 1);
                failedTrains[failedIndex] = m_effectiveCoverageIndices[indexIndex];
                failedIndex++;
            }
        }

        PasHelpers::getInstance()->submitPaTrainBroadcastResultAuditMessage(
            getBroadcastId(),
            failedTrains,
            (failedTrains.length() > 0),
            getSessionId() );

        FUNCTION_EXIT;
    }


} // namespace TA_IRS_App

