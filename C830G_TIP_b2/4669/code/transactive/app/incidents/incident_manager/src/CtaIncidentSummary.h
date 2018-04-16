/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/CtaIncidentSummary.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Simply a placeholder to combine all TransActive interfaces to a single point
  */

#if !defined(CtaIncidentSummary_2E1D319B_E5E3_41b3_8984_F9A0148966B4__INCLUDED_)
#define CtaIncidentSummary_2E1D319B_E5E3_41b3_8984_F9A0148966B4__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
    #pragma warning( disable : 4786 4250)
#endif

#include "core/data_access_interface/incident_dai/src/CtaLocationAccessFactory.h"
#include "bus/incidents/incident_rights/src/CtaIncidentRights.h"
#include "core/data_access_interface/incident_dai/src/CtaIncidentReportFactory.h"
#include "bus/generic_gui/ItaUserMessage/src/CtaUserMessage.h"

#include "app/incidents/incident_manager/src/ItaIncidentSummary.h"
#include "core/process_management/src/CtaRuntime.h"

/**
 * @version 1.0
 * @created 16-May-2005 12:18:54 PM
 */
namespace TA_IRS_App
{	
	class CtaIncidentSummary : public virtual CtaIncidentRights, 
        public virtual CtaLocationAccessFactory, 
        public virtual CtaIncidentReportFactory, 
        public virtual TA_Base_Bus::CtaUserMessage, 
        public virtual ItaIncidentSummary,
        public virtual TA_Base_Core::CtaRuntime
	{
		
	public:
		CtaIncidentSummary(TA_Base_Core::IEntityDataPtr MgrConfiguration);
		virtual ~CtaIncidentSummary();
	};
}// end TA_IRS_App
#endif // !defined(CtaIncidentSummary_2E1D319B_E5E3_41b3_8984_F9A0148966B4__INCLUDED_)