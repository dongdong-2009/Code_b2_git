/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoOutput.h $
  * @author:  Adam Radics
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/08/30 16:09:14 $
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
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"

#include <list>
#include <vector>

namespace TA_IRS_App
{
    // video inputs can be assigned to video outputs
    class VideoInput;
  //  class BvsStage;

    class VideoOutput : public DisplayItem
    {
    public:
        VideoOutput( unsigned long entityKey,
                     const std::string& name,
                     const std::string& description );


        virtual ~VideoOutput() {}

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

		virtual std::string getAddress() = 0;


    protected:

        VideoInput* m_currentInput;  // current input item (or NULL)
        std::string m_name;
    };
}

#endif // !defined(VideoOutput_H)

