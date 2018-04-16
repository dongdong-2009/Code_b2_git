/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/VideoEnquirer.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Light-weight singleton class to communicate with the VideoSwitchAgent
  * for selected enquiry functions only :
  *     - get camera name
  *
  */

#if !defined(AFX_VIDEO_ENQUIRER_H__C08A7DFF_F0FD_4409_BDE5_EB600F0B2A96__INCLUDED_)
#define AFX_VIDEO_ENQUIRER_H__C08A7DFF_F0FD_4409_BDE5_EB600F0B2A96__INCLUDED_

#include <string>
#include "bus/cctv/video_switch_agent/IDL/src/MovementControlCorbaDef.h"

namespace TA_Base_App
{
    class VideoEnquirer
    {
    public:
        VideoEnquirer() {};
        ~VideoEnquirer() {};

        /**
          * getCameraName
          *
          * Gets the name of the Camera with the specified key.
          *
          * @return     std::string
          *             The name of the Camera.
          *
          * @param      unsigned long cameraEntityKey
          *             The entity key of the Camera.
          *
          * @exception  TransactiveException
          *             Thrown if:
          *             1. The VideoSwitchAgent cannot be contacted
          *             2. The Camera with the given entity key was not found.
          *             3. The Camera name cannot be retrieved from the database.
          */

        std::string getCameraName( unsigned long cameraEntityKey );

        /**
          * getMonitorName
          *
          * Gets the name of the Monitor with the specified key.
          *
          * @return     std::string
          *             The name of the Monitor.
          *
          * @param      unsigned long monitorEntityKey
          *             The entity key of the Monitor.
          *
          * @exception  TransactiveException
          *             Thrown if:
          *             1. The VideoSwitchAgent cannot be contacted
          *             2. The Monitor with the given entity key was not found.
          *             3. The Monitor name cannot be retrieved from the database.
          */

        std::string getMonitorName( unsigned long monitorEntityKey );

    };
}

#endif // AFX_VIDEO_ENQUIRER_H__C08A7DFF_F0FD_4409_BDE5_EB600F0B2A96__INCLUDED_
