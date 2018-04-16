/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/VideoOutput.h $
  * @author:  Adam Radics
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/05/14 17:41:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#if !defined(VideoOutput_H)
#define VideoOutput_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/cctv/video_display_manager/src/DisplayItem.h"
#include "bus\cctv\video_switch_agent\IDL\src\VideoOutputCorbaDef.h"
#include <vector>

namespace TA_IRS_App
{
    // video inputs can be assigned to video outputs
    class VideoInput;
	class Camera;
	class VideoOutput : public DisplayItem
    {
    public:
        VideoOutput(unsigned long entityKey,
                    const std::string& name,
                    const std::string& description);


        virtual ~VideoOutput() ;

        /**
          * getDetails
          *
          * Gets a more detailed description of the item.
          * This is formatted for printing in a text box.
          * This includes the details of any assigned item.
          *
          * @return The detailed description.
          */
        virtual std::string getDetails();


        /**
          * assignInput
          *
          * Sets the current input for this output
          *
          * @param videoInput a pointer to the input - or NULL to clear the assignment.
          */
        virtual void assignInput(VideoInput* videoInput);

        /**
          * getInput
          *
          * Gets the current input for this output
          *
          * @param a pointer to the input - or NULL if none.
          */
        virtual VideoInput* getInput();

    private:
        VideoInput* m_currentInput;  // current input item (or NULL)
    };
}

#endif // !defined(VideoOutput_H)

