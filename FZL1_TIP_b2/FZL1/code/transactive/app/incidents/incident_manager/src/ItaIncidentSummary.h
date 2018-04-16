/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/incidents/incident_manager/src/ItaIncidentSummary.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Simply a placeholder to combine all TransActive interfaces to a single point
  */
#if !defined(ItaIncidentSummary_6FF99996_7303_48a6_A4CA_83DE73B017B6__INCLUDED_)
#define ItaIncidentSummary_6FF99996_7303_48a6_A4CA_83DE73B017B6__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
    #pragma warning( disable : 4786)
#endif

#include "core/data_access_interface/incident_dai/src/ItaIncidentReportFactory.h"
#include "bus/incidents/incident_rights/src/ItaIncidentRights.h"
#include "core/data_access_interface/incident_dai/src/ItaLocationAccessFactory.h"
#include "bus/generic_gui/ItaUserMessage/public_interfaces/ItaUserMessage.h"
#include "core/process_management/public_interfaces/ItaRuntime.h"

/**
 * Simply a placeholder to combine all TransActive interfaces to a single point.
 * @version 1.0
 * @created 16-May-2005 12:19:04 PM
 */
using namespace TA_IRS_Bus;

namespace TA_IRS_App
{
	
	class ItaIncidentSummary : public virtual ItaIncidentReportFactory, 
        public virtual ItaIncidentRights, 
        public virtual ItaLocationAccessFactory, 
        public virtual TA_Base_Bus::ItaUserMessage,
        public virtual TA_Base_Core::ItaRuntime
	{
	public:
		virtual ~ItaIncidentSummary() {};
	};
}// end TA_IRS_App
#endif // !defined(ItaIncidentSummary_6FF99996_7303_48a6_A4CA_83DE73B017B6__INCLUDED_)