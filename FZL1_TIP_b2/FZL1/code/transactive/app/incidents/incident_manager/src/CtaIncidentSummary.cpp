/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/incidents/incident_manager/src/CtaIncidentSummary.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Simply a placeholder to combine all TransActive interfaces to a single point
  */

#include "app/incidents/incident_manager/src/stdafx.h"
#include "app/incidents/incident_manager/src/CtaIncidentSummary.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{	
	CtaIncidentSummary::CtaIncidentSummary(TA_Base_Core::IEntityDataPtr MgrConfiguration):
		CtaRuntime(MgrConfiguration)
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}

	CtaIncidentSummary::~CtaIncidentSummary()
	{
		FUNCTION_ENTRY( "Destructor" );
		FUNCTION_EXIT;
	}

}// end TA_IRS_App
