#if !defined(SystemControllerGroupUpdateSubscriber_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
#define SystemControllerGroupUpdateSubscriber_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

/**
  * SystemControllerGroupUpdateSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/generic_gui/src/SystemControllerGroupUpdateSubscriber.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * This class implements represents a subscriber that listens specifically for 
  * SystemControllerComms::SystemControllerGroupOffline and SystemControllerComms::SystemControllerGroupOnline
  * events. When an event is received and it corresponds to a group specified by the command line parameter,
  * --central-server-group (eg. --central-server-group=OCC) this object will notify the observer of
  * whether the system is in degraded or normal mode.
  *
  */

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

namespace TA_Base_Bus
{
    //
    // Forward declarations
    //
    class ISystemStateUpdateObserver;
}

namespace TA_Base_Bus
{
    //
    // SystemControllerGroupUpdateSubscriber class
    //
    class SystemControllerGroupUpdateSubscriber 
        : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {

    public:

        /**
          * SystemControllerGroupUpdateSubscriber
          * 
          * Constructor.
          *
          */
        SystemControllerGroupUpdateSubscriber(TA_Base_Bus::ISystemStateUpdateObserver& systemStateUpdateObserver);

        /**
          * ~SystemControllerGroupUpdateSubscriber
          * 
          * Destructor.
          *
          */
        virtual ~SystemControllerGroupUpdateSubscriber();

        /**
          * receiveSpecialisedMessage
          * 
          * Receives and processes the message.
          *
          * @param message The message
          *
          */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

        /**
          * startNotifications
          * 
          * Subscribes to the messages. Once this method is called, messages will be received.
          *
          */
        void startNotifications();
        
        /**
          * getCentralServerGroupName
          * 
          * Returns the name of the central server group.
          *
          * @return Central server group name
          *
          */
        std::string getCentralServerGroupName() const
        {
            return m_centralServerGroupName;
        }

    private:

        SystemControllerGroupUpdateSubscriber(const SystemControllerGroupUpdateSubscriber& theSubscriber);
        SystemControllerGroupUpdateSubscriber& operator=(const SystemControllerGroupUpdateSubscriber &);

        TA_Base_Bus::ISystemStateUpdateObserver& m_systemStateUpdateObserver;

        std::string m_centralServerGroupName;

        static const std::string CENTRAL_SERVER_GROUP;
        static const std::string RPARAM_CENTRAL_SERVER_GROUP;
		
		//TD16358++
		static const std::string GROUP_ONLINE;


		// The name of the system controller that we're connected to
		std::string m_systemControllerName;
		volatile bool m_sCGroupOnline;
		
		//++TD16358 
    };
}

#endif // !defined(SystemControllerGroupUpdateSubscriber_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
