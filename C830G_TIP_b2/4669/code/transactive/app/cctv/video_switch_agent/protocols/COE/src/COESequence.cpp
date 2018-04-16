/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/COESequence.cpp $
  * @author:   Bruce Polmear
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the general functions for a Sequence
  * using the COE protocol.
  * Inherits from the IPDSequence class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Sequence and
  * that the Sequence and the protocol support the required features.
  */

#include <sstream>
#include <climits>

#include "app/cctv/video_switch_agent/protocols/COE/src/COESequence.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SetSequenceConfigCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/ControlSequencePlaybackCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"

#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    namespace
    {
        static const int MAX_SOURCES_PER_SEQUENCE = 64;
    }

    //
    // COESequence
    //
    COESequence::COESequence( TA_Base_Core::SequencePtr sequenceData )
    : m_sequenceData( sequenceData ), m_dwellTime( 0 ), m_videoInputs()
    {
        COESwitchManager::getInstance().addSequence( *this );
        COESwitchManager::getInstance().getStateValidationCycle().addValidationItem( *this, false );
    }


    //
    // Destructor
    //
    COESequence::~COESequence()
    {
    }


    //
    // notifySequenceConfigUpdated
    //
    void  COESequence::notifySequenceConfigUpdated( unsigned short dwellTime, const std::vector< unsigned long >& videoInputs )
    {
		if(m_dwellTime!=dwellTime || m_videoInputs!=videoInputs)
		{
			m_dwellTime = dwellTime;
			m_videoInputs.clear();
			m_videoInputs.insert( m_videoInputs.end(), videoInputs.begin(), videoInputs.end() );

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
    }


    //
    // updateState
    //
    void COESequence::updateState( const TA_Base_Bus::SequenceCorbaDef::SequenceState& state )
    {
        // Do nothing as protocol does not support mainting Sequence Playback State.
    }


    //
    // updateSequenceConfig
    //
    void COESequence::updateSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig )
    {
        // TODO
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IPDSequence Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getDwellTimeRange
    //
    void COESequence::getDwellTimeRange( unsigned short& minimumDwellTime,
                                           unsigned short& maximumDwellTime )
    {
        minimumDwellTime = 0;
        maximumDwellTime = UCHAR_MAX;
    }


    //
    // getMaxNumberOfInputs
    //
    unsigned short COESequence::getMaxNumberOfInputs()
    {
        return USHRT_MAX;
    }


    //
    // setSequenceConfig
    //
    void COESequence::setSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& newConfig )
    {
        try
        {     
            std::vector< unsigned long > videoInputs( static_cast< std::vector< unsigned long >::size_type >( newConfig.inputKeySequence.length() ) );
            std::vector< unsigned long >::size_type iVideoInputNumber( 0 );

            // TES 557/TD 3192
            // check that there are no more than 64 cameras in this sequence
            // it is checked in the GUI but check here as it is a protocol limitation
            if (videoInputs.size() > MAX_SOURCES_PER_SEQUENCE)
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Client tried to set a sequence that is too long %d sources", videoInputs.size() );

                std::ostringstream msg;
                msg << "A sequence can not contain more than " << MAX_SOURCES_PER_SEQUENCE << " items";
                throw TA_Base_Bus::VideoSwitchAgentException( msg.str().c_str() );
            }

            for ( ; iVideoInputNumber < videoInputs.size(); iVideoInputNumber++ )
            {
                videoInputs[ iVideoInputNumber ] = newConfig.inputKeySequence[ iVideoInputNumber ];
            }

            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new SetSequenceConfigCommand( newConfig.dwellTime, videoInputs, *this ) );
            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create SetSequenceConfigCommand object" );
                LOGMORE( SourceInfo, "Failed to send Switch command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Switch command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                } 
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Set Sequence Config command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Set Sequence Config command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Set Sequence Config command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }         
    }


    //
    // getSequenceConfig
    //
    TA_Base_Bus::SequenceCorbaDef::SequenceConfig* COESequence::getSequenceConfig()
    {
        TA_Base_Bus::SequenceCorbaDef::SequenceConfig* pSequenceConfig = new TA_Base_Bus::SequenceCorbaDef::SequenceConfig();
        
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
    void COESequence::cycleToNextVideoInput( unsigned long videoOutputEntityKey )
    {
        try
        {     
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new ControlSequencePlaybackCommand( 
                ControlSequencePlaybackCommand::SEQUENCE_SKIP_FORWARD, videoOutputEntityKey) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create ControlSequencePlaybackCommand object" );
                LOGMORE( SourceInfo, "Failed to send Sequence Playback command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Sequence Playback command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                } 
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }         
    }


    //
    // cycleToPreviousVideoInput
    //
    void COESequence::cycleToPreviousVideoInput( unsigned long videoOutputEntityKey )
    {
        try
        {     
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new ControlSequencePlaybackCommand( 
                ControlSequencePlaybackCommand::SEQUENCE_SKIP_BACKWARD, videoOutputEntityKey) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create ControlSequencePlaybackCommand object" );
                LOGMORE( SourceInfo, "Failed to send Sequence Playback command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Sequence Playback command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                }
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        } 
    }


    //
    // pause
    //
    void COESequence::pause( unsigned long videoOutputEntityKey )
    {
        try
        {     
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new ControlSequencePlaybackCommand( 
                ControlSequencePlaybackCommand::SEQUENCE_HOLD, videoOutputEntityKey) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create ControlSequencePlaybackCommand object" );
                LOGMORE( SourceInfo, "Failed to send Sequence Playback command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Sequence Playback command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                } 
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        } 
    }

    
    //
    // play
    //
    void COESequence::play( unsigned long videoOutputEntityKey )
    {
        try
        {     
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new ControlSequencePlaybackCommand( 
                ControlSequencePlaybackCommand::SEQUENCE_PLAY, videoOutputEntityKey) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create ControlSequencePlaybackCommand object" );
                LOGMORE( SourceInfo, "Failed to send Sequence Playback command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Sequence Playback command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                }
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        } 
    }


    //
    // stop
    //
    void COESequence::stop( unsigned long videoOutputEntityKey )
    {
        try
        {     
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new ControlSequencePlaybackCommand( 
                ControlSequencePlaybackCommand::SEQUENCE_HOLD, videoOutputEntityKey) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create ControlSequencePlaybackCommand object" );
                LOGMORE( SourceInfo, "Failed to send Sequence Playback command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Sequence Playback command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                } 
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
    }


    //
    // getSequenceState
    //
    TA_Base_Bus::SequenceCorbaDef::SequenceState COESequence::getSequenceState( unsigned long videoOutputEntityKey )
    {
        // TODO throw exception as this protocol does not support;
        return TA_Base_Bus::SequenceCorbaDef::SequenceState();
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // COESequence Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // getKey
    //
    unsigned long COESequence::getKey() const
    {
        return m_sequenceData->getKey();
    }


    //
    // getKey
    //
    std::string COESequence::getAddress()
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
    void COESequence::validateState()
    {
        COECommunicationsHandler::RequestCommandSharedPtr statusCmd = 
            COECommunicationsHandler::RequestCommandSharedPtr( TA_COE::createRequestSequenceConfigCommand( *this ) );
        COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( statusCmd, false );
        statusCmd->waitForCompletion();
    }

} // namespace TA_IRS_App

