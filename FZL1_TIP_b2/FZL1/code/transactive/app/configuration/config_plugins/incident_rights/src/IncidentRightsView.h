/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/incident_rights/src/IncidentRightsView.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// IncidentRightsView.h: interface for the IncidentRightsView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INCIDENTRIGHTSVIEW_H__37D23A43_D5B8_4FFF_8533_377CFE81AF82__INCLUDED_)
#define AFX_INCIDENTRIGHTSVIEW_H__37D23A43_D5B8_4FFF_8533_377CFE81AF82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "app/configuration/config_plugins/config_plugin_helper/src/IMainView.h"

#include "app/configuration/config_plugins/incident_rights/src/IncidentRightsGridControl.h"
#include "app/configuration/config_plugins/incident_rights/src/resource.h"

#include "core/data_access_interface/incident_dai/src/ItaReportAccessFactory.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

  
namespace TA_IRS_App
{	
	class IncidentRightsComponent;
	
	class IncidentRightsView : public CFormView  
	{

	protected:	
		/**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
		IncidentRightsView();

		/**
         * Destructor
         */
		virtual ~IncidentRightsView();

		DECLARE_DYNCREATE(IncidentRightsView)

	public:
	 //{{AFX_DATA(IncidentRightsView)
	enum { IDD = IDD_INCIDENT_RIGHTS_VIEW };
    //}}AFX_DATA

	public:

        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         *
         * @param bool - This indicates if the buttons should be available or not.
         */
        virtual void refreshButtonAvailability(bool isAvailable);

		/**
         * initialise
         *
         * This is called by the component interface to register itself with this view. This view
         * is the main view so it provides all communication between the various
         * bits and pieces. This method will also perform any final initialization required on its
         * child views now that it has all the information it requires.
		 *
		 * @param IncidentRightsComponent* - component interface associated with the view
		 * @param IProgressStatusBar* - The status bar to use for displaying a progress bar.
         */
		void initialise(IncidentRightsComponent* component, TA_Base_App::IProgressStatusBar* statusBar);
	 

		/**
         * redraw
         *
         * This will retrieve incident rights from the database and load it to the 
		 * grid controls.
         */
		void redraw();

		 /**
         * areCurrentChangesPending
         *
         * This is called to determine if there are any current changes that have not been applied.          
         * 
		 */
		bool areCurrentChangesPending();
		
	  // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(IncidentRightsView)
	public:
	virtual void OnInitialUpdate();
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
    protected:
		#ifdef _DEBUG
			virtual void AssertValid() const;
			virtual void Dump(CDumpContext& dc) const;
		#endif

 		// Generated message map functions
		//{{AFX_MSG(RightsView)
		afx_msg void onButtonApply();
		afx_msg void onButtonReset();
		afx_msg void onChangeSubsystemState();		
		afx_msg void OnSelchangeIncidentStationAccessTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
			

	private:
		/**
         * loadItems
         *
         * This will load all the items and populate the grids.
         */
		void loadItems();

 		// These are the control Id's to give the grids we create. These id's are the two last id's in the
        // range specified for this component.
		static const int STATIONRIGHTS_GRID_ID;
        static const int INCIDENTRIGHTS_GRID_ID;		

		
		ItaReportAccessFactory* m_incidentAccessFactory; 

		AccessMatrix m_incidentAccessMatrix;
		AccessMatrix m_stationAccessMatrix;

		IncidentRightsGridControl m_incidentRightsGrid;
		IncidentRightsGridControl m_stationRightsGrid;
		
		IncidentRightsComponent* m_incidentRightsComponent;

		// This is the status bar to use for showing progress
        TA_Base_App::IProgressStatusBar* m_statusBar;
	};
}

#endif // !defined(AFX_INCIDENTRIGHTSVIEW_H__37D23A43_D5B8_4FFF_8533_377CFE81AF82__INCLUDED_)
