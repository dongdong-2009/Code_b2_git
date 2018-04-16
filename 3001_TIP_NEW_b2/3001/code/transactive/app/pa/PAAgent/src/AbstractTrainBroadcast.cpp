/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/AbstractTrainBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #5 $
  * Last modification : $DateTime: 2012/11/29 22:02:45 $
  * Last modified by : $Author: qi.huang $
  *
  *
  */

#include "core/utilities/src/DebugUtil.h"

#pragma warning ( disable : 4250 )
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#pragma warning ( default : 4250 )

#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"


using namespace TA_Base_Core;

namespace TA_IRS_App
{
    const unsigned char MIN_TRAIN_ANNOUNCEMENT_ID = 1;
    const unsigned char MAX_TRAIN_ANNOUNCEMENT_ID = 255;

    ACE_RW_Mutex AbstractTrainBroadcast::s_mtxUsedAnnouncementId;
    std::set<CORBA::Octet> AbstractTrainBroadcast::s_setUsedAnnouncementIds;

    // Thread checked
    AbstractTrainBroadcast::AbstractTrainBroadcast(TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                                   const std::string& sessionId)
    : AbstractBroadcast( broadcastConfigP, sessionId ),
      m_terminateTimeLimit( 0 ),
      m_trainPaCorbaProxy(),
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
        //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
    {
        FUNCTION_ENTRY( "calculateEffectiveCoverageIndices" );

        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices indices;
        indices.length( newTrains.length() );

        // Locate matching trains
        unsigned int index=0;
        for (; index<newTrains.length(); ++index)
        {
            unsigned int j=0;
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
                PasHelpers::getInstance()->throwPaErrorException( 
                    TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified train is not configured for this broadcast" );
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


        for (unsigned int index=0; index<m_effectiveCoverageIndices.length(); ++index)
        {
            effectiveTrainCoverage[index] = allTrains[m_effectiveCoverageIndices[index]];
        }

        FUNCTION_EXIT;
        return effectiveTrainCoverage;
    }


    void AbstractTrainBroadcast::retry( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
                                        bool hasOverrideOption,
                                        const std::string& sessionId )
        //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
    {
        FUNCTION_ENTRY( "retry" );

        if (getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval)
        {
            // log it and throw the exception
            // the calling method should catch this and log an audit message in one point
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Train broadcast retry failed - Broadcast must be terminated first" );

            FUNCTION_EXIT;
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "Broadcast must be terminated first" );
        }

        ThreadGuard guard( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices coverageIndices =
            calculateEffectiveCoverageIndices(getTrainCoverage(), trains);

        // Reset selected zones
        for (unsigned int index=0; index<coverageIndices.length(); ++index)
        {
            m_progressReportSentry->resetZoneOrTrain( coverageIndices[index],
                TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_CONNECTING );
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

        m_terminateTimeLimit = 0;

        // Reset broadcast
        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Execution);
        m_progressReportSentry->resetGlobalState();

        FUNCTION_EXIT;
    }


    CORBA::Octet AbstractTrainBroadcast::getNextAvailableTrainAnnouncementId()
        //throw(TA_Base_Core::NoFreeAnnounceIdException)
    {
        TA_Base_Core::ThreadGuard guard( m_lock );

        static CORBA::Octet lastAnnouncementId = MAX_TRAIN_ANNOUNCEMENT_ID;

        for (int count=0; count <= MAX_TRAIN_ANNOUNCEMENT_ID; ++count)
        {
            if (lastAnnouncementId == MAX_TRAIN_ANNOUNCEMENT_ID)
            {
                // loop around - cannot use an ID=0 < MIN_ANNOUNCE_ID as this is 
                // reserved to indicate a train broadcast that hasn't been executed.
                lastAnnouncementId = MIN_TRAIN_ANNOUNCEMENT_ID;
            }
            else
            {
                ++lastAnnouncementId;
            }

            {
                ACE_Read_Guard<ACE_RW_Mutex> guardLock( s_mtxUsedAnnouncementId );
                // Now check if this is occupied by an existing broadcast
                if ( s_setUsedAnnouncementIds.end() == s_setUsedAnnouncementIds.find( lastAnnouncementId ))
                {
                    // done - found an unused announceId for the trainId
                    return lastAnnouncementId;
                }
            }
        } // for loop

        TA_THROW(TA_Base_Core::NoFreeAnnounceIdException());

        return 0;
    }


    void AbstractTrainBroadcast::releaseUsingAnnouncementId( const CORBA::Octet cUsingId )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guardLock( s_mtxUsedAnnouncementId );

        std::set<CORBA::Octet>::iterator itFound = s_setUsedAnnouncementIds.find( cUsingId );
        if ( s_setUsedAnnouncementIds.end() == itFound )
        {
            return;
        }

        s_setUsedAnnouncementIds.erase( itFound );
    }


    void AbstractTrainBroadcast::occupyUsingAnnouncementId( const CORBA::Octet cUsingId )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guardLock( s_mtxUsedAnnouncementId );

        std::set<CORBA::Octet>::iterator itFound = s_setUsedAnnouncementIds.find( cUsingId );
        if ( s_setUsedAnnouncementIds.end() != itFound )
        {
            return;
        }

        s_setUsedAnnouncementIds.insert( cUsingId );
    }


