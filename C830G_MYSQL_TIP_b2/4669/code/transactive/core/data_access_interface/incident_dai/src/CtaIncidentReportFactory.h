/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/incident_dai/src/CtaIncidentReportFactory.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * CtaLocationAccessFactory is a singleton that is used to retrieve Incident Report objects from the
  * database. 
  */

#ifndef CtaIncidentReportFactory_ED23B9B5_CB0F_4980_A0CA_F090E68FE9EF__INCLUDED_
#define CtaIncidentReportFactory_ED23B9B5_CB0F_4980_A0CA_F090E68FE9EF__INCLUDED_

#include "core/data_access_interface/incident_dai/src/ItaIncidentReportFactory.h"
#include "core/data_access_interface/incident_dai/src/ItaIncidentGuiCallback.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

namespace TA_IRS_Core
{
	
	class CtaIncidentReportFactory : public virtual ItaIncidentReportFactory ,
		public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
	{
		
	public:
		CtaIncidentReportFactory();
		virtual ~CtaIncidentReportFactory();
		
		/**
		* Retrieve all Incident Reports that are viewable by the current Profile/Location
		* combination. For performance reasons, the factory is responsible performing
		* profile/location checks via SQL to determine viewability.
		* 
		* NOTE: The client is reponsible for deleting this data after it has finished
		* with it.  
		*/
		virtual std::vector<ItaIncidentReportData*> getIncidentReports();
		
		/**
		* Only one gui can register. When edit, add, or remove updates arrive
		* ItaIncidentReportFactory will call on this interface with the new information.
		* @param gui
		* 
		*/
		virtual void registerCallback(ItaIncidentGuiCallback* gui);
		
		/**
		* As only one GUI can register, the deregister method does not need a parameter.
		*/
		virtual void deregister();
		/**
		* Returns the incident report data with the given incident ID.
		* @return the incident report data with the given incident ID.   NULL if there is
		* no associated incident.
		* @param id    the incident id
		* 
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		
		virtual ItaIncidentReportData* getIncidentReport(std::string id);
		//Mintao ++
		//Bug 652 (TD12313)
		
		/**
		* Returns a new instance. It determines creator details from the current session.
		* Create an incident report with just the basic report details.  The set methods
		* would have to be used to set other attributes.  The method will figure out the
		* creator and creator profile from the session ID and the creation date and time
		* from the current timestamp. 
		*/
		virtual ItaIncidentReportData* createIncidentReport();
		
		/**
		* Creates an incident with an associated alarm.
		* @param alarmId    The alarm Id
		* 
		*/
		virtual ItaIncidentReportData* createIncidentReport(const std::string & alarmId, const std::string & alarmValue);
		
		/**
		* receiveSpecialisedMessage
		*
		* This method is called whenever the Agent receives a state update or
		* state update request (depending on their mode of operation)
		*
		*/
		virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);
		
	private:
		ItaIncidentGuiCallback* m_ItaIncidentGuiCallback;
		
		
		static const std::string IN_ININCI_ID_COL;
		static const std::string IN_TITLE_COL;
		static const std::string IN_CREATE_OPERATOR_COL;
		static const std::string IN_CREATE_OPERATOR_PROFILE_COL;
		static const std::string IN_CREATE_DATE_COL;
		static const std::string IN_ASSET_COL;
		static const std::string IN_ASSET_SUBSYSTEM_COL;
		static const std::string IN_LOCATION_1_COL;
		static const std::string IN_LOCATION_2_COL;
		static const std::string IN_TRAIN_ID_COL;
		static const std::string IN_INNER_TRACK_COL;
		static const std::string IN_OUTER_TRACK_COL;
		static const std::string IN_ALARM_ID_COL;
		static const std::string IN_CLOSE_DATE_COL;
		
		
	};
} // end TA_IRS_Core
#endif
