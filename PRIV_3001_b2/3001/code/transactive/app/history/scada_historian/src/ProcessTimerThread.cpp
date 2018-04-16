/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/scada_historian/src/ProcessTimerThread.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#include <algorithm>
#include <time.h>
#include "app/history/scada_historian/src/ProcessTimerThread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


using TA_Core::DebugUtil;
using TA_Core::Thread;


namespace // unamed namespace
{
	//
	// compMin
	// 
	// Comparison function which finds the HistoryConfig object
	// with the next (lowest) process time.
	//
    bool compMin(TA_App::HistoryConfig* hc1, TA_App::HistoryConfig* hc2)
    {   
        return (hc1->getNextProcessTime() < hc2->getNextProcessTime());
    }
}


namespace TA_App
{
	//
	// run
	//
	void ProcessTimerThread::run()
	{
		LOG(SourceInfo, DebugUtil::FunctionEntry, "run");

		TA_ASSERT(m_dataCollectionCfg.size() != 0, "History Configs have not been loaded");

        m_threadStarted = true;

		time_t timeNow;
		time(&timeNow);

		time_t processTime = timeNow;

		//
		// Begin processing
		//
		while(!m_terminated)
		{
			//
			// Execute process() call if it is time. callProcedures()
			// will determine whether it is time to process for each
			// HistoryConfig. If it is time, then process() is called.
			//
			clock_t start = clock();
			callProcedures(processTime);
			clock_t finish = clock();

			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"Duration of callProcedures() = %.4f milllisecs", 
				((double)(finish - start) / CLOCKS_PER_SEC)*1000);

			//
			// Get the smallest of the next process times
			//
			std::vector<HistoryConfig*>::iterator hcIt1 = m_dataCollectionCfg.begin();
			std::vector<HistoryConfig*>::iterator hcIt2 = m_dataCollectionCfg.end();
			std::vector<HistoryConfig*>::iterator hcItMin;
       
			hcItMin = std::min_element(hcIt1, hcIt2, compMin);

            time(&timeNow);

			//
			// Sleep until it is time for the next procedure call
			//

			processTime = (*hcItMin)->getNextProcessTime();
			int sleepTimeMilli = (processTime - timeNow) * 1000;

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
				"Sleeping for %lu millisecss", sleepTimeMilli);

			//
			// If it is past the process time, then process now,
			// otherwise sleep
			//
			if(sleepTimeMilli >= 0 && !m_terminated)
			{
                for(int i = 0; (i < sleepTimeMilli && !m_terminated); i+=(sleepTimeMilli/1000))
                {
				    Thread::sleep(sleepTimeMilli/1000);
                }

				time(&timeNow);

				while( (timeNow < processTime) && (!m_terminated) )
				{
					Thread::sleep(1000);
					time(&timeNow);
				}	
			}
		}

        LOG(SourceInfo, DebugUtil::FunctionExit, "run");
	}


	// 
	// terminate
	//
	void ProcessTimerThread::terminate()
	{
        LOG(SourceInfo, DebugUtil::FunctionEntry, "terminate");

		// Do not delete the HistoryConfig pointers here because they are
        // pointers to the IEntities stored in GenericAgent. When Generic
        // Agents cleans up, it will delete them. If they are deleted here,
        // CORBA will crash upon shutdown.

		m_terminated = true;

        LOG(SourceInfo, DebugUtil::FunctionExit, "terminate");
	}


	//
	// addHistoryConfig
	//
	void ProcessTimerThread::addHistoryConfig(HistoryConfig* historyCfg)
	{
		LOG(SourceInfo, DebugUtil::FunctionEntry, "addHistoryConfig");

		TA_ASSERT(historyCfg != NULL, "History Config is NULL");

		//
		// The History Configs are added in order of dependencies.
		// Each History Config calls some sort of database procedure
		// and if History Config A is based on another History Config B, 
		// History Config A will need the procedure of History Config B
		// called first as it will need to use B's data.
		// Hence the Base History Config (that is not dependent on anything)
		// will be inserted at the front, and its dependencies behind it.
		//

		std::vector<HistoryConfig*>::iterator hcIt;

		if(m_dataCollectionCfg.empty())
		{
			m_dataCollectionCfg.push_back(historyCfg);
		}
		else
		{
			for(hcIt = m_dataCollectionCfg.begin(); hcIt != m_dataCollectionCfg.end(); ++hcIt)
			{
				//
				// Try to find dependency
				//
				if(historyCfg->getBaseDataName() == (*hcIt)->getEntityName())
				{
					//
					// Found dependency, make sure element is added behind it
					//
					++hcIt;
					hcIt = m_dataCollectionCfg.insert(hcIt, historyCfg);
					break;
				}
			}

			if(hcIt == m_dataCollectionCfg.end())
			{
				//
				// Dependency wasn't found, add to front
				//
				hcIt = m_dataCollectionCfg.begin();
				hcIt = m_dataCollectionCfg.insert(hcIt, historyCfg);
			}
		}

		LOG(SourceInfo, DebugUtil::FunctionExit, "addHistoryConfig");
	}


	//
	// callProcedures
	//
	void ProcessTimerThread::callProcedures(time_t currentTime)
	{
		LOG(SourceInfo, DebugUtil::FunctionEntry, "callProcedures");
		
		//
		// Iterate through the vector of History Configs and process if it
		// is time. The History Configs should be ordered by dependency, so
		// each procedure called will be called before its dependent procedures.
		// Sleep for 1 sec between each procedure call to allow enough time
		// for the data to be inserted into the database.
		//

		std::vector<HistoryConfig*>::iterator hcIt;
        hcIt = m_dataCollectionCfg.begin();
		int timeMilliSecs = 1000;

		for(; hcIt != m_dataCollectionCfg.end() && !m_terminated; ++hcIt)
		{
			if((*hcIt)->isTimeToProcess(currentTime))
			{
                //
                // Only process (write to database and call db procedures)
                // if in Control mode
                //
                if(m_controlMode && !m_terminated)
                {
                    (*hcIt)->process(currentTime);
                }
			}

			Thread::sleep(timeMilliSecs);
		}

		LOG(SourceInfo, DebugUtil::FunctionExit, "callProcedures");
	}

} // TA_App
