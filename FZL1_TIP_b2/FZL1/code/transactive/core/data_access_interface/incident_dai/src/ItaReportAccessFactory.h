/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/incident_dai/src/ItaReportAccessFactory.h $
  * @author:  Wu Min Tao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * The class provide the access matrix for the incident report access
  */

#ifndef ItaReportAccessFactory_ED23B9B5_CB0F_4980_A0CA_F090E68FE9EF__INCLUDED_
#define ItaReportAccessFactory_ED23B9B5_CB0F_4980_A0CA_F090E68FE9EF__INCLUDED_

//TD15251 - incident DAI shouldn't be referencing to files in bus
// EAccessType and ProfileAccess definitions are moved to core
// #include "bus/incidents/incident_rights/src/EAccessType.h"
// #include "bus/incidents/incident_rights/src/ItaIncidentRights.h"
#include <map>
#include "core/data_access_interface/incident_dai/src/EAccessType.h"
namespace TA_IRS_Core
{	
	typedef std::map<unsigned long, EAccessType> ProfileAccess; //just a mapping of profile group to access level
	typedef std::map<unsigned long, ProfileAccess> AccessMatrix;
	//++lizettejl
	
	class ItaReportAccessFactory 
	{
		
	public:
		virtual ~ItaReportAccessFactory() {};
		
		virtual AccessMatrix getAccessMetrix(bool isIncident) = 0;
		// TD15203 - access matrix can be updated using the incident rights config plugin
		virtual void updateAccessMatrix(const bool isIncident, const AccessMatrix& newMatrix) = 0;
		
	};
}// end TA_IRS_Core
#endif
