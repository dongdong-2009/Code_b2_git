/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/incidents/incident_manager/src/CtaIncidentSummary.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
