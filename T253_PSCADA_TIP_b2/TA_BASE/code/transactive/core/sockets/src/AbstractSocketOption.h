/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/sockets/src/AbstractSocketOption.h $
  * @author:  A. Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * An abstract representation of a socket option.
  * Each socket option will be able to apply its changes to each platform
  * although each option may not expect the same datatype as a parameter.
  *
  * For possible options see:
  * man socket and man setsockopt (socket level options)
  * man tcp (tcp options)
  * man ip (ip options)
  * http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winsock/winsock/socket_options.asp
  */

#if !defined(ABSTRACT_SOCKET_OPTION_H)
#define ABSTRACT_SOCKET_OPTION_H

#include <string>

namespace TA_Base_Core
{

    class AbstractSocketOption
    {

    public:
        AbstractSocketOption() {};
        virtual ~AbstractSocketOption() {};

        /**
         * Apply the changes to the given socket ID
         * Uses setsockopt, the socket ID, and any necessary parameters
         * to set the socket option.
         *
         * @param socketID  The ID of the socket to set the options on
         */
	    virtual void apply(int socketId) const = 0;

        /**
         * Copy this object (so it can be passed by value)
         */
        virtual AbstractSocketOption* clone() const = 0;
    };

}

#endif // !defined(ABSTRACT_SOCKET_OPTION_H)
