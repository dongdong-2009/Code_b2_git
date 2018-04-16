/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/Camera.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#include "Camera.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/FunctionEntry.h"

namespace TA_IRS_App
{

    Camera::Camera(unsigned long entityKey,
                   const std::string& name,
                   const std::string& description,
                   const std::string& address,
                   const std::string& group, unsigned long locationId, bool isMovementSupported)
        : VideoInput(entityKey, name, description, group , locationId), m_isMovementSupported(isMovementSupported),
          m_panSpeed(1),
          m_tiltSpeed(1)
    {
        FUNCTION_ENTRY("Camera");
        m_address = address;
        FUNCTION_EXIT;
    }


    std::string Camera::getAddress()
    {
        FUNCTION_ENTRY("getAddress");
        FUNCTION_EXIT;
        return m_address;
    }

    const unsigned long Camera::getPanSeep()
    {
        return m_panSpeed;
    }

    void Camera::setPanSeep(unsigned long panSeep)
    {
        m_panSpeed = panSeep;
    }

    const unsigned long Camera::getTiltSeep()
    {
        return m_tiltSpeed;
    }

    void Camera::setTiltSeep(unsigned long tiltSpeed)
    {
        m_tiltSpeed = tiltSpeed;
    }
}

