/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/video_display_manager/src/Quad.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
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
              const std::string& group,
			  const std::string& address,
			  const std::string& cameras);


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
		unsigned char* getCameraAddress() { return m_camerasAddress; }
		std::string getAddress() { return m_address;}

    protected:

        std::vector<Camera*> m_cameras;
		
    private:

        // These arent needed
        Quad();
        Quad( const Quad& );
        Quad& operator=( const Quad& );
		std::string m_address;
		unsigned char m_camerasAddress[4];
    };
}

#endif // !defined(Quad_H)

