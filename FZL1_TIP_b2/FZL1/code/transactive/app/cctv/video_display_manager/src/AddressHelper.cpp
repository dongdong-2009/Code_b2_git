/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/FZL1_TIP/FZL1/transactive/app/cctv/video_display_manager/src/AddressHelper.cpp $
  * @author:   zhang hongzhi
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/05/08 16:15:14 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * Provides helper functions and a struct for handling camera addresses.
  */

#ifndef __WIN32__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#endif

#include "app/cctv/video_display_manager/src/AddressHelper.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{
    AddressHelper::AddressHelper()
    {
    }

    AddressHelper::~AddressHelper()
    {
    }
    //
    // splitAddress
    //
    AddressHelper::Address AddressHelper::splitAddress(const std::string& address)
    {
        Address splitAddress;
		std::string stationID = "";
        std::string cameraID = "";
        // Could be of the form address:port or just address
        std::string::size_type col = address.find_first_of(":", 0);
        if(col != std::string::npos)
        {
            // address:port
            stationID = address.substr(0, col);
            cameraID = address.substr(col + 1, std::string::npos);

			std::stringstream ss;
			ss << stationID;
			ss >> splitAddress.stationID;

			ss.clear();
			ss.str("");
			ss << cameraID;
			ss >> splitAddress.cameraID;
        }
        else
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "The format of camera address is not correct: %s", address.c_str());
        }
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "StationID: %d, CameraID: %d", splitAddress.stationID, splitAddress.cameraID);
        return splitAddress;
    }

} // TA_IRS_App
