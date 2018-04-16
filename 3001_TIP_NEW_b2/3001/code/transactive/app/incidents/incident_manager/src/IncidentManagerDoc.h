/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/incidents/incident_manager/src/IncidentManagerDoc.h $
  * @author:  Song Toan
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class inherits from the CDocument.  It will not perform the actual
  * business logic but uses the IncidentHandler to manager all incident related
  * logics to allow the class to be tested easily.
  * 
  */

#if !defined(IncidentManagerDoc_969AA024_5D09_4dc5_A09D_16DE6E1DDB0C__INCLUDED_)
#define IncidentManagerDoc_969AA024_5D09_4dc5_A09D_16DE6E1DDB0C__INCLUDED_

    //Mintao ++
    //Bug 652 (TD12313)
#include <string>
    //Mintao ++
    //Bug 652 (TD12313)
#include "app/incidents/incident_manager/src/IncidentHandler.h"
#include "app/incidents/incident_manager/src/IIncidentUpdate.h"
#include "app/incidents/incident_manager/src/ItaIncidentSummary.h"
#include "core/data_access_interface/incident_dai/src/ItaIncidentReportFactory.h"
#include "core/data_access_interface/incident_dai/src/ItaLocationAccessFactory.h"

namespace TA_IRS_App
{	
	class IncidentManagerDoc : public CDocument, public virtual IIncidentUpdate
	{
	protected:
		DECLARE_DYNCREATE(IncidentManagerDoc)
			
			// Attributes
	public:
		
		// Operations
	public:
		
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(IncidentManagerDoc)
	public:
		virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
		virtual BOOL OnNewDocument();
		//}}AFX_VIRTUAL
		
		// Implementation
#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif
		
		// Generated message map functions
	protected:
		//{{AFX_MSG(IncidentManagerDoc)
		afx_msg void onViewAllIncidents();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
			
			//-------------------------------------------------------------------------
	public:
		IncidentManagerDoc();
		virtual ~IncidentManagerDoc();
		
	public:
		
	/**
	* Returns all incident reports to be displayed on the view.
		*/
		std::vector<ItaIncidentReportData*> getIncidents();
		
		/**
		* @param incidentReportFactory
		* @param incidentRights
		* @param locationAccessFactory
		* @param userMessage
		* 
		*/
		void initialise(	ItaIncidentReportFactory& incidentReportFactory, 
			ItaIncidentRights& incidentRights, 
			ItaLocationAccessFactory& locationAccessFactory, 
			TA_Base_Bus::ItaUserMessage& userMessage);
			/**
			* Returns false if this class has not yet been initialised.
			* @return false if this class has not yet been initialised.
		*/
		bool isInitialised();
		/**
		* It will tell the view to close any unsaved windows.
		*/
		void prepareForClose();
		
		ItaLocationAccessFactory& getLocationAccessFactory();
		ItaIncidentRights& getIncidentRights();
		ItaIncidentReportFactory& getIncidentReportFactory();
		TA_Base_Bus::ItaUserMessage& getUserMessage();
		
		/**
		* Updates the rights on the selected incident based on the operator's new
		* privileges.
		*/
		virtual void updateRights();
		
		/**
		* Could be called when there is a change in the incident list.
		*/
		virtual void reloadIncidents();
		/**
		* Display the dialog for creating an incident report
		* @param alarmId
		* 
		*/
		virtual void createIncident(const std::string& alarmId);
		/**
		* Remove the specified incident from the list.
		* @param incidentId
		* 
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		virtual void removeIncident(std::string incidentId);
		//Mintao ++
		//Bug 652 (TD12313)
		/**
		* Updates the specified incident on the displayable list.
		* @param incident
		* 
		*/
		virtual void updateIncident(ItaIncidentReportData* incident);
		
	private:
		ItaIncidentRights* m_incidentRights;
		ItaLocationAccessFactory* m_locationAccessFactory;
		TA_Base_Bus::ItaUserMessage* m_userMessage;
		IncidentHandler* m_incidentHandler;
		ItaIncidentSummary *m_ItaIncidentSummary;
		ItaIncidentReportFactory *m_incidentReportFactory;
		
		bool m_bInitialised;
		bool m_showAll;
	};
}// end TA_IRS_App
#endif  // !defined(IncidentManagerDoc_969AA024_5D09_4dc5_A09D_16DE6E1DDB0C__INCLUDED_)
