/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/AddressHelper.cpp $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Provides helper functions and a struct for handling IP addresses.
  */

#ifndef __WIN32__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#endif

#include "app/cctv/video_switch_agent/protocols/COE/src/AddressHelper.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    //
    // splitAddress
    //
    AddressHelper::Address AddressHelper::splitAddress( const std::string& address )
    {
        Address splitAddress;
        splitAddress.host = "";
        splitAddress.port = "";

        // Could be of the form address:port or just address

        std::string::size_type col = address.find_first_of( ":", 0 );

        std::string ip;
        if ( col != address.size() - 1 )
        {
            // address:port

            splitAddress.host = address.substr( 0, col );
            splitAddress.port = address.substr( col + 1, std::string::npos );
        }
        else
        {
            // Just address

            splitAddress.host = address;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "Host: %s, Port: %s", splitAddress.host.c_str(), splitAddress.port.c_str() );

        return splitAddress;
    }


    //
    // validateIpAddress
    //
    bool AddressHelper::validateIpAddress( const std::string& address )
    {
        // Attempt to turn the ipaddress into an unsigned int ipaddress
        // If that worked, everything is fine

        unsigned long inetAdd = inet_addr( address.c_str() );
        if ( inetAdd != (unsigned long)-1 )
        {
            return true;
        }
        else
        {
            struct hostent *hostEnt;
            hostEnt = gethostbyname( address.c_str() );
            if ( NULL != hostEnt )
            {
                return true;
            }
        }

        return false;
    }

} // TA_IRS_App
