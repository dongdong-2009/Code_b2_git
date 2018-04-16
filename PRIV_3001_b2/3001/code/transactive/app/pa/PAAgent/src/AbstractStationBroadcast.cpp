/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/AbstractStationBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2018/03/14 13:51:20 $
  * Last modified by : $Author: lim.cy $
  *
  *
  */

#include "core/utilities/src/DebugUtil.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"

namespace TA_IRS_App
{
    // 0 is used by PAS to present free transaction id
	const unsigned short MIN_STATION_ANNOUNCEMENT_ID = 1;
    //4095 is used by PAS to present non-local transaction in progress
	const unsigned short MAX_STATION_ANNOUNCEMENT_ID = 4094;

	ACE_RW_Mutex AbstractStationBroadcast::s_mtxUsedAnnouncementId;
	std::set<CORBA::UShort> AbstractStationBroadcast::s_setUsedAnnouncementIds;

    // Thread checked
    AbstractStationBroadcast::AbstractStationBroadcast( 
        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
    AbstractBroadcast( broadcastConfigP, sessionId ),
      m_announceId( PAS_FREE_TRANSACTION ),
      m_nTransactionResult( TRANSACTION_NOT_EXECUTED )
    {
        FUNCTION_ENTRY( "AbstractStationBroadcast" );

        switch( broadcastConfigP->broadcastType )
        {
            case TA_Base_Bus::IPAAgentCorbaDef::StationMusic:
                {
                    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage nullZones;
                    nullZones.length(0);
                    m_progressReportSentry = new ProgressReportSentry(broadcastConfigP->broadcastId, nullZones);
                }
                break;
            case TA_Base_Bus::IPAAgentCorbaDef::StationLive:
                m_progressReportSentry = new ProgressReportSentry( broadcastConfigP->broadcastId,
                                                                   broadcastConfigP->theBroadcastConfigSpecifics.theStationLiveConfig().zones);
                break;
            case TA_Base_Bus::IPAAgentCorbaDef::StationDva:
                m_progressReportSentry = new ProgressReportSentry( broadcastConfigP->broadcastId,
                                                                   broadcastConfigP->theBroadcastConfigSpecifics.theStationDvaConfig().zones);
                break;
            case TA_Base_Bus::IPAAgentCorbaDef::StationRecording:
                {
                    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage recordingZone;
                    recordingZone.length(1);
                    recordingZone[0] = 0u;
                    m_progressReportSentry = new ProgressReportSentry(broadcastConfigP->broadcastId, recordingZone);
                }
                break;
            case TA_Base_Bus::IPAAgentCorbaDef::TrainLive:
            case TA_Base_Bus::IPAAgentCorbaDef::TrainDva:
            default:
                TA_THROW(TA_Base_Core::CorruptInternalStateDataException("Unexpected broadcastType"));

        } // switch

        FUNCTION_EXIT;
    }


    // Thread checked
    AbstractStationBroadcast::~AbstractStationBroadcast()
    {
        FUNCTION_ENTRY( "~AbstractStationBroadcast" );

        if (m_progressReportSentry)
        {
            delete m_progressReportSentry;
            m_progressReportSentry = NULL;
        }

        FUNCTION_EXIT;
    }


    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices AbstractStationBroadcast::calculateEffectiveCoverageIndices( const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& allZones, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& newZones )
        //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
    {
        FUNCTION_ENTRY( "calculateEffectiveCoverageIndices" );

        TA_THREADGUARD( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices indices;
        indices.length( newZones.length() );

        // Locate matching zones
        bool bZoneMatched = false;
        for (unsigned int index=0; index<newZones.length(); ++index)
        {
            bZoneMatched = false;
            for (unsigned int j=0;j<allZones.length(); ++j)
            {
                if (newZones[index] == allZones[j])
                {
                    // A matched zone
                    bZoneMatched = true;
                    indices[index] = j;
                    break; // j loop
                }
            } // for j

            if ( !bZoneMatched )
            {
                // log it and throw the exception
                // the calling method should catch this and log an audit message in one point
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Station broadcast failed - The specified zone is not configured for this broadcast" );

                FUNCTION_EXIT;
                PasHelpers::getInstance()->throwPaErrorException( 
                    TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified zone is not configured for this broadcast" );
            }
        } // for index
        FUNCTION_EXIT;
        return indices;
    }


    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage AbstractStationBroadcast::getEffectiveZoneCoverage( const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& allZones )
    {
        FUNCTION_ENTRY( "getEffectiveZoneCoverage" );

        TA_THREADGUARD( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage effectiveZoneCoverage;
        effectiveZoneCoverage.length( m_effectiveCoverageIndices.length() );

        for (unsigned int index=0; index<m_effectiveCoverageIndices.length(); ++index)
        {
            effectiveZoneCoverage[index] = allZones[m_effectiveCoverageIndices[index]];
        }

        FUNCTION_EXIT;
        return effectiveZoneCoverage;
    }


    void AbstractStationBroadcast::retry( const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const std::string& sessionId )
        //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException)
    {
        FUNCTION_ENTRY( "retry" );

        if (getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval)
        {
            // log it and throw the exception
            // the calling method should catch this and log an audit message in one point
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "Station broadcast retry failed - Broadcast must be terminated first" );

            FUNCTION_EXIT;
            PasHelpers::getInstance()->throwPaErrorException( 
                TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "Broadcast must be terminated first" );
        }

        TA_THREADGUARD( m_lock );

        TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices coverageIndices =
            calculateEffectiveCoverageIndices(getZoneCoverage(), zones);

        std::vector<unsigned long> vecNewKeys;
        PasHelpers::getInstance()->coverageToZoneKeys( zones, vecNewKeys );
        updateEffectiveZones( vecNewKeys );

        // Reset selected zones
        for (unsigned int index=0; index<coverageIndices.length(); ++index)
        {
            m_progressReportSentry->resetZoneOrTrain(coverageIndices[index]);
        }

        // Update the list of effective zones to broadcast with.  This time only contains
        // the ones specified
        setEffectiveCoverageIndices(coverageIndices);

        // Reset sessionId
        setSessionId(sessionId);

        // Revalidate broadcast in case parameters are no longer valid
        validateBroadcastConfig();

        // Reset broadcast
        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Execution);
        m_progressReportSentry->resetGlobalState();

        FUNCTION_EXIT;
    }

    void AbstractStationBroadcast::updateEffectiveZones( const std::vector<unsigned long>& vecNewZoneKeys )
    {
    }

	//added by lixiaoxia
	CORBA::UShort AbstractStationBroadcast::getNextAvailableStationAnnouncementId()
		//throw(TA_Base_Core::NoFreeAnnounceIdException)
	{
		TA_Base_Core::ThreadGuard guard( m_lock );

		static CORBA::UShort lastAnnouncementId = MAX_STATION_ANNOUNCEMENT_ID;

		for (int count=0; count <= MAX_STATION_ANNOUNCEMENT_ID; ++count)
		{
			if (lastAnnouncementId == MAX_STATION_ANNOUNCEMENT_ID)
			{
				// loop around - cannot use an ID=0 < MIN_ANNOUNCE_ID as this is 
				// reserved to indicate a train broadcast that hasn't been executed.
				lastAnnouncementId = MIN_STATION_ANNOUNCEMENT_ID;
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

	void AbstractStationBroadcast::releaseUsingAnnouncementId( const CORBA::UShort cUsingId )
	{
		ACE_Write_Guard<ACE_RW_Mutex> guardLock( s_mtxUsedAnnouncementId );

		std::set<CORBA::UShort>::iterator itFound = s_setUsedAnnouncementIds.find( cUsingId );
		if ( s_setUsedAnnouncementIds.end() == itFound )
		{
			return;
		}

		s_setUsedAnnouncementIds.erase( itFound );
	}

	void AbstractStationBroadcast::occupyUsingAnnouncementId( const CORBA::UShort announcementId )
    {
        ACE_Write_Guard<ACE_RW_Mutex> guardLock( s_mtxUsedAnnouncementId );

        std::set<CORBA::UShort>::iterator itFound = s_setUsedAnnouncementIds.find( announcementId );
        if ( s_setUsedAnnouncementIds.end() != itFound )
        {
            return;
        }

        s_setUsedAnnouncementIds.insert( announcementId );
	}

	CORBA::UShort AbstractStationBroadcast::getAnnouncementId()
	{
		FUNCTION_ENTRY( "getAnnouncementId" );

		TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters );

		FUNCTION_EXIT;
		return m_announceId;
	}

	void AbstractStationBroadcast::setAnnouncementId( CORBA::UShort announcementId, bool sendSynchronisation )
	{
		FUNCTION_ENTRY( "setAnnouncementId" );

        TA_Base_Core::ThreadGuard guard( m_lockForBroadcastParameters);

        if ( m_announceId == announcementId )
        {
            return;
        }

		if( m_announceId != PAS_FREE_TRANSACTION )
		{
			releaseUsingAnnouncementId( m_announceId );
		}

		if( announcementId != PAS_FREE_TRANSACTION )
		{
			occupyUsingAnnouncementId( announcementId );
        }

        m_announceId = announcementId;

        if (sendSynchronisation)
        {
            CORBA::Any data;
            data <<= announcementId;

            PasHelpers::getInstance()->sendPAAgentBroadcastParameterUpdate
                (TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_STATION_ANNOUNCE_ID, data, getBroadcastId().c_str());
        }

		FUNCTION_EXIT;
	}

	TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* AbstractStationBroadcast::getBroadcastParametersCopy()
	{
		FUNCTION_ENTRY( "getBroadcastParametersCopy" );

		TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq* parameters =
			AbstractBroadcast::getBroadcastParametersCopy();

		int originalLength = parameters->length();
		parameters->length(originalLength+1);

		CORBA::Any data1;
		data1 <<= getAnnouncementId();
		(*parameters)[originalLength].broadcastId = getBroadcastId().c_str();
		(*parameters)[originalLength].parameter   = TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_STATION_ANNOUNCE_ID;
		(*parameters)[originalLength].data        = data1;

		FUNCTION_EXIT;
		return parameters;
	}

	void AbstractStationBroadcast::setBroadcastParameter(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& parameter )
	{
		FUNCTION_ENTRY( "setBroadcastParameter" );

		if (parameter.parameter == TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_PARAMETER_STATION_ANNOUNCE_ID)
		{
			CORBA::UShort announcementId = 0u;

			if ( parameter.data >>= announcementId )
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

