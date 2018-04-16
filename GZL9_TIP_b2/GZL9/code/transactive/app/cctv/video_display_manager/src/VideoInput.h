/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoInput.h $
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


#if !defined(VideoInput_H)
#define VideoInput_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DisplayItem.h"

namespace TA_IRS_App
{
    class VideoInput : public DisplayItem
    {
    public:

        VideoInput( unsigned long entityKey,
                    const std::string& name,
                    const std::string& description,
                    const std::string& group , unsigned long locationId  );


        virtual ~VideoInput() {}

        /**
          * getGroup
          *
          * Gets the group for this input
          *
          * @return The tree group eg OCC/Group1/SubGroup2
          */
        virtual std::string getGroup();

		virtual unsigned long getLocationId () 
		{
			 return m_locationId;
		}

		virtual std::string getAddress() = 0;

    protected:

        std::string m_group;    // the group - for displaying in the tree
		unsigned long m_locationId;

    private:

        // These arent needed
        VideoInput();
        VideoInput( const VideoInput& );
        VideoInput& operator=( const VideoInput& );


    };
}

#endif // !defined(VideoInput_H)

