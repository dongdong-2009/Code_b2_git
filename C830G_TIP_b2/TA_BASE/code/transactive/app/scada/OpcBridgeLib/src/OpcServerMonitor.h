/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcBridgeLib/src/OpcServerMonitor.h $
  * @author  HoaVu
  * @version $Revision: #3 $
  *
  * Last modification : $DateTime: 2013/01/25 15:23:55 $
  * Last modified by : $Author: haijun.li $
  *
  *	Thread for monitoring the OPC Server
  *
  */

#ifndef OPCSERVERMONITOR_H
#define OPCSERVERMONITOR_H

#include "core/threads/src/Thread.h"

namespace TA_Base_App
{
	class OpcServerMonitor : public TA_Base_Core::Thread
	{
	public:

		OpcServerMonitor () {};


		virtual ~OpcServerMonitor () {};

		//
		// Thread derived functions to start/stop the
		// DataNodeQueueProcessorWorker thread
		//
		virtual void run();
		virtual void terminate();

	private:

		volatile bool						m_keepRunning;
	};
}

#endif
