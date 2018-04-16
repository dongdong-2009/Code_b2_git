/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/PIDSSigDisplayRequest.h $
  * @author:   huang.wenbo
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
**/


#ifndef PIDSSIGDISPLAYREQUEST_H
#define PIDSSIGDISPLAYREQUEST_H

#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/signs/pids_agent/IDL/src/IPIDSManagerCorbaDef.h"
#include "app/signs/pids_agent/src/PIDSMessage.h"
namespace TA_IRS_App
{
	class PIDSSigDisplayRequest:public TA_IRS_App::PIDSMessage
	{
	public:

		PIDSSigDisplayRequest(){};

		~PIDSSigDisplayRequest(){};

		virtual void log(bool sendSuccess){};	

		virtual TA_Base_Bus::DataBlock<WORD> createProtocolMessage();

		void setData(const TA_Base_Bus::IPIDSManagerCorbaDef::SigData& data);

		static const int SIGINFO_FEP_REGISTER_ADDRESS;

	private:		
		TA_Base_Bus::IPIDSManagerCorbaDef::SigData m_data;
	};
}

#endif
