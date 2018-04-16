/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSequence.cpp $
  * @author:   Bruce Polmear
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/08/22 11:34:16 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the general functions for a Sequence
  * using the IMOS protocol.
  * Inherits from the IPDSequence class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Sequence and
  * that the Sequence and the protocol support the required features.
  */

#include <sstream>
#include <climits>

#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSequence.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSHelper.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/SetSequenceConfigCommand.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/ControlSequencePlaybackCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/SequenceCorbaDef.h"

#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#if defined(sparc)
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/imos_public.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/imos_errcode.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/imos_def.h"
#elif defined(WIN32)
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/imos_public.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/imos_errcode.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/imos_def.h"
#endif

namespace TA_IRS_App
{
    namespace
    {
        static const int MAX_SOURCES_PER_SEQUENCE = IMOS_SWITCH_CAM_MAXNUM;
    }

    //
    // IMOSSequence
    //
    IMOSSequence::IMOSSequence( TA_Base_Core::SequencePtr sequenceData )
    : m_sequenceData( sequenceData ), m_dwellTime( 0 ), m_videoInputs()
    {
        IMOSSwitchManager::getInstance().addSequence( *this );
        //IMOSSwitchManager::getInstance().getStateValidationCycle().addValidationItem( *this, false );
    }


    //
    // Destructor
    //
    IMOSSequence::~IMOSSequence()
    {
    }


