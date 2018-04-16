/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Katherine Thomson
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
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
    VideoInput::VideoInput(TA_Base_Core::IEntityDataPtr entityData)
        : VideoSwitchResource(entityData),
          m_videoInputData(boost::dynamic_pointer_cast<TA_Base_Core::VideoInput>(entityData)),
          m_pdVideoInput(0)
    {
        if(0 == m_videoInputData.get())
        {
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG, "The provided entity data is not VideoInput data."));
        }
        // Create an object to handle the protocol dependent work for the VideoInput.
        m_pdVideoInput = std::auto_ptr< IPDVideoInput >(PDInterfaceFactory::createVideoInput(m_videoInputData));
        if(0 == m_pdVideoInput.get())
        {
            // m_videoInputData is cleaned up by GenericAgent, so we don't have to clean it up here.
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDVideoInput"));
        }
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
        catch(const TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Reason unknown");
        }
    }


    //
    // getVideoInputData
    //
    TA_Base_Core::VideoInputPtr& VideoInput::getVideoInputData()
    {
        TA_ASSERT(0 != m_videoInputData.get(), "The database object for this VideoInput has not been initialised.");
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
    void VideoInput::updateState(const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state)
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "updateState");
        TA_ASSERT(0 != m_pdVideoInput.get(), "The object handling the protocol dependent video output tasks is NULL");
        m_pdVideoInput->updateState(state);
        LOG_FUNCTION_EXIT(SourceInfo, "updateState");
    }


    //
    // getVideoInputState
    //
    TA_Base_Bus::VideoInputCorbaDef::VideoInputState* VideoInput::getVideoInputState()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_Base_Bus::VideoInputCorbaDef::VideoInputState* state = new TA_Base_Bus::VideoInputCorbaDef::VideoInputState();
        state->sessionIdOfLockOwner = CORBA::string_dup(m_pdVideoInput->getLockOwner().c_str());
        state->isInService = true;
        state->isDateSuperimposed = false;
        state->isNameSuperimposed = false;
        state->superimposedText = CORBA::string_dup("");
        return state;
    }


    //
    // getVideoInputGroupName
    //
    char* VideoInput::getVideoInputGroupName()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_ASSERT(0 != m_videoInputData.get(), "The video input database object has not been set.");
        try
        {
            return CORBA::string_dup(m_videoInputData->getVideoInputGroupName().c_str());
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
    }
    //
    // lock
    //
    void VideoInput::lock(const char* sessionId)
    {
    }


    //
    // unlock
    //
    void VideoInput::unlock(const char* sessionId)
    {
    }


    //
    // overrideLock
    //
    void VideoInput::overrideLock(const char* sessionId)
    {
    }


    //
    // isInService
    //
    bool VideoInput::isInService()
    {
        return true;
    }


    //
    // setInService
    //
    void VideoInput::setInService(bool isInService, const char* sessionId)
    {
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
        return "";
    }


    //
    // verifyUserHoldsLock
    //
    void VideoInput::verifyUserHoldsLock(const std::string& sessionId)
    {
    }


    //
    // verifyInService
    //
    void VideoInput::verifyInService()
    {
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
    void VideoInput::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        TA_ASSERT(0 != m_videoInputData.get(), "The database object for the VideoInput has not been initialised.");
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
    // activateCorbaServants
    //
    void VideoInput::activateCorbaServants()
    {
    }


    //
    // deactivateCorbaServants
    //
    void VideoInput::deactivateCorbaServants()
    {
    }

} // namespace TA_IRS_App
