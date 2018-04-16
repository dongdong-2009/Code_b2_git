#if !defined(AFX_ACTIVEPLANSTEP_H__711C3829_932C_4693_B233_9AF5C6DA25DD__INCLUDED_)
#define AFX_ACTIVEPLANSTEP_H__711C3829_932C_4693_B233_9AF5C6DA25DD__INCLUDED_

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/ActivePlanStep.h $
  *  @author:  Huang Jian
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/10/30 19:47:26 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

#include "ace/Mutex.h"

#include "core/threads/src/Thread.h"

namespace TA_Base_App
{

class ActivePlanStep: public TA_Base_Core::Thread
{

public:
    ActivePlanStep( const unsigned int unId );
    virtual ~ActivePlanStep();

    bool isRunning();
    void reset();

    // TA_Base_Core::Thread interface should be implemented by concrete step
    virtual void run();
    virtual void terminate();

    virtual void execute() = 0;
    virtual void stop() = 0;

private:
    volatile ThreadState_t m_eStepState;
    ACE_Mutex m_mtxState;

protected:
    const unsigned int m_unId;
};

} // namespace TA_Base_App


#endif // !defined(AFX_ACTIVEPLANSTEP_H__711C3829_932C_4693_B233_9AF5C6DA25DD__INCLUDED_)