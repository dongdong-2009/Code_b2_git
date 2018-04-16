/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/3001/transactive/app/scada/PMSAgent/src/RTUDownloadWorker.h $
  * @author:	Liang Weimin
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2012/02/06 16:15:14 $
  * Last modified by:	$Author: haijun.li $
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
