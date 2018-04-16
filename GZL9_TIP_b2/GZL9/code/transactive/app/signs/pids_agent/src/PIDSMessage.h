#ifndef PIDSMESSAGE_H_INCLUDED
#define PIDSMESSAGE_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSMessage.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include <vector>
#include <string>

// IDL compiler-produced header file
//#include "core/data_access_interface/Pids_agent_4669/IDL/src/PIDSMessageDataDefinitions.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/modbus_comms/src/DataBlock.h"

namespace TA_IRS_App
{
	class PIDSMessage
	{
	public:
		/**
		  * constructor
		  *
		  * @return     nothing.
		  *
		  */
		PIDSMessage();

		/**
		  * StatusMonitor
		  *
		  * Destructor
		  *
		  * @return     none.
		  *
		  */
		virtual ~PIDSMessage();

		// Other methods

		virtual TA_Base_Bus::DataBlock<WORD> createProtocolMessage() = 0;

		virtual void log(bool sendSuccess) = 0;

		void setData();

		// Indicates whether the data members have been set
		bool m_dataIsSet;

		// e.g 'OCC', 'DBG' 
		std::string m_location;

	private:
		TA_Base_Bus::DataBlock<WORD> m_dataBlock;
	};

}

#endif // STATUSMONITOR_H_INCLUDED