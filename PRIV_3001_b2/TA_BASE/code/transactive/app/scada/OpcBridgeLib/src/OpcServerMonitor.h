/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/OpcBridgeLib/src/OpcServerMonitor.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2018/03/14 13:51:20 $
  * Last modified by : $Author: lim.cy $
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
