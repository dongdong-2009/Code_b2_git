#ifndef IACTIVEPLANFLOW_H_INCLUDED
#define IACTIVEPLANFLOW_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/IActivePlanFlow.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

namespace TA_Base_App
{
    class ActivePlanState;

	//Class declaration
	class IActivePlanFlow
	{
	// Public methods
	public:
		IActivePlanFlow(){}
		virtual ~IActivePlanFlow(){}

		virtual void setIsActive( bool isActive ) = 0;
		virtual void setNextStep( TA_Base_Core::StepNumber step ) = 0;
		virtual void setNextStep( void ) = 0;
		virtual void setCurrentStepPosition( TA_Base_Core::StepNumber step) = 0;
		virtual TA_Base_Core::StepNumber getStepNumber() = 0;

		virtual const TA_Base_Core::ActivePlanId& getPlan( void ) const = 0;

		virtual void setTerminatedInterlock( TA_Base_Core::NodeId interlock ) = 0; 
		virtual void setCompletedInterlock( const TA_Base_Core::ActivePlanId& interlock ) = 0;
        virtual void setOperatorResponseInterlock(bool interlock ) = 0;

        virtual void setPlanStepFailed() = 0;
        virtual bool anyPlanStepFailed() = 0;

        virtual void changeState(ActivePlanState* pState) = 0;

        virtual void setInteractiveStep(bool isInteractiveStep) = 0;

		virtual void pausePlan( const std::string& remark, const std::string& failedStepAlarmId = "" ) = 0;

		virtual std::string getRemark( void ) = 0;

	private:
		IActivePlanFlow& operator=( const IActivePlanFlow& );
		IActivePlanFlow( const IActivePlanFlow& );

	}; // IActivePlanFlow
}; // TA_Base_App

#endif // IACTIVEPLANFLOW_H_INCLUDED
