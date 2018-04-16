/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/CameraPresetsAccessFactory.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * CameraPresetsAccessFactory is a singleton that is used to generate and retrieve 
  * ICameraPresets objects.
  */

#pragma warning ( disable : 4786 )

#include <string>

#include "core/data_access_interface/video_switch_agent/src/CameraPresetsAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/CameraPresetsData.h"

namespace TA_IRS_Core
{
    //
    // getCameraPresets
    //
    ICameraPresets* CameraPresetsAccessFactory::getCameraPresets( unsigned long cameraKey )
    {
        // Create the CameraPresets object to represent this object.

        ICameraPresets* cameraPresets = new CameraPresetsData( cameraKey );

        // Return the cameraPresets pointer. The class that recieves this pointer is responsible
        // for deleting it.

        return cameraPresets;
    }

} //end namespace TA_IRS_Core


