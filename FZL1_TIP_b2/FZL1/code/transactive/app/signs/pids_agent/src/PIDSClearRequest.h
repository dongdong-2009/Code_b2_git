#ifndef PIDSCLEARREQUEST_H_INCLUDED
#define PIDSCLEARREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/PIDSClearRequest.h $
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

	class PIDSClearRequest : public PIDSMessage
	{
	public:

		PIDSClearRequest();

		~PIDSClearRequest();

		virtual TA_Base_Bus::DataBlock<WORD> createProtocolMessage();

		virtual void log(bool sendSuccess);

		void setData(TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList destinationList, ::CORBA::Long trainNo);

	private:

		TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList m_destinationList;
		int m_trainNo; 
	};

}

#endif
