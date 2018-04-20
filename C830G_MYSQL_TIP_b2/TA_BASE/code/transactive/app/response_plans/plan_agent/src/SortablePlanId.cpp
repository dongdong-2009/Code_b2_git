/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/SortablePlanId.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "PlanAgent.h"


namespace TA_Base_App
{
	SortablePlanId::SortablePlanId()
	{
		m_plan.plan     = 0;
		m_plan.instance = 0;
	}

	SortablePlanId::SortablePlanId( const SortablePlanId& rhs ) :
		m_plan(rhs.m_plan)
	{
	}

	SortablePlanId::SortablePlanId(const TA_Base_Core::ActivePlanId& plan) :
		m_plan(plan)
	{
	}
			
	bool SortablePlanId::operator<( const SortablePlanId& rhs ) const
	{

        /*
        if ( m_plan.location < rhs.m_plan.location )
            return true;
        else if ( m_plan.location > rhs.m_plan.location )
            return false;

            */

		if( m_plan.plan < rhs.m_plan.plan )
		{
			return true;
		}
		else if( m_plan.plan > rhs.m_plan.plan )
		{
			return false;
		}
		else
		{
			if( m_plan.instance < rhs.m_plan.instance )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

		void SortablePlanId::set( const TA_Base_Core::ActivePlanId& newPlan )
	{
		m_plan = newPlan;
	}

		void SortablePlanId::setPlan( TA_Base_Core::NodeId plan )
	{
		m_plan.plan = plan;
	}

	void SortablePlanId::setInstance( CORBA::ULong instance )
	{
		m_plan.instance = instance;
	}

	TA_Base_Core::ActivePlanId SortablePlanId::get( void ) const
	{
		return m_plan;
	}

	TA_Base_Core::NodeId SortablePlanId::getPlan( void ) const
	{
		return m_plan.plan;
	}

	CORBA::ULong SortablePlanId::getInstance( void ) const
	{
		return m_plan.instance;
	}

} // TA_Base_App
