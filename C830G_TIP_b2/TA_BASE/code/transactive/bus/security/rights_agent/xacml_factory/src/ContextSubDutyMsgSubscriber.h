#ifndef CONTEXTSUBDUTYMSGSUBSCRIBER_H_INCLUDED
#define CONTEXTSUBDUTYMSGSUBSCRIBER_H_INCLUDED

/**
  * ContextSubDutyMsgSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  G. Kirkpatrick
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This class listens for the Duty Agent comms message containing updates for subsystem duty.
  *
  */

namespace TA_Base_Bus
{
    class ContextSubDutyMsgSubscriber : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
										public TA_Base_Core::AbstractThreadedTimeoutUser
    {
    public:

        ContextSubDutyMsgSubscriber( RightsType::LocSubDutyMap& locSubDutyMap,
			ACE_RW_Mutex& locSubDutyLock );

		~ContextSubDutyMsgSubscriber();

        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

		//
		// operations required by the AbstractThreadedTimeoutUser class
		//
		
		
		/**
		*
		* threadedTimerExpired
		*
		* To be implemented by users of TimerUtil.  This operation will be called
		* when the registered timeout period has expired.
		*
		*/
		virtual void threadedTimerExpired(long timerId, void* userData);

    private:

        ContextSubDutyMsgSubscriber(const ContextSubDutyMsgSubscriber&);
	    ContextSubDutyMsgSubscriber& operator=(const ContextSubDutyMsgSubscriber &);

		void update(const DutyAgentTypeCorbaDef::SubsystemDutySequence& update, bool isAdd );

		void accumulateUpdates(TA_Base_Bus::DutyAgentTypeCorbaDef::RightsLibraryUpdateSequence& updates);

		RightsType::LocSubDutyMap& m_locSubDutyMap;
		ACE_RW_Mutex& m_locSubDutyLock;

		const unsigned long MAX_LSDM_SIZE;

		std::auto_ptr<TA_Base_Core::CommsMessageSender> m_commsSender;
		unsigned long m_locationKey;
		std::string m_locationStr;

		TA_Base_Bus::DutyAgentTypeCorbaDef::VersionedRightsLibraryUpdate * m_pUpdate;		
			
		ACE_Mutex m_mutex;
		CORBA::ULong m_version;
    };

} // TA_Base_App

#endif // CONTEXTSUBDUTYMSGSUBSCRIBER_H_INCLUDED





