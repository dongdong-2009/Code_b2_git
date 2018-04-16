/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanSharedDataMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanSharedDataMessage.h"

namespace TA_Base_App
{
    ActivePlanSharedDataMessage::ActivePlanSharedDataMessage( const TA_Base_Core::KeyValueSeq& detail ) :
    m_detail(detail)
	{
	}

	void ActivePlanSharedDataMessage::execute( ActivePlan& activePlan ) const
    {
        std::map<std::string, std::string> mapSharedData;

        for ( unsigned int unLoop = 0u; unLoop < m_detail.length(); ++unLoop )
        {
            const TA_Base_Core::KeyValue& refDetail = m_detail[unLoop];

            mapSharedData[refDetail.key.in()] = refDetail.value.in();
        }

        activePlan.planSharedData( mapSharedData );
	}

} // TA_Base_App
