#ifndef ACTIVE_INCLUSIVE_GATEWAY_H_INCLUDED
#define ACTIVE_INCLUSIVE_GATEWAY_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveInclusiveGateway.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */
#include <set>
#include <string>

#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/internal/src/ConditionalGatewayParameter.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveInclusiveGateway : public ActiveStep
	{
	// Public methods
	public:
        ActiveInclusiveGateway( const BasicParameter& detail, IActivePlan& plan );
        virtual std::list<ActiveStep*> getNextStep();
		
    protected:
        virtual void initImpl();
		virtual bool executeImpl();
        virtual int waitImpl();

        void initAsDiverging();
        void initAsConverging();

        std::string outputName( const unsigned int unId );
        std::string outputValue( const size_t szOutgoingNum );

        size_t divergingSize( const unsigned int unId );

	private:
		ActiveInclusiveGateway& operator=( const ActiveInclusiveGateway& );
		ActiveInclusiveGateway( const ActiveInclusiveGateway& );

    private:
        bool m_bSuccess;
        bool m_bNextReturned;
        std::set<unsigned long> m_outgoingStepIds;
        std::string m_strFailureReason;
        size_t m_szConvergingSize;
		ConditionalGatewayParameter m_params;
	}; // ActiveInclusiveGateway
}; // TA_Base_App

#endif // ACTIVE_INCLUSIVE_GATEWAY_H_INCLUDED
