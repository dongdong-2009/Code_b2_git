#ifndef AA_MESSAGE_SENDER_HEADER_INCLUDED
#define AA_MESSAGE_SENDER_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/AAMessageSender.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  A thread-safe Singleton factory for updating database and adding into database.
  *  
  */
//

namespace TA_Base_App
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "AAMessageSender" and that "AAMessageSenderImpl" can
	// not be used.

    
	class AAMessageSenderImpl
	{
	// Public methods
	public:

		friend class ACE_Singleton<AAMessageSenderImpl, ACE_Recursive_Thread_Mutex>;

        void sendAuditMessage(const std::string& session, const TA_Base_Core::MessageType& auditMessageType, const TA_Base_Core::DescriptionParameters& description);

        // Returns the ID of the generated alarm
		std::string raiseAlarm(const TA_Base_Core::MessageType& alarmType, const TA_Base_Core::DescriptionParameters& description);

        void closeAlarm(const std::string& alarmId);
         
        void deleteMessageSenders(void);

	// Private methods
	private:
		AAMessageSenderImpl();
		~AAMessageSenderImpl();

		AAMessageSenderImpl( const AAMessageSenderImpl& );
		AAMessageSenderImpl& operator=( const AAMessageSenderImpl& );

    // Private data
    private:

		std::string getOperatorNameNid(const std::string& session);

		TA_Base_Core::AuditMessageSender* m_auditMessageSender;


		//TA_Base_Core::AlarmHelper* m_alarmHelper;
		//TD18093,jianghp
        //TA_Base_Core::NonUniqueAlarmHelper& m_alarmHelper; //getNonUniqueAlarmHelper()
		//TD18093,jianghp

		mutable ACE_RW_Mutex m_editPlanMapLock;

        TA_Base_Bus::AuthenticationLibrary m_authenticationLib;
	}; // AAMessageSenderImpl

	typedef ACE_Singleton<AAMessageSenderImpl, ACE_Recursive_Thread_Mutex> AAMessageSender;

} // TA_Base_App

#endif //AA_MESSAGE_SENDER_HEADER_INCLUDED
