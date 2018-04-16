/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/CameraPresetsAccessFactory.cpp $
  * @author Katherine Thomson
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
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


