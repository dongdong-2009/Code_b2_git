/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/VideoOutput.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/05/14 17:41:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/VideoInput.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/VideoOutput.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include <algorithm>
#include <sstream>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

namespace TA_IRS_App
{

    VideoOutput::VideoOutput( unsigned long entityKey,
                              const std::string& name,
                              const std::string& description )
      : DisplayItem(entityKey, name, description),
        m_currentInput(NULL)
    {
    }

	VideoOutput::~VideoOutput()
	{
	}

    void VideoOutput::assignInput(VideoInput* videoInput)
    {
        ThreadGuard guard( m_lock );
        m_currentInput = videoInput;
    }

    VideoInput* VideoOutput::getInput()
    {
        ThreadGuard guard( m_lock );
        return m_currentInput;
    }

    std::string VideoOutput::getDetails()
    {
        ThreadGuard guard( m_lock );
        // get the item details
        std::ostringstream detailsStr;
        detailsStr << DisplayItem::getDetails() << ":" << "\r\n";
        // details of current assignment - if any
        if (m_currentInput != NULL)
        {
            detailsStr << m_currentInput->getDetails();
        }
        else
        {
            detailsStr << "No current assignment";
        }
        return detailsStr.str();
    }
} // TA_App
