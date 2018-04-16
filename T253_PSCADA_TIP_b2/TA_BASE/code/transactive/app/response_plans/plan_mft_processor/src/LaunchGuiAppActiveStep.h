/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/LaunchGuiAppActiveStep.h $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  **/

#ifndef LAUNCHGUIAPPATICACTIVESTEP_H
#define LAUNCHGUIAPPATICACTIVESTEP_H

#include "app/response_plans/plan_mft_processor/src/ActivePlanStep.h"


namespace TA_Base_App
{

    class LaunchGuiAppActiveStep : public TA_Base_App::ActivePlanStep
    {
        // Operations
    public:
        LaunchGuiAppActiveStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);

        // ActivePlanStep interface
        virtual void execute();
        virtual void stop();

        virtual ~LaunchGuiAppActiveStep();

    private:
        void launch();
    };

}

#endif //LAUNCHGUIAPPATICACTIVESTEP_H