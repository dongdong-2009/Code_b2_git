/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/active_ml/src/MLActiveStep.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/22 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Base class for ML Algorithms' step classes.
*/


#ifndef MLACTIVESTEP_H
#define MLACTIVESTEP_H

#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/active_ml/src/MLStepParameter.h"

namespace TA_Base_App
{

    //Class declaration
    class MLActiveStep : public ActiveStep
    {
        // Public methods
    public:
        MLActiveStep(BaseStepParameter& refParameter, IActivePlan& flowControl);
		virtual ~MLActiveStep();

    protected:
		virtual void initImpl();
		virtual bool executeImpl() = 0;
		//virtual int waitImpl();
	
	private:
		MLActiveStep(const MLActiveStep&);
		MLActiveStep& operator=(const MLActiveStep&);

    }; // MLActiveStep

}; // TA_Base_App

#endif // MLACTIVESTEP_H