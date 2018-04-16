/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/PRIV_3001/3001/transactive/app/scada/PMSAgent/src/RTUDownloadWorker.h $
  * @author:	Liang Weimin
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2018/03/14 13:51:20 $
  * Last modified by:	$Author: lim.cy $
  *
  * Description:
  *
  *	The RTUDownloadWorker object is responsible for upload config file to RTU
  *
  */

#ifndef RTUDOWNLOADWORKER_H
#define RTUDOWNLOADWORKER_H

#include <string>
#include <vector>

#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/sockets/src/SocketOptions.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/Thread.h"

namespace TA_IRS_App
{
	class RTU;

	class RTUDownloadWorker : public TA_Base_Core::Thread
	{
	// operations
	public:

		RTUDownloadWorker(RTU & rtu);
                          
        virtual ~RTUDownloadWorker();

		//
		// operations required by the TA_Core::TA_Thread interface
		//
		virtual void run();
		virtual void terminate();

		void startDownload();

	private:
        // disabled constructors
		RTUDownloadWorker ( const RTUDownloadWorker & obj );
		RTUDownloadWorker & operator= ( const RTUDownloadWorker & rhs );

	// attributes
	private:

		volatile bool m_isTerminating;
		TA_Base_Core::Semaphore m_semaphore;

		RTU & m_rtu;
		
	};
}

#endif
