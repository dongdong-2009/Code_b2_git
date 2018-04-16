/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/incident_dai/src/ItaIncidentGuiCallback.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Used by the GUI to allow ItaIncidentReportFactory to notify it about database
  * updates of interest.
  */

#ifndef ItaIncidentGuiCallback_24402EB3_71E4_4f88_9CA6_A15745C9A855__INCLUDED_
#define ItaIncidentGuiCallback_24402EB3_71E4_4f88_9CA6_A15745C9A855__INCLUDED_

#include "core/data_access_interface/incident_dai/src/ItaIncidentReportData.h"
namespace TA_IRS_Core
{	
	class ItaIncidentGuiCallback
	{
		
	public:
		virtual ~ItaIncidentGuiCallback() {};
		/**
		* A new Incident Report has been created by another operator and is viewable (at
		* least) by the operator associated with the current session.
		* @param incidentData
		* 
		*/
		virtual void add(ItaIncidentReportData* incidentData) =0;
		
		/**
		* One of the reports in the list of viewable reports has been removed by another
		* operator.
		* @param incidentId
		* 
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		virtual void remove(const char* incidentId) =0;
		
		/**
		* A report in the list of reports viewable by the operator associated with the
		* current session has been modified by another operator.
		* @param incidentId
		* 
		*/
		virtual void update(const char* incidentId) =0;
		//Mintao ++
		//Bug 652 (TD12313)
		
	};
}// end TA_IRS_Core
#endif
