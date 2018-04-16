/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * This class creates objects that are used to read and change rows in the
  * ScheduledJob database table.
  *
  */


#include "core/synchronisation/src/ThreadGuard.h"

#include "core/data_access_interface/scheduling_agent/src/JobDataAccessFactory.h"
#include "core/data_access_interface/scheduling_agent/src/ScheduleDataAccessFactory.h"
#include "core/data_access_interface/scheduling_agent/src/ConfigJobData.h"

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated to '255' characters in the debug
#endif // _MSC_VER

namespace TA_Base_Core
{


JobDataAccessFactory* JobDataAccessFactory::m_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable JobDataAccessFactory::m_singletonLock;


JobDataAccessFactory& JobDataAccessFactory::getInstance()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);
    
    if(m_instance == NULL)
    {
        m_instance = new JobDataAccessFactory();
    }
    
    return *m_instance;
}


JobDataAccessFactory::JobDataAccessFactory()
{
}


JobDataAccessFactory::~JobDataAccessFactory()
{
}



IConfigJobData* JobDataAccessFactory::getJob(const std::string& id)
{
    return new ConfigJobData(id);
}


JobDataAccessFactory::IConfigJobDataVector JobDataAccessFactory::getAllJobs()
{
    return ConfigJobData::getAllJobs();
}


JobDataAccessFactory::IConfigJobDataVector
    JobDataAccessFactory::getAllJobsOfType(const std::string& jobType)
{
    return ConfigJobData::getAllJobsOfType(jobType);
}


IConfigJobData* JobDataAccessFactory::createJob()
{
    return new ConfigJobData();
}


};// TA_Base_Core
