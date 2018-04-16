/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoOutput.cpp $
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


//#include "BvsStage.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/VideoInput.h"
#include "app/cctv/video_display_manager/src/VideoOutput.h"

//#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

//#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include <algorithm>
#include <sstream>


using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

using TA_IRS_Core::VideoTriggeringEventAccessFactory;
using TA_IRS_Core::IVideoTriggeringEvent;
/*
using TA_IRS_Core::VideoStationIdAccessFactory;
typedef TA_IRS_Core::VideoStationIdAccessFactory::StationIdToLocationKeyMap StationIdToLocationKeyMap;
*/

namespace TA_IRS_App
{

    VideoOutput::VideoOutput( unsigned long entityKey,
                              const std::string& name,
                              const std::string& description )
      : DisplayItem(entityKey, name, description),
        m_currentInput(NULL),
        m_name(name)
    {
        FUNCTION_ENTRY( "VideoOutput" );
        FUNCTION_EXIT;
    }


    void VideoOutput::assignInput(VideoInput* videoInput)
    {
        FUNCTION_ENTRY( "assignInput" );

        ThreadGuard guard( m_lock );
        m_currentInput = videoInput;

        FUNCTION_EXIT;
    }


    VideoInput* VideoOutput::getInput()
    {
        FUNCTION_ENTRY( "getInput" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_currentInput;
    }



    std::string VideoOutput::getDetails()
    {
        FUNCTION_ENTRY( "getDetails" );

        // reentrant lock - parent class uses the same lock
        ThreadGuard guard( m_lock );

        // get the item details
        std::ostringstream detailsStr;
        detailsStr << DisplayItem::getDetails() << ":" << "\r\n";

        if (m_currentInput != NULL)
        {
            detailsStr << m_currentInput->getDetails();
        }
        else
        {
            detailsStr << "No current assignment";
        }

        FUNCTION_EXIT;
        return detailsStr.str();
    }
} // TA_App
