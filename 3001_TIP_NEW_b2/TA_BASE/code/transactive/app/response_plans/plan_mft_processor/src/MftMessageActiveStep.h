/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/MftMessageActiveStep.h $
  *  @author:  Huang Jian
  *  @version: $Revision: #13 $
  *
  *  Last modification: $DateTime: 2014/03/14 15:03:32 $
  *  Last modified by:  $Author: huangjian $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

#ifndef MFTMESSAGEACTIVESTEP_H
#define MFTMESSAGEACTIVESTEP_H

#include "app/response_plans/plan_mft_processor/src/ActivePlanStep.h"

namespace TA_Base_Bus
{
    class MessageDialogHelper;
}

namespace TA_Base_App
{

class MftMessageActiveStep : public TA_Base_App::ActivePlanStep
{
    // Operations
public:
    MftMessageActiveStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);

    // ActivePlanStep interface
    virtual void execute();
    virtual void stop();

    virtual ~MftMessageActiveStep();
private:
    void launch();

    TA_Base_Bus::MessageDialogHelper* m_pMessageDialog;
};

}

#endif //MFTMESSAGEACTIVESTEP_H