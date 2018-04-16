/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/incidents/incident_manager/src/IncidentReportDialog.h $
  * @author:  Song Toan
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2014/07/01 10:46:14 $
  * Last modified by:  $Author: Shiva $
  *
  * This dialog is responsible for displaying the incident report.  It's a modeless
  * dialog to allow several of it to be opened at a time.
  *
  */
#if !defined(IncidentReportDialog_D24D515D_B48F_4e4a_9824_7D176E872244__INCLUDED_)
#define IncidentReportDialog_D24D515D_B48F_4e4a_9824_7D176E872244__INCLUDED_

#include <vector>
    //Mintao ++
#include <string>
    //Mintao ++
#include <time.h>

#include "core/data_access_interface/incident_dai/src/ItaIncidentReportData.h"
#include "core/data_access_interface/incident_dai/src/ItaLocationAccessFactory.h"
#include "bus/incidents/incident_rights/src/ItaIncidentRights.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/mfc_extensions/src/readonly_enabled_listbox/ReadOnlyListBox.h"

#include "app/incidents/incident_manager/src/StationReportDialog.h"
#include "app/incidents/incident_manager/src/PlanDialog.h"
#include "app/incidents/incident_manager/src/AssetDialog.h"
#include "app/incidents/incident_manager/src/resource.h"

#include "core/utilities/src/DateFormat.h" // TD12474

namespace TA_Base_Bus{
		class ItaUserMessage;
		 
	};

/**
 * This dialog is responsible for displaying the incident report.  It's a modeless
 * dialog to allow several of it to be opened at a time.
 * @version 1.0
 * @created 16-May-2005 12:19:02 PM
 */
namespace TA_IRS_App
{
	
	class IncidentReportDialog: public CDialog
	{
		// Dialog Data
		//{{AFX_DATA(IncidentReportDialog)
		enum { IDD = IDD_INCIDENT_REPORT_DIALOG };
		CStatic	m_staticLocation2;
		CStatic	m_staticLocation1;
		CStatic	m_staticStartTime;
		CStatic	m_staticStartDate;
		CStatic	m_staticEndTime;
		CStatic	m_staticEndDate;
		CButton	m_buttonAddPlan;
		CButton	m_buttonAsset;
		CButton	m_buttonSave;
		CButton	m_buttonRemovePlan;
		CButton	m_buttonPrint;
		CButton	m_buttonViewStationReport;
		CButton	m_buttonEditStationReport;
		//TA_Base_Bus::CReadOnlyListBox m_listPlan; //TD15807
		CListBox m_listPlan;
		CString	m_sTitle;
		CString	m_sTrain;
		CString	m_sRecommendedAction;
		CString	m_sOperator;
		CString	m_sComment;
		CString	m_sAsset;
		CString	m_sCreator;
		CString	m_sCreatorProfile;
		CString	m_sCreationDate;
		CString	m_sCreationTime;
		CString	m_sModifier;
		CString	m_sModifiedDate;
		CString	m_sModifiedTime;
		CString	m_sModifierProfile;
		BOOL	m_bFalseIncident;
		BOOL	m_bInnerTrack;
		BOOL	m_bOuterTrack;
		COleDateTime	m_timeStartDate;
		COleDateTime	m_timeEndDate;
		COleDateTime	m_timeEndTime;
		COleDateTime	m_timeStartTime;
		CString	m_sAssociatedAlarm;
		CString	m_sAdditionalAlarmDescription;
		CString	m_sLocation1;
		CString	m_sLocation2;
		CDateTimeCtrl m_ctrlStartDate;
		CDateTimeCtrl m_ctrlEndDate;
		CDateTimeCtrl m_ctrlStartTime;
		CDateTimeCtrl m_ctrlEndTime;
		//TD15204 - reverting back the changes of TD15041
		CString	m_uID; // ++ lizettejl
		CString	m_uKilometreLoss;
		CString	m_uAccumulatedDelayHours;
		CString	m_uAccumulatedDelayMinutes;
		CString	m_uAccumulatedDelaySeconds;
		CString	m_uInitialDelayHours;
		CString	m_uInitialDelayMinutes;
		CString	m_uInitialDelaySeconds;
		//}}AFX_DATA
		
		AssetType m_Asset;
		
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(IncidentReportDialog)
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		virtual BOOL DestroyWindow();
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL
		
