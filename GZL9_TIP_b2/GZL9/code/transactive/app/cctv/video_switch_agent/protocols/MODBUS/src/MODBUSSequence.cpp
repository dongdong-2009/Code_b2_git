/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSequence.cpp $
  * @author:   Bruce Polmear
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/08/22 11:34:16 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the general functions for a Sequence
  * using the MODBUS protocol.
  * Inherits from the IPDSequence class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Sequence and
  * that the Sequence and the protocol support the required features.
  */

#include <sstream>
#include <climits>

#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSequence.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSHelper.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/SetSequenceConfigCommand.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/ControlSequencePlaybackCommand.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/SequenceCorbaDef.h"

#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    //
    // MODBUSSequence
    //
    MODBUSSequence::MODBUSSequence( TA_Base_Core::SequencePtr sequenceData )
    : m_sequenceData( sequenceData ), m_dwellTime( 0 ), m_videoInputs()
    {
        MODBUSSwitchManager::getInstance().addSequence( *this );
        //MODBUSSwitchManager::getInstance().getStateValidationCycle().addValidationItem( *this, false );
        TA_IRS_Core::SequenceHelper sequenceHelper;
        std::string t;
		sequenceHelper.getSequenceRelationShip(sequenceData->getKey() , m_dwellTime, t, m_videoInputs);
    }


    //
    // Destructor
    //
    MODBUSSequence::~MODBUSSequence()
    {
    }


    //
    // notifySequenceConfigUpdated
    //
    void  MODBUSSequence::notifySequenceConfigUpdated( unsigned short dwellTime, const std::vector< unsigned long >& videoInputs )
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
    void MODBUSSequence::updateState( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState& state )
    {
        // Do nothing as protocol does not support mainting Sequence Playback State.
    }


    //
    // updateSequenceConfig
    //
    void MODBUSSequence::updateSequenceConfig( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& sequenceConfig )
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
    void MODBUSSequence::getDwellTimeRange( unsigned short& minimumDwellTime,
                                           unsigned short& maximumDwellTime )
    {
        minimumDwellTime = 0;
        maximumDwellTime = UCHAR_MAX;
    }


    //
    // getMaxNumberOfInputs
    //
    unsigned short MODBUSSequence::getMaxNumberOfInputs()
    {
        return USHRT_MAX;
    }


    //
    // setSequenceConfig
    //
    void MODBUSSequence::setSequenceConfig( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& newConfig,bool con )
    {
		try
		{
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
    TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig* MODBUSSequence::getSequenceConfig()
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
    void MODBUSSequence::cycleToNextVideoInput( unsigned long videoOutputEntityKey )
    {
    }


    //
    // cycleToPreviousVideoInput
    //
    void MODBUSSequence::cycleToPreviousVideoInput( unsigned long videoOutputEntityKey )
    {
    }


    //
    // pause
    //
    void MODBUSSequence::pause( unsigned long videoOutputEntityKey )
    {
    }

    
    //
    // play
    //
    void MODBUSSequence::play( unsigned long videoOutputEntityKey )
    {
    }


    //
    // stop
    //
    void MODBUSSequence::stop( unsigned long videoOutputEntityKey )
    {
    }


    //
    // getSequenceState
    //
    TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState MODBUSSequence::getSequenceState( unsigned long videoOutputEntityKey )
    {
		TA_ASSERT(false, "Proctocal not support");
		return TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState();
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // MODBUSSequence Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // getKey
    //
    unsigned long MODBUSSequence::getKey() const
    {
        return m_sequenceData->getKey();
    }


    //
    // getKey
    //
    std::string MODBUSSequence::getAddress()
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
    void MODBUSSequence::validateState()
    {
    }

	bool MODBUSSequence::isReadOnly()
	{
		return m_sequenceData->isReadOnly();
	}

} // namespace TA_IRS_App

