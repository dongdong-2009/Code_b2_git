/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/src/Sequence.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * <description>
 */
#include "app/cctv/video_switch_agent/src/Sequence.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/ConfigUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDSequence.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/GlobalConstants.h"
#include "app/cctv/video_switch_agent/src/VisualAuditMessageSender.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include <sstream>

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

    //
    // Constructor
    //
    Sequence::Sequence(TA_Base_Core::IEntityDataPtr entityData )
    : VideoInput( entityData )
    , m_sequenceEntityData( boost::dynamic_pointer_cast<TA_Base_Core::Sequence>( entityData ) )
    , m_pdSequence( NULL )
    {
        if ( m_sequenceEntityData.get() == NULL )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                        TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG,
                        "Failed to cast entity data to Sequence data" ) );
        }
        m_pdSequence = std::auto_ptr< IPDSequence >( PDInterfaceFactory::createSequence( m_sequenceEntityData ) );    
		//activateServantAtInit();
    }

	void Sequence::addCameraForSequence(TA_IRS_App::SwitchManager::CameraMap& cameras)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "addCameraForSequence: %d, %s " , getKey(), getName());
		m_pdSequence->addCameraForSequence(cameras);
	}

    //
    // Destructor
    //
    Sequence::~Sequence()
    {    
		deactivateTheServant();
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // updateState
    //
    void Sequence::updateState( const TA_Base_Bus::SequenceCorbaDef::SequenceState& state )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "updateState" );
        TA_ASSERT( 0 != m_pdSequence.get(), "The object handling the protocol dependent sequence tasks is NULL" );
        m_pdSequence->updateState( state );
        LOG_FUNCTION_EXIT( SourceInfo, "updateState" );
    }


    //
    // updateSequenceConfig
    //
    void Sequence::updateSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "updateSequenceConfig" );

        TA_ASSERT( 0 != m_pdSequence.get(), "The object handling the protocol dependent sequence tasks is NULL" );
        m_pdSequence->updateSequenceConfig( sequenceConfig );  
		const char* tmp = sequenceConfig.description;
		std::string desc = tmp;
		m_sequenceEntityData->setSequenceDescription(desc);
        LOG_FUNCTION_EXIT( SourceInfo, "updateSequenceConfig" );
    }


    //
    // isReadOnly
    //
    bool Sequence::isReadOnly()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();

        if ( m_pdSequence.get() == NULL )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Internal error: protocol dependent component not initialised" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        return m_sequenceEntityData->isReadOnly();
    }

	//
	// getSequenceDescription
	char* Sequence::getSequenceDescription()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();

        if ( m_pdSequence.get() == NULL )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Internal error: protocol dependent component not initialised" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }

		return CORBA::string_dup((m_sequenceEntityData->getSequenceDescription()).c_str());
	}

    //
    // getDwellTimeRange
    //
    void Sequence::getDwellTimeRange( unsigned short& minimumDwellTime, unsigned short& maximumDwellTime )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();

        if ( m_pdSequence.get() == NULL )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Internal error: protocol dependent component not initialised" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        m_pdSequence->getDwellTimeRange( minimumDwellTime, maximumDwellTime );
    }


    //
    // getMaxNumberOfInputs
    //
    unsigned short Sequence::getMaxNumberOfInputs()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();

        if ( m_pdSequence.get() == NULL )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Internal error: protocol dependent component not initialised" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        return m_pdSequence->getMaxNumberOfInputs();
    }


    //
    // getSequenceConfig
    //
    TA_Base_Bus::SequenceCorbaDef::SequenceConfig* Sequence::getSequenceConfig()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();

        if ( m_pdSequence.get() == NULL )
        {    
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Internal error: protocol dependent component not initialised" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }
    
        // Pass to protocol dependent component
		TA_Base_Bus::SequenceCorbaDef::SequenceConfig* seqConf = m_pdSequence->getSequenceConfig();
		seqConf->description = getSequenceDescription();
		return seqConf;
    }


    //
    // setSequenceConfig
    //
    void Sequence::setSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& newConfig, const char* sessionId )
    {
        if ( isReadOnly() )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Sequence configuration is read only" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SEQUENCE_SET_CONFIG );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_sequenceEntityData,
                                              TA_Base_Bus::aca_EDIT_SEQUENCES, 
                                              "Edit Sequence Configuration" );
        
		// Pass to protocol dependent component
		m_pdSequence->setSequenceConfig( newConfig );

		// peter.wong, if all successful, save into database
		std::map<std::string, std::string> parameterList;
		std::string attached = "";
		std::string attachedCameras = "";
		for ( unsigned long i = 0, sz = newConfig.inputKeySequence.length(); i < sz; ++i )
		{
			std::auto_ptr<TA_Base_Core::IEntityData> entity(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(newConfig.inputKeySequence[i]));
			attached += entity->getName() + ",";
		}
		attachedCameras = attached.substr(0, attached.size()-1);
		parameterList.insert(std::make_pair("AttachedCameras",attachedCameras));
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Updating database with new AttachedCameras:%s for sequence %s", attachedCameras.c_str(), m_sequenceEntityData->getName().c_str());
		std::string dwellTime;
		std::stringstream temp;
		temp << newConfig.dwellTime;
		dwellTime = temp.str();
		parameterList.insert(std::make_pair("SequenceDwellTime",dwellTime));
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Updating database with new DwellTime:%s for sequence %s", dwellTime.c_str(), m_sequenceEntityData->getName().c_str());
		TA_Base_Core::EntityHelper* helper = m_sequenceEntityData->getHelper();
		helper->writeParameters(parameterList);

        try
        {
            // Send audit message
            /*
			std::string operatorName;
			TA_Base_Core::ISession* session = NULL;
			session  = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);
			if (session != NULL)
			{
				operatorName = session->getOperatorName();

				delete session;
				session = NULL;
			}
			std::string seqenceAddress = m_sequenceEntityData->getAddress();
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::EditSequence, m_sequenceEntityData->getKey(), sessionId, operatorName, seqenceAddress);
            */

			// peter.wong, now broadcast to all Display Manager
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Broadcasting updated config for sequence %s", m_sequenceEntityData->getName().c_str());
			StateUpdateBroadcaster::getInstance().notifySequenceConfigUpdate(*this);
        }
        catch ( ... )
        {
        }
    }


    //
    // setSequenceDescription
    //
    void Sequence::setSequenceDescription( const char* newDescription, const char* sessionId )
    {
        if ( isReadOnly() )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Sequence configuration is read only" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SEQUENCE_SET_CONFIG );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_sequenceEntityData,
                                              TA_Base_Bus::aca_EDIT_SEQUENCES, 
                                              "Edit Sequence Description" );

        try
        {
            unsigned long sequenceKey = m_sequenceEntityData->getKey();
            // TD16691
			// m_sequenceEntityData->setDescription( newDescription );
			m_sequenceEntityData->setSequenceDescription(newDescription);
            // ConfigUpdateBroadcaster::getInstance().getInstance().notifyEntityDescriptionUpdated( sequenceKey );
			// TD16691

			// peter.wong, if all successful, save into database
			TA_Base_Core::EntityHelper* help=m_sequenceEntityData->getHelper();
			std::map<std::string, std::string> parameterList;
			std::string desc(newDescription);
			parameterList.insert(std::make_pair("SequenceDescription",desc));
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Updating database with new description:%s for sequence %s", desc.c_str(), m_sequenceEntityData->getName().c_str());
			help->writeParameters(parameterList);

			// peter.wong, now broadcast to all Display Manager
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Broadcasting updated config for sequence %s", m_sequenceEntityData->getName().c_str());
			StateUpdateBroadcaster::getInstance().notifySequenceConfigUpdate(*this);

			// send EditSequence event
            /*
			std::string operatorName;
			std::auto_ptr<TA_Base_Core::ISession> session(TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId));
			operatorName = session->getOperatorName();
			std::string seqenceAddress = m_sequenceEntityData->getAddress();
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::EditSequence, m_sequenceEntityData->getKey(), sessionId, operatorName, seqenceAddress);
            */
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw EXCEPTION_PERFORMING_REQUEST;
        }  
    }


    //
    // cycleToNextVideoInput
    //
    void Sequence::cycleToNextVideoInput( unsigned long videoOutputEntityKey, const char* sessionId )
    {
		return;
    }


    //
    // cycleToPreviousVideoInput
    //
    void Sequence::cycleToPreviousVideoInput( unsigned long videoOutputEntityKey, const char* sessionId )
    {
		return;
    }


    //
    // pause
    //
    void Sequence::pause( unsigned long videoOutputEntityKey, const char* sessionId )
    {
		return;
    }


    //
    // play
    //
    void Sequence::play( unsigned long videoOutputEntityKey, const char* sessionId )
    {
		return;
    }


    //
    // stop
    //
    void Sequence::stop( unsigned long videoOutputEntityKey, const char* sessionId )
    {
		return;
    }


    //
    // start
    //
    void Sequence::start()
    {
        VideoInput::start();
    }


    //
    // stop
    //
    void Sequence::stop()
    {
        deactivateTheServant();
        VideoInput::stop();
    }


    //
    // getSequenceState
    //
	// Useless in c955
    TA_Base_Bus::SequenceCorbaDef::SequenceState Sequence::getSequenceState( unsigned long videoOutputEntityKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        return m_pdSequence->getSequenceState( videoOutputEntityKey );
    }


    //
    // update
    //
    void Sequence::update( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        m_sequenceEntityData->invalidate();
        VideoInput::update( updateEvent );
    }


    //
    // remove
    //
    void Sequence::remove()
    {
    }


    //
    // setToControlMode
    //
    void Sequence::activateTheServant()
    {
        try
        {
            activateServantWithName( m_sequenceEntityData->getName() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "A Sequence cannot be activated." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "A Sequence cannot be activated." );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "A Sequence cannot be activated." );
        }
        catch( ... )
        {
            // There is nothing we can do to recover.
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "A Sequence cannot be activated." );
        }
    }


    //
    // setToMonitorMode
    //
    void Sequence::deactivateTheServant()
    {
        deactivateServant();
    }

	void Sequence::setToControlMode()
	{
		activateTheServant();
	}

	void Sequence::setToMonitorMode()
	{
		deactivateTheServant();
	}

}
