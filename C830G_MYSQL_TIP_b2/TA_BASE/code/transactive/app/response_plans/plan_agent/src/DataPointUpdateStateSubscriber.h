#if !defined(DataPointUpdateStateSubscriber_PlanAgent)
#define DataPointUpdateStateSubscriber_PlanAgent

/**
  * DataPointUpdateStateSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/DataPointUpdateStateSubscriber.h $
  * @author:  ismard
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/01/08 11:35:48 $
  * Last modified by:  $Author: builder $
  * 
  * This class represents a subscriber that listens for data point update events. 
  * When a data point value has changed, this object will send appropriate Audit Messages
  * to be displaed in Event Viewer
  *
  */

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_App
{
	 //	
	 // DataPointUpdateStateSubscriber class
    //
    class DataPointUpdateStateSubscriber 
        : public TA_Base_Bus::IEntityUpdateEventProcessor
    {

    public:

		  static DataPointUpdateStateSubscriber& getInstance();
        static void destroyInstance();

        /**
          * DataPointUpdateStateSubscriber
          * 
          * Constructor.
          *
          */
        DataPointUpdateStateSubscriber();

        /**
          * ~DataPointUpdateStateSubscriber
          * 
          * Destructor.
          *
          */
        virtual ~DataPointUpdateStateSubscriber();

        /**
          * processEntityUpdateEvent
          * 
          * Receives and processes the message.
          *
          * @param message The message
          *
          */
        virtual void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

        /**
          * subscribeToMessage
          * 
          * Subscribes to the DataPointStateUpdate messages.
          *
          */
        virtual void subscribeToMessages(unsigned long entityKey, std::string sessionID, 
			  TA_Base_Core::ActivePlanId planID);

        /**
          * unsubscribeToMessage
          * 
          * Unsubscribes to the DataPointStateUpdate messages.
          *
          */
        virtual void unsubscribeToMessages();
			

    private:
		  
		  DataPointUpdateStateSubscriber(const DataPointUpdateStateSubscriber& theSubscriber);
        DataPointUpdateStateSubscriber& operator=(const DataPointUpdateStateSubscriber &);
	 
	 private:

		 static DataPointUpdateStateSubscriber *s_instance;
		 static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;
		 
		 struct ActiveBRC
		 {
			 TA_Base_Core::ActivePlanId planID; //the planID of a GRC
			 unsigned long entityKey;
			 std::string   sessionID;
		 };

		 std::vector <ActiveBRC> m_activeBRC;//need to remember these for each BRC step
														 //as the callback is asynchronous

         std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*> m_proxyMap;

    };
}

#endif // !defined(DataPointUpdateStateSubscriber_PlanAgent)