		// Implementation
	public:
		// Generated message map functions
		//{{AFX_MSG(IncidentReportDialog)
		afx_msg void onSave();
		afx_msg void onEditStationReport();
		afx_msg void onViewStationReport();
		afx_msg void onPrint();
		afx_msg void onAddPlan();
		afx_msg void onAssetSelection();
		afx_msg void onCancel();
		afx_msg long OnPrepareForClose( WPARAM wParam=0, LPARAM lParam=0);
		afx_msg long OnStationReportDlgEnd( WPARAM wParam=0, LPARAM lParam=0); // TD13867  
		virtual BOOL OnInitDialog();
		afx_msg void onRemovePlan();
		afx_msg void OnListPlanSelChange();
		afx_msg void OnEndDateChange(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnStartDateChange(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnStartTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnEndTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
		//}}AFX_MSG
		
		DECLARE_MESSAGE_MAP()
			
	public:
// CL18904++
		BOOL isAlarmExists();
// ++CL18904
		void setReadOnlySpecial(const int dlgID, CStatic &staticCtrl,const CString &strDisplay);
		StationReportDialog* GetStationReportDialg() {return m_StationReportDialog; } 
		static const UINT WM_APP_PREPAREFORCLOSE;
		static const UINT WM_STATIONREPORTDLG_END;
		/**
		* Initialise dialog with the given data.  If the specified ItaIncidentReportData
		* is not NULL, then we need to make a copy of the incident rather than
		* referencing it directly (for the obvious fact that we don't want the changes to
		* be reflected in the actual object - it's supposed to be updated from the
		* incident callback).  Create the ItaIncidentReportData object if it's NULL.
		* @param report
		* @param editable
		* @param incidentRights
		* @param locationAccessFactory
		* @param userMessage
		* @param controlClass
		* 
		*/
		
		IncidentReportDialog(   const ItaIncidentReportData* report, 
			bool editable, 
			ItaIncidentRights& incidentRights, 
			ItaLocationAccessFactory& locationAccessFactory,
			TA_Base_Bus::ItaUserMessage& userMessage,
			CWnd* pParent = NULL );
			/**
			* Constructs the report using the alarm ID.  It will attempt to populate the
			* dialog with as much details as possible using based on the alarm ID.
			* @param alarmId
			* @param incidentRights
			* @param locationAccessFactory
			* @param userMessage
			* @param controlClass
		*/
		IncidentReportDialog(	const std::string& alarmId, const std::string &alarmValue,
			bool editable, 
			ItaIncidentRights& incidentRights, 
			ItaLocationAccessFactory& locationAccessFactory, 
			TA_Base_Bus::ItaUserMessage& userMessage,
			CWnd* pParent = NULL );
		
		virtual ~IncidentReportDialog();
		
		/**
		* This just checks every controls in the dialog against the original data.
		* Returns true if anything is modified.  It doesn't actually sets the value in
		* m_report.
		* @return true if anything is modified.
		*/
		bool isModified();
		
		/**
		* Returns the incident that this dialog is displaying.
		* 
		*/
		ItaIncidentReportData* getIncident();
		
		/**
		* This is called whenever the application receives a callback that an incident
		* has been removed.  This method needs to check if the incident matches the
		* specified incident.  If it is different, then ignore it.  Otherwise, it needs
		* to pop out an error message telling the operator that the incident has been
		* removed and close the dialog upon acknowledgement.
		* @param incidentId
		* 
		*/
		//Mintao ++
		//TD13540 - modified parameter to follow programming guidelines
		void onIncidentRemoved(const std::string& incidentId );
		//Mintao ++
		
		/**
		* This is called whenever the application is notified of an incident update.
		* This method will check to see if the incident that it is displaying is the same
		* as the specified incident.  If it is not, just ignore it.  If it is, then it
		* must pop out an error message telling the operator that the incident has been
		* modified by another operator and give the option to the operator whether he/she
		* wants to refresh the dialog.  If the operator chooses to refresh the dialog,
		* the dialog will be populated with the new data.  Otherwise, the old data will
		* maintain in the dialog and the operator could proceed with whatever he/she was
		* doing before.
		* @param incident
		* 
		*/
		void onIncidentUpdate( ItaIncidentReportData* incident );
		
		/**
		* cleanupDialog
		* 
		* clean up the dialog
		* 
		* @return void 
		* 
		* @exception <exceptions> Optional
		*/
		void cleanupDialog();
		
		
	private:
    
		//Mintao ++
		//Bug 746
		/**
		* checkStartAndEndDateTime
		* 
		* This function checks the start date/time and end date/time
		* The start date/time should not be later then the end date/time
		* @return bool 
		* 
		* @exception <exceptions> Optional
		*/
		bool checkStartAndEndDateTime();
		//Mintao ++
		//Bug 746
		
		/**
		* setReadOnly
		* 
		* set the control to be read-only
		* 
		* @return void 
		* @param : const int itemId
		* @param : const int controlType
		* 
		* @exception <exceptions> Optional
		*/
		void setReadOnly(const int itemId, const int controlType);
		
		/*
		* Refresh controls with value from database
		* @return      none
		*/
		void refreshDisplay();
    
		/*
		* Retrieve date/time from dialog controls
		* @return      true if ok. Otherwise, user does not select any value
		*              the output value will be set to zero.
		* @param       datetimeStart/datetimeEnd: Buffer to store Date/Time
		*              Zero if user does not select any value.
		*/
		void getStartDateTimeFromDialog( time_t& datetimeStart ) const;
		void getEndDateTimeFromDialog( time_t& datetimeEnd ) const;
		
		/*
		* Enable date/time controls in the dialog 
		* @return      none
		* @param       bEnable: true or false
		*/
		void enableStartDateTimeControls(bool bEnable = true);
		void enableEndDateTimeControls(bool bEnable = true);
		/*
		* Add a PlanType into list box
		* @return      none
		* @lb          the list box
		* @planType    the PlanType object
		*/
		void addPlanTypeToListBox( CListBox& lb, PlanType& planType );
		
		/*
		* Get PlanType from list box
		*
		* @return      none
		* @lb          the list box
		* @arrayPlanTypes    the PlanType object array
		*/
		void getPlanTypesFromListBox( CListBox& lb, std::vector<PlanType>& arrayPlanTypes );
		
		/*
		* Clean up the PlanType list box
		*
		* @return      none
		* @lb          the list box
		*/
		void cleanUpPlanTypeListBox( CListBox& lb );
		
		/**
		* Contains the data of the incident report
		*/
		ItaIncidentReportData* m_report;
		ItaLocationAccessFactory& m_locationAccessFactory;
		ItaIncidentRights& m_incidentRights;
		TA_Base_Bus::ItaUserMessage& m_userMessage;
		bool m_bEditable;
		bool m_startDateEnable;
		bool m_endDateEnable;
		std::string m_alarmId;
		std::string m_alarmValue;
		
		//Begin-- TD18725
		//indicate if the report is new
		bool m_bIsReportNew;
		/*--------------------------------------------------
		Function:Set the range of the time control
		@return: none
		@bIsEndTime: true  -- is the end time
					 false -- isn't the end time
		@bIsDateChange: true  -- is date change
						false -- is time change
		----------------------------------------------------*/
		void SetTimeRange(bool bIsEndTime,bool bIsDateChange);
		//End-- TD18725
		StationReportDialog* m_StationReportDialog;

		//++Support Location display name
		CString getLocationName(CString strLocDisplayName);
	private:
		void setControlsToReadOnly();
		static const CTime Time_Lower_Limit;   //TD18725
		//the string used to launch the report manager application
		static const std::string STRING_TO_LAUNCH_REPORT_MANAGER;
		static const std::string FILE_NAME_PARAM;
		static const std::string REPORT_ID_PARAM;
		//Mintao ++
		//Bug 643
		static const std::string DB_CONNECTION_STRING;
		//Mintao ++
		//Bug 643
		
		static const std::string RPARAM_REPORTTEMPLATE;
		static const int IS_EDIT_BOX;
		static const int IS_COMBO_BOX;
		static const int IS_CHECK_OR_RADIO_BUTTON;
		static const int IS_DATE_TIME_CONTROL;

    	//TD13540 - add constants for the date and time format
    	//static const std::string DATE_FORMAT; // TD12474
		static const std::string TIME_FORMAT;

		static const std::string DB_CONNECTION_FILE;

		TA_Base_Core::DateFormat m_DateFormat; // TD12474

		//++Support Location display name
		T_LocDisplayMap m_mapLocations;
	};

}// end TA_IRS_App
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(IncidentReportDialog_D24D515D_B48F_4e4a_9824_7D176E872244__INCLUDED_)
