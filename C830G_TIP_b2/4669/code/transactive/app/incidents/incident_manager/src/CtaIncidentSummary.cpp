/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/CtaIncidentSummary.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
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
