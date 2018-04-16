/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/incidents/incident_manager/src/CachedIncident.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// CachedIncident.h: interface for the CachedIncident class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CachedIncident_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_CachedIncident_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include <string>
#include <map>
#include <vector>
//TD15251 - EAccessType.h location has been moved to core
//#include "bus/incidents/incident_rights/src/EAccessType.h"
#include "core/data_access_interface/incident_dai/src/EAccessType.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h" //TD12923 

namespace TA_IRS_Bus
{
	class ItaIncidentRights;
}

namespace TA_IRS_Core
{
	class ItaIncidentReportData;
}

using namespace TA_IRS_Bus;
using namespace TA_IRS_Core;
  
namespace TA_IRS_App
{	
	class CachedIncident  
	{
		
	public:
    /**
	* getInstance
	*
	* Returns an instance of the class
	*
	* @return the pointer to this singleton instance
	*
		*/
		static CachedIncident& getInstance();
		
		/**
		* removeInstance
		*
		* Removes the instance of the class (if already created) and cleanup the members.  Primarily 
		* used upon program termination (e.g. from main()) so that Purify does not consider this class
		* and its members to be leaks.
		*
		*/
		static void removeInstance();
		
		/**
		* setCachedIncidentRight
		* 
		* set the incident cached rights
		* 
		* @return std::map<std::string, EAccessType> 
		* 
		* @exception <exceptions> Optional
		*/
		void setCachedIncidentRight(std::vector<ItaIncidentReportData*>& cachedincident);
		
		
		/**
		* reloadCachedIncidentRight
		* 
		* reload the cached incident rights due to the session changed
		* 
		* @return void 
		* @param : std::vector<ItaIncidentReportData*>& cachedincident
		* 
		* @exception <exceptions> Optional
		*/
		void reloadCachedIncidentRight(std::vector<ItaIncidentReportData*>& cachedincident);
		
		
		/**
		* addCachedIncidentRight
		* 
		* add the new cached incident rights to the map
		* 
		* @return void 
		* @param : ItaIncidentReportData* incidentData
		* 
		* @exception <exceptions> Optional
		*/
		void addCachedIncidentRight(ItaIncidentReportData* incidentData);
		
		/**
		* removeCachedIncidentRight
		* 
		* remove the cached rights from the map
		* 
		* @return void 
		* @param : ItaIncidentReportData* incidentData
		* 
		* @exception <exceptions> Optional
		*/
		void removeCachedIncidentRight(std::string& id);
		
		/**
		* getCachedIncidentControlRight
		* 
		* <description>
		* 
		* @return  bool 
		* 
		* @exception <exceptions> Optional
		*/
		bool getCachedIncidentControlRight(std::string& id);
		
		/**
		* getCachedIncidentViewRight
		* 
		* <description>
		* 
		* @return bool 
		* @param : std::string& id
		* 
		* @exception <exceptions> Optional
		*/
		bool getCachedIncidentViewRight(std::string& id);
		
		/**
		* setIncidentRights
		* 
		* set up the incident rights
		* 
		* @return void 
		* @param : ItaIncidentRights& incidentRights
		* 
		* @exception <exceptions> Optional
		*/
		void setIncidentRights(ItaIncidentRights& incidentRights);
		
	private:
	   	static CachedIncident* m_me;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock; //TD12923 
		ItaIncidentRights* m_incidentRights;
		
	    CachedIncident();
		virtual ~CachedIncident();
		
	    std::map<std::string, bool> m_cachedIncidentControl;
	    std::map<std::string, bool> m_cachedIncidentView;
		
	};
}// end TA_IRS_App
#endif // !defined(AFX_CachedIncident_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
