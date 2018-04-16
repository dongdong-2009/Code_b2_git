/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/JobThread.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/06/25 12:06:55 $
  * Last modified by:  $Author: builder $
  *
  * Runs an instance of a job.
  *
  */


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/scheduling/scheduling_agent/src/JobThread.h"
#include "app/scheduling/scheduling_agent/src/JobRunner.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceExecutionException.h"
#include "app/scheduling/scheduling_agent/src/IJob.h"
#include "core/corba/src/CorbaUtil.h"
#include <string>


using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{



JobThread::JobThread(const IJobInstance* jobInstance)
{
    m_jobInstance = const_cast<IJobInstance*>(jobInstance);
}


JobThread::~JobThread() 
{
}


IJobInstance* JobThread::getJobInstance()
{
    return m_jobInstance;
}


void JobThread::run()
{
    std::string errorMessage = "";

    try
    {
        m_jobInstance->execute();
    }
    catch(const JobInstanceExecutionException& jiee)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "JobInstanceExecutionException",
                            jiee.what());

        errorMessage = jiee.what();
    }
	catch(const TA_Base_Core::TransactiveException& ex)// Wuzhongyi CL-21357
	{
		errorMessage = "JobThread::run exception : "; 
		errorMessage += ex.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", errorMessage.c_str());
	}
	catch(const CORBA::Exception& ex)
	{
		errorMessage = "JobThread::run exception : "; 
		errorMessage += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", errorMessage.c_str());
	}
    catch(...)
    {
		// Wuzhongyi CL-21357
		errorMessage = "Unknown exception caught by JobThread";
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", errorMessage.c_str());
    }

    try
    {
        JobRunner::getInstance().jobCompleted(this, errorMessage);
    }
	catch(const TA_Base_Core::TransactiveException& ex)// Wuzhongyi CL-21357
	{
		std::string strErrMsg("Exception caught in JobThread run method : ");
		strErrMsg += ex.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", strErrMsg.c_str());
	}
    catch(...)
    {
        LOG_GENERIC(
            SourceInfo, DebugUtil::DebugError,
            "Exception caught in JobThread run method.");
    }
}


void JobThread::terminate()
{
}



}; // TA_Base_App

