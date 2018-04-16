#if !defined ADDRESS_HELPER_H
#define ADDRESS_HELPER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/AddressHelper.h $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Provides helper functions and a struct for handling IP addresses.
  */

#include <string>

namespace TA_IRS_App
{
    class AddressHelper
    {
    public:
        struct Address              // Used to define the address of the AddressHelper.
        {
            std::string host;       // The host name or IP address.
            std::string port;       // The port number.
        };

        /**
          * AddressHelper
          *
          * Standard constructor.
          */

        AddressHelper() {};
    
        /**
          * ~AddressHelper
          *
          * Standard destructor.
          */

        virtual ~AddressHelper() {};

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

        static Address splitAddress( const std::string& address );
          
        /**
          * validateIpAddress
          *
          * Checks that the input string is a valid IP address.
          * If the address is a hostname - true will be returned automatically.
          *
          * @param      const std::string& address 
          *             String representing the IP address to validate.
          *
          * @return     bool
          *             True if the IP address is valid.
          *             False otherwise.
          */

        static bool validateIpAddress( const std::string& address );

    private:

        //
        // Disable copy constructor and assignment operator.
        //

        AddressHelper( const AddressHelper& theAddressHelper );
        AddressHelper& operator=( const AddressHelper& theAddressHelper );
    };

} // TA_IRS_App

#endif // ADDRESS_HELPER_H
