/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/incidents/incident_manager/src/CtaIncidentSummary.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
