/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/Quad.h $
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


#if !defined(Quad_H)
#define Quad_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoInput.h"

#include <vector>

namespace TA_IRS_App
{
    class Camera;

    class Quad : public VideoInput
    {
    public:

        Quad( unsigned long entityKey,
              const std::string& name,
              const std::string& description,
              const std::string& group,unsigned long locationId  );


        virtual ~Quad() {}

        /**
          * getCameras
          *
          * Gets the cameras associated with this Quad.
          * Always 4 - quadrants with no camera have NULL.
          * The order is Top Left, Top Right, Bottom Left, Bottom Right
          *
          * @return the list of cameras. Order is important.
          */
        virtual std::vector<Camera*> getCameras();

        /**
          * setTopLeft
          *
          * Sets the cameras associated with this Quad.
          *
          * @param topLeft  The top left camera
          */
        virtual void setTopLeft(Camera* topLeft);

        /**
          * setTopRight
          *
          * Sets the cameras associated with this Quad.
          *
          * @param topRight  The top right camera
          */
        virtual void setTopRight(Camera* topRight);

        /**
          * setBottomLeft
          *
          * Sets the cameras associated with this Quad.
          *
          * @param bottomLeft  The bottom left camera
          */
        virtual void setBottomLeft(Camera* bottomLeft);

        /**
          * setBottomRight
          *
          * Sets the cameras associated with this Quad.
          *
          * @param bottomRight  The bottom right camera
          */
        virtual void setBottomRight(Camera* bottomRight);


        /**
          * getDetails
          *
          * Gets a more detailed description of the item.
          * This is formatted for printing in a text box.
          *
          * @return The detailed description.
          */
        virtual std::string getDetails();

	
		virtual std::string getAddress();


    protected:

        std::vector<Camera*> m_cameras;
    };
}

#endif // !defined(Quad_H)

