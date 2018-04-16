#ifndef PIDSCOMMUNICATIONSHANDLER_H_INCLUDED
#define PIDSCOMMUNICATIONSHANDLER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSCommunicationsHandler.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "core/sockets/src/TcpNonblockingSocket.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/alarm/src/AlarmHelperManager.h"

#include "bus/scada/ModbusHelper/src/ModbusHelperFactory.h"
#include "app/signs/pids_agent/src/PIDSMessage.h"

namespace TA_Base_Bus
{
	class IModbusHelper;
}

namespace TA_IRS_App
{
	
	class PIDSCommunicationsHandler
	{
	public:
		/**
		* getInstance
		*
		* Returns an instance of the class
		*
		* @return the pointer to this singleton instance
		*
		*/
		static PIDSCommunicationsHandler* getInstance();

		/**
		* removeInstance
		*
		* Removes the instance of the class (if already created) and cleanup the members.  Primarily 
		* used upon program termination (e.g. from main()) so that Purify does not consider this class
		* and its members to be leaks.
		*
		*/
		static void removeInstance();

		PIDSCommunicationsHandler();

		~PIDSCommunicationsHandler();

		void initialise(unsigned long agentLocation);

		//throw PIDSServerNotConnectedException
		short sendMessage( PIDSMessage* data ,bool isSigMsg = false, WORD startAddress = 1);		//return 0 means successfully, other means fail

	protected:
		static PIDSCommunicationsHandler*                        m_instance;
		static TA_Base_Core::ReEntrantThreadLockable             m_lock;

	private:

//		TA_Base_Core::ReEntrantThreadLockable    m_writeLock;   // Ensure only one thread writes at a time.
//		bool m_connected;   // Is the socket connected to the remote host

		TA_Base_Core::AlarmHelper* m_alarmHelper;

		boost::shared_ptr<TA_Base_Bus::IModbusHelper>            m_pModbusHelper;

		unsigned long m_locationKey;

		void logDataBlockToBinaryString( int nFileLine, TA_Base_Bus::DataBlock<WORD>& dbwToBeLoged );
	};

}

#endif
