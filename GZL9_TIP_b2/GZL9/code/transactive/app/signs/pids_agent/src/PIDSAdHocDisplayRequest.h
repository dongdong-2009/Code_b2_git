#ifndef PIDSADHOCDISPLAYREQUEST_H_INCLUDED
#define PIDSADHOCDISPLAYREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSAdHocDisplayRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "app/signs/pids_agent/src/PIDSMessage.h"
#include "bus/signs/pids_agent/IDL/src/IPIDSManagerCorbaDef.h"

// IDL compiler-produced header file
//#include "core/data_access_interface/Pids_agent_4669/IDL/src/PIDSMessageDataDefinitions.h"

namespace TA_IRS_App
{

	class PIDSAdHocDisplayRequest : public PIDSMessage
	{
	public:

		PIDSAdHocDisplayRequest();

		~PIDSAdHocDisplayRequest();

		//hongran++ TD17502
		virtual void log(bool sendSuccess);
		//++hongran TD17502

		virtual TA_Base_Bus::DataBlock<WORD> createProtocolMessage();

		void setData(TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList destinationList, 
			TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage messageContent, 
			::CORBA::Boolean msgType, 
			::CORBA::Boolean showType, 
			::CORBA::Long trainNo, 
			const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& startTime, 
			const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& endTime, 
			::CORBA::Long interval, 
			::CORBA::Boolean isCyclic);

	private:


		/**
		  * ledFontToString
		  *
		  * Converts the led font related data members to a string
		  * to be added to the protocol message
		  *
		  * @return     std::string
		  *             The string containing the led attributes
		  */
//		std::string getLedFontAttributesAsString();

		// The actual ad-hoc message content
//		std::string m_message;

		// Where the message is to be displayed
//        TA_Base_Core::PIDSDestination m_destination;

		// Unused at present
//		int m_tag;

		TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList m_destinationList;
		TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage m_messageContent;
		
		bool m_msgType;
		bool m_showType; 
		int m_trainNo; 

		TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime m_startTime;

		TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime m_endTime;

		long m_interval;
		bool m_isCyclic;
	};

}

#endif
