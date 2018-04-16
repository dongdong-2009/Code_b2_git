/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/LogActiveStep.h $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/01/21 11:19:32 $
  * Last modified by:  $Author: huangjian $
  *
  **/

#pragma once

#include "app/response_plans/plan_mft_processor/src/ActivePlanStep.h"

namespace TA_Base_App
{
    class CLogEntryDlg;

    class LogActiveStep: public TA_Base_App::ActivePlanStep
    {
        // Operations
    public:
        LogActiveStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);

        // ActivePlanStep interface
        virtual void execute();
        virtual void stop();

        virtual ~LogActiveStep();

    protected:
        virtual void launch();

    private:
        TA_Base_App::CLogEntryDlg* m_pLogEntryDlg;
    };

}