    CORBA::Octet AbstractTrainBroadcast::getAnnouncementId()
    {
        FUNCTION_ENTRY( "getAnnouncementId" );

        TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

        FUNCTION_EXIT;
        return m_trainAnnouncementId;
    }


    void AbstractTrainBroadcast::setAnnouncementId(CORBA::Octet announcementId, bool sendSynchronisation)
    {
        FUNCTION_ENTRY( "setAnnouncementId" );

        TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters);

        if ( m_trainAnnouncementId == announcementId )
        {
            return;
        }

        if ( 0 != m_trainAnnouncementId )
        {
            releaseUsingAnnouncementId( m_trainAnnouncementId );
        }

        if ( 0 != announcementId )
        {
            occupyUsingAnnouncementId( announcementId );
        }

        m_trainAnnouncementId = announcementId;

        if (sendSynchronisation)
        {
            CORBA::Any data;
            data <<= CORBA::Any::from_octet(announcementId);

            PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
                (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_TRAIN_ANNOUNCEMENT_ID, data, getBroadcastId().c_str());
        }

        FUNCTION_EXIT;
    }


    TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* AbstractTrainBroadcast::getBroadcastParametersCopy()
    {
        FUNCTION_ENTRY( "getBroadcastParametersCopy" );

        TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* parameters =
            AbstractBroadcast::getBroadcastParametersCopy();

        int originalLength = parameters->length();
        parameters->length(originalLength+1);

        CORBA::Any data1;
        data1 <<= CORBA::Any::from_octet(getAnnouncementId());
        (*parameters)[originalLength].broadcastId = getBroadcastId().c_str();
        (*parameters)[originalLength].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_TRAIN_ANNOUNCEMENT_ID;
        (*parameters)[originalLength].data        = data1;

        FUNCTION_EXIT;
        return parameters;
    }


    void AbstractTrainBroadcast::setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter )
    {
        FUNCTION_ENTRY( "setBroadcastParameter" );

        if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_TRAIN_ANNOUNCEMENT_ID)
        {
            CORBA::Octet announcementId = 0;

            if ( 0 != (parameter.data >>= CORBA::Any::to_octet(announcementId) ))
            {
                setAnnouncementId(announcementId, false);
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                    "BroadcastParameter update received: announcementId=%d", announcementId );
            }
        }
        else
        {
            AbstractBroadcast::setBroadcastParameter(parameter);
        }

        FUNCTION_EXIT;
    }

} // namespace TA_IRS_App

