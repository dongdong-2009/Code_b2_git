/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/app/response_plans/plan_mft_processor/src/LaunchGuiAppActiveStep.h $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2016/01/22 14:37:26 $
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