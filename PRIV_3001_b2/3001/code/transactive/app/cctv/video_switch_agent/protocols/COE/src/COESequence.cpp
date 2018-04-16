/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COESequence.cpp $
  * @author:   Bruce Polmear
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/GlobalConstants.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/tokenizer.hpp>

namespace TA_IRS_App
{
    //
    // COESequence
    //
    COESequence::COESequence( TA_Base_Core::SequencePtr sequenceData )
    : m_sequenceData( sequenceData ), m_dwellTime( 0 ), m_videoInputs()
    {
		m_dwellTime = PREDEFINE_DWELL_TIME;
		TA_Base_Core::EntityHelper* help=sequenceData->getHelper();

		// If it is editable, get dwell time from database
		if (!sequenceData->isReadOnly())
		{
			std::string temp = help->getParameter("SequenceDwellTime");
			m_dwellTime = atoi(temp.c_str());
		}

        m_cameraList = help->getParameter("AttachedCameras");
		
		COESwitchManager::getInstance().addSequence(*this);
    }


    //
    // Destructor
    //
    COESequence::~COESequence()
    {
    }

	void COESequence::addCameraForSequence(TA_IRS_App::SwitchManager::CameraMap& cameras)
	{
		typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(",");
		tokenizer tokens(m_cameraList, sep);
		for(tokenizer::iterator it = tokens.begin(); it != tokens.end(); ++it)
		{
			try
			{
				//TA_Base_Core::IEntityDataPtr entity(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(*it));
				TA_IRS_App::SwitchManager::CameraMap::iterator it_camera = cameras.begin();
				for(; it_camera != cameras.end(); ++it_camera)
				{
					Camera& camera = *(it_camera->second);
					std::string cameraName = camera.getName();
					if (*it == cameraName)
					{
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Load sequence %s attached camera %s", m_sequenceData->getName().c_str(), cameraName.c_str());
						m_videoInputs.push_back(it_camera->first);
						break;
					}
				}
			}
			catch (...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Load sequence %s attached camera %s failed", m_sequenceData->getName().c_str(), (*it).c_str());
			}
		}
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
		m_videoInputs.clear();
		m_dwellTime = sequenceConfig.dwellTime;
		int inputNum = sequenceConfig.inputKeySequence.length();
		for(int i=0;i<inputNum;i++)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"updateSequenceConfig %d sources", sequenceConfig.inputKeySequence[i]);
			m_videoInputs.insert(m_videoInputs.end(), sequenceConfig.inputKeySequence[i]);
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
            // check that there are no more than 64 cameras in this sequence
            // it is checked in the GUI but check here as it is a protocol limitation
            if (videoInputs.size() > MAX_SOURCES_PER_SEQUENCE)
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Client tried to set a sequence that is too long %d sources", videoInputs.size() );
				throw EXCEPTION_PERFORMING_REQUEST;
            }
			for(unsigned long i = 0, sz = videoInputs.size(); i < sz; ++i)
            {
                videoInputs[i] = newConfig.inputKeySequence[i];
            }	

			m_dwellTime = newConfig.dwellTime;
            COECommunicationsHandler::RequestCommandSharedPtr command;
			try
			{
				command = COECommunicationsHandler::RequestCommandSharedPtr(new SetSequenceConfigCommand( newConfig.dwellTime, videoInputs, *this ));
			}
			catch (...)
			{
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create SetSequenceConfigCommand object" );
				throw EXCEPTION_PERFORMING_REQUEST;
			}
            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );
            command->waitForCompletion();
            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
				if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
				{
					throw EXCEPTION_CONNECTION_WITH_VSS;
				}
				throw EXCEPTION_PERFORMING_REQUEST;
            }

        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Set Sequence Config command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Set Sequence Config command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Set Sequence Config command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw EXCEPTION_PERFORMING_REQUEST;
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
		for(unsigned long i = 0, sz = m_videoInputs.size(); i < sz; ++i)
        {
            pSequenceConfig->inputKeySequence[i] = m_videoInputs[i];
        }

        return pSequenceConfig;
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
} // namespace TA_IRS_App

