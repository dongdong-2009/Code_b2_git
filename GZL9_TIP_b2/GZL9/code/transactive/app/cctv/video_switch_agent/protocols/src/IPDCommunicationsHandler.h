#if !defined(IPDCommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
#define IPDCommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/src/IPDCommunicationsHandler.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the operations for a CommunicationsHandler.  
  * This interface is used by the CommunicationsHandler class. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#include <string>

namespace TA_IRS_App
{
    class IPDCommunicationsHandler
    {
    public:

        /**
          * ~IPDCommunicationsHandler
          *
          * Standard destructor.
          */

        virtual ~IPDCommunicationsHandler() {};

        ///////////////////////////////////////////////////////////////////////
        //
        // IPDCommunicationsHandler Methods to be overridden
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * connect
          *
          * Connect to the switching hardware.  This object is responsible for
          * submitting/closing alarms relating to the connection status.
          *
          * Once the connection has been made, this object must request the
          * state of all objects from the hardware.
          */

        virtual void connect() = 0;

        /**
          * disconnect
          *
          * Disconnect from the switching hardware.
          */

        virtual void disconnect() = 0;

		virtual void setControlPrivilege(int privilege) {}

    }; // IPDCommunicationsHandler

} // TA_IRS_App
    
#endif // !defined(IPDCommunicationsHandler_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
