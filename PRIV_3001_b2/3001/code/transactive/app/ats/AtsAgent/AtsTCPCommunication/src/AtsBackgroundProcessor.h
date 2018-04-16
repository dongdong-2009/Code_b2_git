
#ifndef _ATS_BACKGROUND_PROCESSOR_H
#define _ATS_BACKGROUND_PROCESSOR_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/tvss_agent/src/TVSSBackgroundProcessor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  * The TVSSBackgroundProcessor is a class that allows for performing
  * RadioTasks in a side thread off the main line of execution.  This is to
  * enable, for instance, asynchonous processing of unsolicited events that
  * require further information from the TcpServer 
  */

namespace TA_Base_Core
{
	class ThreadPoolSingletonManager;
}

namespace TA_IRS_App
{
	class AtsTcpServerWrapper;
	
	class AtsBackgroundProcessor
	{
	public:
		AtsBackgroundProcessor(AtsTcpServerWrapper* ats);
		virtual ~AtsBackgroundProcessor();

	private:

		AtsTcpServerWrapper& m_atsTcpServerWrapper;
		TA_Base_Core::ThreadPoolSingletonManager* m_threadPool;

	};
};

#endif // end of _ATS_BACKGROUND_PROCESSOR_H