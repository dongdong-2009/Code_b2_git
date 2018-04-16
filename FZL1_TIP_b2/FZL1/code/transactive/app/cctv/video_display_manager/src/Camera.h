/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/Camera.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#if !defined(Camera_H)
#define Camera_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoInput.h"

namespace TA_IRS_App
{

    class Camera : public VideoInput
    {
        public:

            Camera(unsigned long entityKey,
                   const std::string& name,
                   const std::string& description,
                   const std::string& address,
                   const std::string& group,
				   unsigned long locationId,
				   bool isMovementSupported);

            virtual ~Camera();
            virtual std::string getAddress();
            bool getIsMovementSupported();
            const unsigned long getPanSeep();
            void setPanSeep(unsigned long panSeep);
            const unsigned long getTiltSeep();
            void setTiltSeep(unsigned long tiltSpeed);
        private:
            std::string m_address;  // entity address
            bool m_isMovementSupported;
            unsigned long m_panSpeed;
            unsigned long m_tiltSpeed;
    };
}

#endif // !defined(Camera_H)

