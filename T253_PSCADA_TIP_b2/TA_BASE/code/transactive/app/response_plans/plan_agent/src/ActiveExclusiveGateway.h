#ifndef ACTIVE_EXCLUSIVE_GATEWAY_H_INCLUDED
#define ACTIVE_EXCLUSIVE_GATEWAY_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveExclusiveGateway.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/internal/src/ConditionalGatewayParameter.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveExclusiveGateway : public ActiveStep
	{
	// Public methods
	public:
        ActiveExclusiveGateway( const BasicParameter& detail, IActivePlan& plan );
        virtual std::list<ActiveStep*> getNextStep();
		
    protected:
        virtual void initImpl();
        virtual bool executeImpl();

        void initAsDiverging();
        void initAsConverging();

	private:
		ActiveExclusiveGateway& operator=( const ActiveExclusiveGateway& );
		ActiveExclusiveGateway( const ActiveExclusiveGateway& );

    private:
        bool m_bSuccess;
		unsigned long m_outgoingStepId;
        std::string m_strFailureReason;
		ConditionalGatewayParameter m_params;
	}; // ActiveExclusiveGateway
}; // TA_Base_App

#endif // ACTIVE_EXCLUSIVE_GATEWAY_H_INCLUDED
