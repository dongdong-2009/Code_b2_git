/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/Camera.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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

        Camera( unsigned long entityKey,
                const std::string& name,
                const std::string& description,
                const std::string& address,
                const std::string& group );


        virtual ~Camera() {}
        std::string getAddress();


    protected:

    private:

        // These arent needed
        Camera();
        Camera( const Camera& );
        Camera& operator=( const Camera& );

        std::string m_address;  // entity address

    };
}

#endif // !defined(Camera_H)

