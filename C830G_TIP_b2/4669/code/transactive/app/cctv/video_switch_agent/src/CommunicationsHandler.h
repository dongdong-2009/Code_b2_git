#if !defined COMMUNICATIONS_HANDLER_H
#define COMMUNICATIONS_HANDLER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/CommunicationsHandler.h $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Handles communications with the hardware.
  */

#include <string>

#include "app/cctv/video_switch_agent/protocols/src/IPDCommunicationsHandler.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"

// Forward declarations.

namespace TA_Base_Core
{
    class ConfigUpdateDetails;
}

namespace TA_IRS_App
{
    class CommunicationsHandler
    {
    public:
        /**
          * CommunicationsHandler
          *
          * Standard constructor.
          *
          * @exception  CommunicationsHandlerAgentException
          *             Thrown if the protocol dependent initialisation fails.
          */

        CommunicationsHandler( TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData );

        /**
          * ~CommunicationsHandler
          *
          * Standard destructor.
          */

        virtual ~CommunicationsHandler() {};

        /**
          * connect
          *
          * Connect to the hardware (if the agent is in Control mode).
          *
          * Once the connection has been made, the protocol dependent object 
          * must request the state of all objects from the hardware.
          */

        void connect();

        /**
          * disconnect
          *
          * Disconnect from the hardware.
          */

        void disconnect();

    private:
        //
        // Disable copy constructor and assignment operator
        //

        CommunicationsHandler( const CommunicationsHandler& theCommunicationsHandler );
        CommunicationsHandler& operator=( const CommunicationsHandler& theCommunicationsHandler );

        //
        // The object that handles the protocol dependent tasks for this object.
        //

        std::auto_ptr< IPDCommunicationsHandler > m_pdCommunicationsHandler;
    };

} // TA_IRS_App

#endif // COMMUNICATIONS_HANDLER_H
