#if !defined ADDRESS_HELPER_H
#define ADDRESS_HELPER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/FZL1_TIP/FZL1/transactive/app/cctv/video_display_manager/src/AddressHelper.h $
  * @author:   zhang hongzhi
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/05/08 16:15:14 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * Provides helper functions and a struct for handling camera addresses.
  */

#include <string>
#include <boost/noncopyable.hpp>

namespace TA_IRS_App
{
    class AddressHelper : boost::noncopyable
    {
        public:
            struct Address              // Used to define the address of the AddressHelper.
            {
                unsigned short stationID;       // The host name or IP address.
                unsigned short cameraID;       // The port number.
            };

            /**
              * AddressHelper
              *
              * Standard constructor.
              */

            AddressHelper();

            /**
              * ~AddressHelper
              *
              * Standard destructor.
              */

            virtual ~AddressHelper();

            // The standard copy constructor and assignment operators are fine.

            /**
              * splitAddress
              *
              * Splits the address into host ipaddress/name and port number.
              *
              * @return     Address
              *             The split up address.  Port will be "" if no
              *             port is provided.
              *
              * @param      const std::string& address
              *             The address to split in the form:
              *             1. ipaddress:port
              *             2. hostname:port
              *             3. ipaddress
              *             4. hostname
              */

            static Address splitAddress(const std::string& address);
    };

} // TA_IRS_App

#endif // ADDRESS_HELPER_H
