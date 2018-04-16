/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AbstractJobInstance.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This abstract class represents an instance of an executable job.
  * It implements the getJob and getMinuteStarted methods of the
  * IJobInstance interface.
  *
  */



#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"

#include "app/scheduling/scheduling_agent/src/AbstractJobInstance.h"



namespace TA_Base_App
{



IJob* AbstractJobInstance::getJob() 
{
    return m_job;
}

unsigned long AbstractJobInstance::getMinuteStarted() const
{
    return m_minuteStarted;
}

AbstractJobInstance::~AbstractJobInstance()
{
}

AbstractJobInstance::AbstractJobInstance( const IJob* job, const unsigned long minuteStarted)
    :
    m_job(const_cast<IJob*>(job)),
    m_minuteStarted(minuteStarted)
{
}


}; // TA_Base_App
