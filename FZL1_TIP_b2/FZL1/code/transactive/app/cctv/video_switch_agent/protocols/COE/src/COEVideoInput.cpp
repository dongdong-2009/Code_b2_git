/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Rob Young
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Handles the protocol dependent sections of the operations for a VideoInput
  * using the COE protocol.
  * Inherits from IPDVideoInput. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoInput.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

namespace TA_IRS_App
{
    //
    // COEVideoInput
    //
    COEVideoInput::COEVideoInput(TA_Base_Core::VideoInputPtr videoInputData)
        : m_videoInputData(videoInputData),
          m_lockOwner("")
    {
    }

    COEVideoInput::~COEVideoInput()
    {
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // notifyVideoInputStateUpdated
    //
    void COEVideoInput::notifyVideoInputStateUpdated()
    {
    }


    //
    // updateState
    //
    void COEVideoInput::updateState(const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state)
    {
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IPDVideoInput Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // lock
    //
    void COEVideoInput::lock(const std::string& sessionId)
    {
    }


    //
    // unlock
    //
    void COEVideoInput::unlock(const std::string& sessionId)
    {
    }


    //
    // overrideLock
    //
    void COEVideoInput::overrideLock(const std::string& sessionId)
    {
    }


    //
    // getLockOwner
    //
    std::string COEVideoInput::getLockOwner()
    {
        return m_lockOwner;
    }


    //
    // verifyUserHoldsLock
    //
    void COEVideoInput::verifyUserHoldsLock(const std::string& sessionId)
    {
        return; // TODO REMOVE THIS
    }


    //
    // isPublicViewingAllowed
    //
    bool COEVideoInput::isPublicViewingAllowed()
    {
        return true;
    }


    //
    // setPublicViewingAllowed
    //
    void COEVideoInput::setPublicViewingAllowed(bool isPublicViewingAllowed)
    {
    }


    //
    // isInService
    //
    bool COEVideoInput::isInService()
    {
		return true;
    }


    //
    // setInService
    //
    void COEVideoInput::setInService(bool isInService)
    {
    }


} // namespace TA_IRS_App

