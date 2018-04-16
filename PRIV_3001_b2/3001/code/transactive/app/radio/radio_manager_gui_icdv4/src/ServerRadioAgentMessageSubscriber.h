/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/ServerRadioAgentMessageSubscriber.h $
  * @author:  Glen Kidd
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#if !defined(AFX_SERVERRADIOAGENTMESSAGESUBSCRIBER_H__982F9269_BEE3_48DD_94B7_8C7CDF975045__INCLUDED_)
#define AFX_SERVERRADIOAGENTMESSAGESUBSCRIBER_H__982F9269_BEE3_48DD_94B7_8C7CDF975045__INCLUDED_


#include "app/radio/radio_manager_gui_icdv4/src/IRadioAgentMessageListener.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "core/message/src/GenericSubscriberPublisher.h"
#include "core/message/idl/src/CommsMessageCorbaDef.h"
#include "core/process_management/src/CtaRuntime.h"
#include "core/message/src/CommsMessageSender.h"


class ServerRadioAgentMessageSubscriber : public GenericSubscriberPublisher<TA_Base_Core::CommsMessageCorbaDef>  
{
	public:
		
		ServerRadioAgentMessageSubscriber(TA_Base_Core::CtaRuntime* pCtaRuntime);

		virtual ~ServerRadioAgentMessageSubscriber();

		/**
		 * Allows classes implementing the IRadioAgentMessageListener interface to be notified of
		 * SDS messages. This calls addGenericListener() in the base class.
		 * @param listener
		 */
		void addListener(IRadioAgentMessageListener* listener);

		virtual void Subscribe();

		virtual void ExtractMessageData(const TA_Base_Core::CommsMessageCorbaDef& messageData);

		virtual void PublishToListener(const TA_Base_Core::CommsMessageCorbaDef& messageData, IGenericObserverListener* listener);

	private:

		// disabled methods
		ServerRadioAgentMessageSubscriber(const ServerRadioAgentMessageSubscriber&);
		void operator=(const ServerRadioAgentMessageSubscriber&);	


		void sendNotification(MessageInbox* message);

		
	private:

		RadioResource m_source;

		std::string m_message;

		TA_Base_Core::CtaRuntime* m_pCtaRuntime;

		std::string m_mftName;

		unsigned long m_mftentityKey;
		unsigned long m_mftSubsystem;
		unsigned long m_mftLocation;

		TA_Base_Core::CommsMessageSender* m_commsSender;
};

#endif 
