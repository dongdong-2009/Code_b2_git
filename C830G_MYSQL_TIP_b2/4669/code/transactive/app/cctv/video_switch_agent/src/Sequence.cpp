/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/Sequence.cpp $
 * @author:  Jade Welton
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2010/01/26 19:01:38 $
 * Last modified by:  $Author: builder $
 * 
 * <description>
 */
#include "app/cctv/video_switch_agent/src/Sequence.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/ConfigUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDSequence.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/src/VisualAuditMessageSender.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

//#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"  // TD14548

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
    , m_userPrivilegeVerifier( UserPrivilegeVerifier::getInstance() ) // HuangQi++
    {
        if ( m_sequenceEntityData.get() == NULL )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                        TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG,
                        "Failed to cast entity data to Sequence data" ) );
        }
        m_pdSequence = std::auto_ptr< IPDSequence >( PDInterfaceFactory::createSequence( m_sequenceEntityData ) );    

		activateServantAtInit();
    }


    //
    // Destructor
    //
    Sequence::~Sequence()
    {    

		deactivateServantAtClose();
/*        try
        {
            stop();
        }
        catch( TA_Base_Core::TransactiveException& tex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", tex.what() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught deleting Sequence object" );
        } */
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

        LOG_FUNCTION_EXIT( SourceInfo, "updateSequenceConfig" );
    }


    //
    // isReadOnly
    //
    bool Sequence::isReadOnly()
    {
        if ( m_pdSequence.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
        return m_sequenceEntityData->isReadOnly();
    }

	//
	// getSequenceDescription TD16691
	char* Sequence::getSequenceDescription()
	{
        if ( m_pdSequence.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }

		return CORBA::string_dup((m_sequenceEntityData->getSequenceDescription()).c_str());
	}
	// TD16691

    //
    // getDwellTimeRange
    //
    void Sequence::getDwellTimeRange( unsigned short& minimumDwellTime, unsigned short& maximumDwellTime )
    {
        if ( m_pdSequence.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
        m_pdSequence->getDwellTimeRange( minimumDwellTime, maximumDwellTime );
    }


    //
    // getMaxNumberOfInputs
    //
    unsigned short Sequence::getMaxNumberOfInputs()
    {
        if ( m_pdSequence.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
        return m_pdSequence->getMaxNumberOfInputs();
    }


    //
    // getSequenceConfig
    //
    TA_Base_Bus::SequenceCorbaDef::SequenceConfig* Sequence::getSequenceConfig()
    {
        if ( m_pdSequence.get() == NULL )
        {    
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
    
        // Pass to protocol dependent component
        return m_pdSequence->getSequenceConfig();    
    }


    //
    // setSequenceConfig
    //
    void Sequence::setSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& newConfig, const char* sessionId )
    {
        if ( isReadOnly() )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Sequence configuration is read only" );
        }

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SEQUENCE_SET_CONFIG );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_sequenceEntityData,
                                              TA_Base_Bus::aca_EDIT_SEQUENCES, 
                                              "Edit Sequence Configuration" );
        
        // Pass to protocol dependent component
        m_pdSequence->setSequenceConfig( newConfig );

        try
        {
			// TD14548
			std::string operatorName;
			TA_Base_Core::ISession* session = NULL;
			session  = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);
			if (session != NULL)
			{
				operatorName = session->getOperatorName();

				delete session;
				session = NULL;
			}
			// TD14548 - change audit message type
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::EditSequence, m_sequenceEntityData->getKey(), sessionId, operatorName, m_sequenceEntityData->getName() );
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
            throw TA_Base_Bus::VideoSwitchAgentException( "Sequence configuration is read only" );
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
            try
            {
                VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::ActionSuccess, m_sequenceEntityData->getKey(), sessionId, "Saved New Sequence Description", m_sequenceEntityData->getName() );
            }
            catch ( ... )
            {
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }  
    }


    //
    // cycleToNextVideoInput
    //
    void Sequence::cycleToNextVideoInput( unsigned long videoOutputEntityKey, const char* sessionId )
    {
        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SEQUENCE_CYCLE_TO_NEXT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_sequenceEntityData,
                                              TA_Base_Bus::aca_CONTROL_VIDEO_INPUT, 
                                              TA_Visual::SEQUENCE_CYCLE_TO_NEXT );

        m_pdSequence->cycleToNextVideoInput( videoOutputEntityKey );

        try
        {
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::ActionSuccess, m_sequenceEntityData->getKey(), sessionId, "Set Sequence Playback to CYCLE FORWARD", m_sequenceEntityData->getName() );
        }
        catch ( ... )
        {
        }
    }


    //
    // cycleToPreviousVideoInput
    //
    void Sequence::cycleToPreviousVideoInput( unsigned long videoOutputEntityKey, const char* sessionId )
    {
        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SEQUENCE_CYCLE_TO_PREVIOUS );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_sequenceEntityData,
                                              TA_Base_Bus::aca_CONTROL_VIDEO_INPUT, 
                                              TA_Visual::SEQUENCE_CYCLE_TO_PREVIOUS );

        m_pdSequence->cycleToPreviousVideoInput( videoOutputEntityKey );

        try
        {
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::ActionSuccess, m_sequenceEntityData->getKey(), sessionId, "Set Sequence Playback to CYCLE BACKWARD", m_sequenceEntityData->getName() );
        }
        catch ( ... )
        {
        }
    }


    //
    // pause
    //
    void Sequence::pause( unsigned long videoOutputEntityKey, const char* sessionId )
    {
        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SEQUENCE_PAUSE );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_sequenceEntityData,
                                              TA_Base_Bus::aca_CONTROL_VIDEO_INPUT,
                                              TA_Visual::SEQUENCE_PAUSE );

        m_pdSequence->pause( videoOutputEntityKey );

        try
        {
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::ActionSuccess, m_sequenceEntityData->getKey(), sessionId, "Set Sequence Playback to PAUSE", m_sequenceEntityData->getName() );
        }
        catch ( ... )
        {
        }
    }


    //
    // play
    //
    void Sequence::play( unsigned long videoOutputEntityKey, const char* sessionId )
    {
        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SEQUENCE_PLAY );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_sequenceEntityData,
                                              TA_Base_Bus::aca_CONTROL_VIDEO_INPUT,
                                              TA_Visual::SEQUENCE_PLAY );

        m_pdSequence->play( videoOutputEntityKey );

        try
        {
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::ActionSuccess, m_sequenceEntityData->getKey(), sessionId, "Set Sequence Playback to PLAY/RESUME", m_sequenceEntityData->getName() );
        }
        catch ( ... )
        {
        }
    }


    //
    // stop
    //
    void Sequence::stop( unsigned long videoOutputEntityKey, const char* sessionId )
    {
        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SEQUENCE_STOP );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_sequenceEntityData,
                                              TA_Base_Bus::aca_CONTROL_VIDEO_INPUT,
                                              TA_Visual::SEQUENCE_STOP );

        m_pdSequence->stop( videoOutputEntityKey );

        try
        {
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::ActionSuccess, m_sequenceEntityData->getKey(), sessionId, "Set Sequence Playback to PAUSE", m_sequenceEntityData->getName() );
        }
        catch ( ... )
        {
        }
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
        deactivateServant();
        VideoInput::stop();
    }


    //
    // getSequenceState
    //
    TA_Base_Bus::SequenceCorbaDef::SequenceState Sequence::getSequenceState( unsigned long videoOutputEntityKey )
    {
        try
        {
            return m_pdSequence->getSequenceState( videoOutputEntityKey );
        }
        catch ( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
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
        // TODO
    }


    //
    // setToControlMode
    //
    void Sequence::activateServantAtInit()
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
    void Sequence::deactivateServantAtClose()
    {
        deactivateServant();
    }

}
