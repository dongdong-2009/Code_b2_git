/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveTrainTisStep.cpp $
  *  @author:  Bart Golab
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Represents a Train Traveller Info System step in an executing plan.
  */

#include "PlanAgent.h"

#if defined ( STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM )


// namespace alias for Corba structures/types

namespace TA_Base_App
{

    ActiveTrainTisStep::ActiveTrainTisStep(const TA_Base_Core::StepDetail& stepDetail,
                                           const TA_Base_Core::TrainTravellerInfoSystemParameters& params,
                                           IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveTrainTisStep::executeImpl( void )
	{
        // TODO: BartG 23/6/05
        //       TD6840 - to be implemented when the TTIS subsystem has been extended to meet all the requirements of this step.
        //
        // The following issues have been raised with the Visual team:
        // 1. It is not possible to target a message at inbound/outbound/all trains. This was raised as a requirement with 
        //    Craig Grant at the beginning of 2004 and he committed to extending the ITTISManagerCorbaDef interface to 
        //    accomodate this. It makes sense to have this in the agent, as opposed to every client generating the respective 
        //    train lists.
        // 2. Apparently, a TTIS request may take quite a while to complete if the train does not respond. For a group of 
        //    trains this will be even longer, so it might be a good idea to add support for some asynchronous calls... 

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Train TIS step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		m_remark = "Step executed.";
		
		m_flowControl.setNextStep();
		return( stepState );
	}

} // TA_Base_App


#endif //  STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM
