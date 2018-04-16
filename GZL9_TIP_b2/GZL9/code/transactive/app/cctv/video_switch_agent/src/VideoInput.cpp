/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/VideoInput.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles all the common functions of video inputs.
  * Implements the IVideoInput interface. 
  * Uses the IPDVideoInput interface to perform the protocol
  * dependent sections of the control functions.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/SuperimposedTextControl.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/src/VisualAuditMessageSender.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_agent/src/GenericAgent.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_App
{
    const std::string VideoInput::GET_ADDRESS                       = "GetAddress";
    const std::string VideoInput::GET_SUBSYSTE_KEY                  = "GetSubsystemKey";
    const std::string VideoInput::GET_LOCATION_KEY                  = "GetLocationKey";
    const std::string VideoInput::GET_SUPPORTED_FEATURES            = "GetSupportedFeatures";
    const std::string VideoInput::GET_STATE                         = "GetState";
    const std::string VideoInput::GET_KEY                           = "GetKey";
    const std::string VideoInput::GET_NAME                          = "GetName";
    const std::string VideoInput::GET_TYPE                          = "GetType";
    const std::string VideoInput::GET_DESCRIPTION                   = "GetDescription";
    const std::string VideoInput::GET_VIDEO_INPUT_GROUP_NAME        = "GetVideoInputGroupName";
    const std::string VideoInput::IS_SUPERIMPOSED_TEXT_CONTROLLABLE = "IsSuperimposedTextControllable";
    const std::string VideoInput::GET_SUPERIMPOSED_TEXT_CONTROL     = "GetSuperimposedTextControl";
    const std::string VideoInput::IS_PUBLIC_VIEWING_ALLOWED         = "IsPublicViewingAllowed";
    const std::string VideoInput::IS_IN_SERVICE                     = "IsInService";

	//
	// VideoInput
	//
	VideoInput::VideoInput( TA_Base_Core::IEntityDataPtr entityData )
        : VideoSwitchResource( entityData ),
        m_videoInputData( boost::dynamic_pointer_cast<TA_Base_Core::VideoInput>(entityData) ),
          m_pdVideoInput( 0 ),
          m_superimposedTextControl( 0 )
	{
		if ( 0 == m_videoInputData.get() )
		{
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG, 
                "The provided entity data is not VideoInput data." ) );
		}

        // Create an object to handle the protocol dependent work for the VideoInput.

        m_pdVideoInput = std::auto_ptr< IPDVideoInput >( getPDInterfaceFactory().createVideoInput( m_videoInputData ) );
        if ( 0 == m_pdVideoInput.get() )
        {
            // m_videoInputData is cleaned up by GenericAgent, so we don't have to clean it up here.

            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDVideoInput" ) );
        }

        createControls();

        unsigned long key = getKey();
	}

	
	//
	// ~VideoInput
	//
	VideoInput::~VideoInput()
	{
        // If an exception is thrown here, there is nothing we can do, 
        // but we should make sure no exceptions are thrown out of this destructor.

        try
        {
            // Deactivate the Corba servants to they no longer accept incoming requests.
            // They are auto_ptrs, so they will be automatically deleted.

            deactivateCorbaServants();

        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
             LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
        }
    }


    //
    // getVideoInputData
    //
    TA_Base_Core::VideoInputPtr& VideoInput::getVideoInputData()
    {
        TA_ASSERT( 0 != m_videoInputData.get(), "The database object for this VideoInput has not been initialised." );
        return m_videoInputData;
    }
            
    
    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////

        
    //
    // updateState
    //
    void VideoInput::updateState( const TA_Base_Bus_GZL9::VideoInputCorbaDef::VideoInputState& state )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "updateState" );

        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video output tasks is NULL" );
        m_pdVideoInput->updateState( state );

        if ( 0 != m_superimposedTextControl.get() )
        {
            m_superimposedTextControl->updateState( state );
        }

        LOG_FUNCTION_EXIT( SourceInfo, "updateState" );
    }


	///////////////////////////////////////////////////////////////////////
	//
	// VideoInputCorbaDef Methods
	//
	///////////////////////////////////////////////////////////////////////


    //
    // getSupportedFeatures
    //
    TA_Base_Bus_GZL9::VideoInputCorbaDef::SupportedVideoInputFeatures VideoInput::getSupportedVideoInputFeatures()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_Base_Bus_GZL9::VideoInputCorbaDef::SupportedVideoInputFeatures features;

        // Get the VideoInput features.

        if ( 0 != m_superimposedTextControl.get() )
        {
            features.isSuperimposeDateSupported = m_superimposedTextControl->isFeatureSupported( TA_Visual::SuperimposeDate );
            features.isSuperimposeNameSupported = m_superimposedTextControl->isFeatureSupported( TA_Visual::SuperimposeName );
            features.isSuperimposeTextSupported = m_superimposedTextControl->isFeatureSupported( TA_Visual::SuperimposeText );
        }
        else
        {
         	features.isSuperimposeDateSupported = false;
            features.isSuperimposeNameSupported = false;
            features.isSuperimposeTextSupported = false;
        }     

        return features;
    }


    //
    // getVideoInputState
    //
    TA_Base_Bus_GZL9::VideoInputCorbaDef::VideoInputState* VideoInput::getVideoInputState()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_Base_Bus_GZL9::VideoInputCorbaDef::VideoInputState* state = new TA_Base_Bus_GZL9::VideoInputCorbaDef::VideoInputState();
        if ( 0 == state )
        {
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( "Failed to create new state object." );
        }

        try
        {
            // VideoInput state.

            state->sessionIdOfLockOwner = CORBA::string_dup( m_pdVideoInput->getLockOwner().c_str() );
            state->isPublicViewingAllowed = isPublicViewingAllowed();
            state->isInService = isInService();            

            if ( 0 != m_superimposedTextControl.get() )
            {
                state->isDateSuperimposed = m_superimposedTextControl->isDateSuperimposed();
                state->isNameSuperimposed = m_superimposedTextControl->isNameSuperimposed();
                state->superimposedText = m_superimposedTextControl->getSuperimposedText();
            }
            else
            {
         	    state->isDateSuperimposed = false;
                state->isNameSuperimposed = false;
                state->superimposedText = CORBA::string_dup( "" );
            } 
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            delete state;
            state = 0;
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            delete state;
            state = 0;
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            delete state;
            state = 0;
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }

        return state;
    }

    
    //
    // getVideoInputGroupName
    //
    char* VideoInput::getVideoInputGroupName()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_ASSERT( 0 != m_videoInputData.get(), "The video input database object has not been set." );

        try
        {
		    return CORBA::string_dup( m_videoInputData->getVideoInputGroupName().c_str() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }
        return 0;
    }
	    

    //
    // isSuperimposedTextsControllable
    //
    bool VideoInput::isSuperimposedTextControllable()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( IS_SUPERIMPOSED_TEXT_CONTROLLABLE );
		return ( 0 != m_superimposedTextControl.get() );
    }


	//
	// getSuperimposedTextControl
	//
    TA_Base_Bus_GZL9::SuperimposedTextControlCorbaDef_ptr VideoInput::getSuperimposedTextControl()
	{
        AgentModeMonitor::getInstance().verifyInControlMode( GET_SUPERIMPOSED_TEXT_CONTROL );
        TA_ASSERT( isSuperimposedTextControllable(), "VideoInput does not support superimposed text control" );
		return m_superimposedTextControl->_this();
	}


	//
	// lock
	//
	void VideoInput::lock( const char* sessionId )
	{
        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video input tasks is NULL" );

        VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::LockAttempt,
                                                             m_videoInputData->getKey(),
                                                             sessionId,
                                                             m_videoInputData->getName() );

        try
        {
            AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::LOCK );
            m_userPrivilegeVerifier.verifyRights( sessionId, m_videoInputData, TA_Base_Bus::aca_CONTROL_VIDEO_INPUT, TA_Visual::LOCK );

            TA_Base_Core::ThreadGuard guard( m_lockOwnerLock );
        	m_pdVideoInput->lock( sessionId );
		}
		catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& )
        {
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::LockFailure,
                                                                 m_videoInputData->getKey(),
                                                                 sessionId,
                                                                 m_videoInputData->getName() );
            throw;
        }
        catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::LockFailure,
                                                                 m_videoInputData->getKey(),
                                                                 sessionId,
                                                                 m_videoInputData->getName() );

			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}

        VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::LockSuccess,
                                                             m_videoInputData->getKey(),
                                                             sessionId,
                                                             m_videoInputData->getName() );
	}


	//
	// unlock
	//
	void VideoInput::unlock( const char* sessionId )
	{
        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video input tasks is NULL" );

        VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::UnlockAttempt,
                                                             m_videoInputData->getKey(),
                                                             sessionId,
                                                             m_videoInputData->getName() );

        try
        {
            AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::UNLOCK );
            m_userPrivilegeVerifier.verifyRights( sessionId, m_videoInputData, TA_Base_Bus::aca_CONTROL_VIDEO_INPUT, TA_Visual::UNLOCK );

            TA_Base_Core::ThreadGuard guard( m_lockOwnerLock );
        	m_pdVideoInput->unlock( sessionId );
		}
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& )
        {
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::UnlockFailure,
                                                                 m_videoInputData->getKey(),
                                                                 sessionId,
                                                                 m_videoInputData->getName() );
            throw;
        }
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::UnlockFailure,
                                                                 m_videoInputData->getKey(),
                                                                 sessionId,
                                                                 m_videoInputData->getName() );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}

        VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::UnlockSuccess,
                                                             m_videoInputData->getKey(),
                                                             sessionId,
                                                             m_videoInputData->getName() );
	}


	//
	// overrideLock
	//
	void VideoInput::overrideLock( const char* sessionId )
	{
        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video input tasks is NULL" );

        VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::OverrideLockAttempt,
                                                             m_videoInputData->getKey(),
                                                             sessionId,
                                                             m_videoInputData->getName() );

        try
        {
            AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::OVERRIDE_LOCK );
            m_userPrivilegeVerifier.verifyRights( sessionId, m_videoInputData, TA_Base_Bus::aca_CONTROL_VIDEO_INPUT, TA_Visual::OVERRIDE_LOCK );

            TA_Base_Core::ThreadGuard guard( m_lockOwnerLock );
        	m_pdVideoInput->overrideLock( sessionId );
		}
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& )
        {
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::OverrideLockFailure,
                                                                 m_videoInputData->getKey(),
                                                                 sessionId,
                                                                 m_videoInputData->getName() );
            throw;
        }
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::OverrideLockFailure,
                                                                 m_videoInputData->getKey(),
                                                                 sessionId,
                                                                 m_videoInputData->getName() );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}

        VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::OverrideLockSuccess,
                                                             m_videoInputData->getKey(),
                                                             sessionId,
                                                             m_videoInputData->getName() );
	}


	//
	// isPublicViewingAllowed
	//
	bool VideoInput::isPublicViewingAllowed()
	{
        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video input tasks is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( IS_PUBLIC_VIEWING_ALLOWED );
		try
		{
        	return m_pdVideoInput->isPublicViewingAllowed();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
	}


	//
	// setPublicViewingAllowed
	//
	void VideoInput::setPublicViewingAllowed( bool isPublicViewingAllowed, const char* sessionId )
	{
        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video input tasks is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SET_PUBLIC_VIEWING_ALLOWED );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_videoInputData, TA_Base_Bus::aca_CONTROL_VIDEO_INPUT, TA_Visual::SET_PUBLIC_VIEWING_ALLOWED );
		verifyUserHoldsLock( sessionId );
		try
		{
			m_pdVideoInput->setPublicViewingAllowed( isPublicViewingAllowed );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
	}


	//
	// isInService
	//
	bool VideoInput::isInService()
	{
        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video input tasks is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( IS_IN_SERVICE );
		try
		{
        	return m_pdVideoInput->isInService();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
	}


	//
	// setInService
	//
	void VideoInput::setInService( bool isInService, const char* sessionId )
	{
        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video input tasks is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SET_IN_SERVICE );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_videoInputData, TA_Base_Bus::aca_CONTROL_VIDEO_INPUT, TA_Visual::SET_IN_SERVICE );
		verifyUserHoldsLock( sessionId );
		try
		{
			m_pdVideoInput->setInService( isInService );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
	}


	///////////////////////////////////////////////////////////////////////
	//
	// Supporting Methods
	//
	///////////////////////////////////////////////////////////////////////


	//
	// getLockOwner
	//
    std::string VideoInput::getLockOwner()
	{
        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video input tasks is NULL" );
        return m_pdVideoInput->getLockOwner();
	}


    //
    // verifyUserHoldsLock
    //
	void VideoInput::verifyUserHoldsLock( const std::string& sessionId )
    {
        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video input tasks is NULL" );
        m_pdVideoInput->verifyUserHoldsLock( sessionId );
	}

    
    //
    // verifyInService
    //
    void VideoInput::verifyInService()
    {
        TA_ASSERT( 0 != m_pdVideoInput.get(), "The object handling the protocol dependent video input tasks is NULL" );
        if ( !m_pdVideoInput->isInService() )
        {
            std::stringstream error;
            error << "This action cannot be performed because the video input ";
            try
            {
                error << getName();
            }
            catch( const TA_Base_Core::DatabaseException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
                error << "(name unknown)";
            }
            catch( const TA_Base_Core::DataException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
                error << "(name unknown)";
            }
            error << " is out of service.";

        	throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
		}
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntity Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // isValid
    //
    bool VideoInput::isValid()
    {
        return true;
    }


    //
    // start
    //
    void VideoInput::start() 
    {
        // Activate the Corba servants so they accept incoming requests.
		activateCorbaServants();
    }

    
    //
    // stop
    //
    void VideoInput::stop() 
    {
        // Deactivate and delete the Corba servants so they no longer accept incoming requests.
        deactivateCorbaServants();
    }
    

    //
    // update
    //
    void VideoInput::update( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        TA_ASSERT( 0 != m_videoInputData.get(), "The database object for the VideoInput has not been initialised." );
        m_videoInputData->invalidate();
     
        // FUTURE TODO When adding online updates - may need to notify PD object.
        // FUTURE TODO If you update your corba name you have to reregister and update m_myCorbaName.
        // FUTURE TODO If they no longer support superimposed text control, it should be removed.
    }
    
    //
    // remove
    //
    void VideoInput::remove()
    {
        stop();
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////

        
    //
    // createControls
    //
    void VideoInput::createControls()
    {
        // Create the superimposed text control if required.

        try
        {
            // Check that the protocol supports superimposed text control.

            if ( getPDInterfaceFactory().isSuperimposedTextControlSupported() )
            {
                // Check that this Camera supports focus and iris control.

                if ( m_videoInputData->isSuperimposeDateSupported() || 
                     m_videoInputData->isSuperimposeNameSupported() ||
                     m_videoInputData->isSuperimposeTextSupported() )
                {
                    m_superimposedTextControl = std::auto_ptr< SuperimposedTextControl >( new SuperimposedTextControl( *this ) );
                    if ( 0 == m_superimposedTextControl.get() )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                            "Failed to create SuperimposedTextControl" );
                    }
                }
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to create SuperimposedTextControl" );
            m_superimposedTextControl = std::auto_ptr< SuperimposedTextControl >( 0 );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to create SuperimposedTextControl" );
            m_superimposedTextControl = std::auto_ptr< SuperimposedTextControl >( 0 );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to create SuperimposedTextControl" );
            m_superimposedTextControl = std::auto_ptr< SuperimposedTextControl >( 0 );
        }
    }


    //
    // activateCorbaServants
    //
    void VideoInput::activateCorbaServants()
    {
        // Activate the CORBA servants, so they accept incoming calls.

        try
		{
			if ( 0 != m_superimposedTextControl.get() )
			{
				m_superimposedTextControl->activateServant();
			}
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "SuperimposedTextControl CORBA servant deactivation failed." );
        }
        catch( ... )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                "SuperimposedTextControl CORBA servant deactivation failed." );
        }
    }
  

    //
    // deactivateCorbaServants
    //
    void VideoInput::deactivateCorbaServants()
    {
		// Deactivate the CORBA servants, so they no longer accepts incoming calls.
        // Don't call deactivateAndDelete() here because we may not have finished with the object
        // (ie if we're just going into Monitor mode).
        // This is an auto_ptr, so it will be automatically deleted at the right time.

		try
		{
			if ( 0 != m_superimposedTextControl.get() )
			{
				m_superimposedTextControl->deactivateServant();
			}
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "SuperimposedTextControl CORBA servant deactivation failed." );
        }
        catch( ... )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                "SuperimposedTextControl CORBA servant deactivation failed." );
        }
    }  

} // namespace TA_IRS_App