    //
    // notifySequenceConfigUpdated
    //
    void  IMOSSequence::notifySequenceConfigUpdated( unsigned short dwellTime, const std::vector< unsigned long >& videoInputs )
    {
		try
		{
			StateUpdateBroadcaster::getInstance().notifySequenceConfigUpdate( m_sequenceData->getKey() );
		}
		catch( const TA_Base_Core::DatabaseException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
				"Failed to notify the agent's clients that a change in sequence config has occurred." );
		}
		catch( const TA_Base_Core::DataException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
				"Failed to notify the agent's clients that a change in sequence config has occurred." );
		}
    }


    //
    // updateState
    //
    void IMOSSequence::updateState( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState& state )
    {
        // Do nothing as protocol does not support mainting Sequence Playback State.
    }


    //
    // updateSequenceConfig
    //
    void IMOSSequence::updateSequenceConfig( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& sequenceConfig )
    {
		m_dwellTime = sequenceConfig.dwellTime;
		const TA_Base_Bus_GZL9::SequenceCorbaDef::VideoInputKeySequence& sequence = sequenceConfig.inputKeySequence;
		m_videoInputs.clear();
		m_videoInputs.reserve(sequence.length());
		for(_CORBA_ULong i = 0; i < sequence.length(); ++i)
		{
			m_videoInputs.push_back(sequence[i]);
		}
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IPDSequence Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getDwellTimeRange
    //
    void IMOSSequence::getDwellTimeRange( unsigned short& minimumDwellTime,
                                           unsigned short& maximumDwellTime )
    {
        minimumDwellTime = 0;
        maximumDwellTime = UCHAR_MAX;
    }


    //
    // getMaxNumberOfInputs
    //
    unsigned short IMOSSequence::getMaxNumberOfInputs()
    {
        return USHRT_MAX;
    }


	SWITCH_BASE_S IMOSSequence::queryImosSeqBaseInfo()
	{
		static bool alreadyRun = false;
		static SWITCH_BASE_S bs;
		if(!alreadyRun)
		{
			SWITCH_RESOURE_S s;
			IMOSSwitchManager::getInstance().getCommunicationsHandler()->querySequenceInfo(m_sequenceData->getAddress(), &s);
			alreadyRun = true;
			bs = s.stSwitchBaseInfo;
		}
		return bs;
	}
    //
    // setSequenceConfig
    //
    void IMOSSequence::setSequenceConfig( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& newConfig,bool con )
    {
        try
        {     
			ULONG seqLen = newConfig.inputKeySequence.length();
            if (seqLen > MAX_SOURCES_PER_SEQUENCE)
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Client tried to set a sequence that is too long %d sources", seqLen);
                std::ostringstream msg;
                msg << "A sequence can not contain more than " << MAX_SOURCES_PER_SEQUENCE << " items";
                throw TA_Base_Bus_GZL9::VideoSwitchAgentException( msg.str().c_str() );
            }

			SWITCH_UNIT_S us[IMOS_SWITCH_CAM_MAXNUM];
			for (ULONG i = 0; i < seqLen; ++i)
			{
				IMOSCamera* cam = IMOSSwitchManager::getInstance().getCamera(newConfig.inputKeySequence[i]);
				if (0 == cam)
				{
					std::stringstream ss;
					ss << "Not find coecamera object " << newConfig.inputKeySequence[i];
					throw TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
						ss.str());
				}
				strncpy(us[i].szCameraCode, cam->getAddress().c_str(), IMOS_RES_CODE_LEN);
				strncpy(us[i].szCameraName, cam->getName().c_str(), IMOS_NAME_LEN);
				us[i].ulSequence = i;
				us[i].ulInterval = newConfig.dwellTime;
			}

			SWITCH_RESOURE_S s;
			s.stSwitchBaseInfo = queryImosSeqBaseInfo();
			s.ulSwitchResNum = seqLen;
			memcpy(s.astSwitchUnitList, us, IMOS_SWITCH_CAM_MAXNUM * sizeof(SWITCH_UNIT_S));
			IMOSSwitchManager::getInstance().getCommunicationsHandler()->modifySequenceConfig(m_sequenceData->getAddress(), &s);
			// update local & broadcast
			updateSequenceConfig(newConfig);
			notifySequenceConfigUpdated(m_dwellTime, m_videoInputs);
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Set Sequence Config command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Set Sequence Config command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }
		catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", e.what.in() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentException", "Failed to send Set Sequence Config command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what.in() );
        }  
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Set Sequence Config command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }         
    }


    //
    // getSequenceConfig
    //
    TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig* IMOSSequence::getSequenceConfig()
    {
        TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig* pSequenceConfig = new TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig();
        
        pSequenceConfig->dwellTime = m_dwellTime;
        pSequenceConfig->inputKeySequence.length( m_videoInputs.size() );
        std::vector< unsigned long >::size_type iVideoInputNumber( 0 );
        for ( ; iVideoInputNumber < m_videoInputs.size(); iVideoInputNumber++ )
        {
            pSequenceConfig->inputKeySequence[ iVideoInputNumber ] = m_videoInputs[ iVideoInputNumber ];
        }

        return pSequenceConfig;
    }


    //
    // cycleToNextVideoInput
    //
    void IMOSSequence::cycleToNextVideoInput( unsigned long videoOutputEntityKey )
    {
    }


    //
    // cycleToPreviousVideoInput
    //
    void IMOSSequence::cycleToPreviousVideoInput( unsigned long videoOutputEntityKey )
    {
    }


    //
    // pause
    //
    void IMOSSequence::pause( unsigned long videoOutputEntityKey )
    {
    }

    
    //
    // play
    //
    void IMOSSequence::play( unsigned long videoOutputEntityKey )
    {
    }


    //
    // stop
    //
    void IMOSSequence::stop( unsigned long videoOutputEntityKey )
    {
    }


    //
    // getSequenceState
    //
    TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState IMOSSequence::getSequenceState( unsigned long videoOutputEntityKey )
    {
		TA_ASSERT(false, "Proctocal not support");
		return TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState();
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // IMOSSequence Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // getKey
    //
    unsigned long IMOSSequence::getKey() const
    {
        return m_sequenceData->getKey();
    }


    //
    // getKey
    //
    std::string IMOSSequence::getAddress()
    {
        return m_sequenceData->getAddress();
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IStateValidationItem Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // validateState
    //
    void IMOSSequence::validateState()
    {
    }

} // namespace TA_IRS_App

