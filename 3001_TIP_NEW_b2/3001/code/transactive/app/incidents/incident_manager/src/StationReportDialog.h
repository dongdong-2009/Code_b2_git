/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/incidents/incident_manager/src/StationReportDialog.h $
  * @author:  Grace Koh
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This dialog is responsible for displaying the details of the station report.
  * This is a modal dialog.
  */
#if !defined(StationReportDialog_2632499D_317D_44da_A52C_1F671C522661__INCLUDED_)
#define StationReportDialog_2632499D_317D_44da_A52C_1F671C522661__INCLUDED_

#include "bus/generic_gui/ItaUserMessage/public_interfaces/ItaUserMessage.h"
#include "core/data_access_interface/incident_dai/src/ItaStationReportData.h"
#include "bus/incidents/incident_rights/src/ItaIncidentRights.h"
#include "app/incidents/incident_manager/src/resource.h"

#include "core/utilities/src/DateFormat.h" // TD12474

/**
 * This dialog is responsible for displaying the details of the station report.
 * This is a modal dialog.
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:19:05 PM
 */
using namespace TA_IRS_Bus;

namespace TA_IRS_App
{	
	class StationReportDialog: public CDialog
	{
		
		// Construction
	public:
		
		// Dialog Data
		//{{AFX_DATA(StationReportDialog)
		enum { IDD = IDD_STATION_REPORT_DIALOG };
		CStatic	m_staticDate;
		CButton	m_SaveBtn;
		CButton	m_CausedByPower;
		CEdit	m_EditEPControl;
		CButton	m_IncTypeOtherCrtl;
		CButton	m_IncTypeHeatedArgCrtl;
		CButton	m_IncTypeAssaultCrtl;
		CButton	m_IncTypeArrestPersonCrtl;
		CButton	m_IncTypeCriminalActCrtl;
		CButton	m_IncTypeTrespassingCrtl;
		CButton	m_IncTypeEquipMalfCrtl;
		CButton	m_IncTypeEquipAbnormalCrtl;
		CButton	m_IncTypeFireCrtl;
		CButton	m_IncTypeDemagedFacilityCrtl;
		CButton	m_IncTypeDerailmentCrtl;
		CButton	m_IncTypeTrainDelayCrtl;
		//TD15204 - reverting back the changes of TD15041
		CEdit	m_ReportIdControl; // ++ lizettejl
		CButton	m_LocationOtherControl;
		CButton	m_LocationDepotControl;
		CButton	m_LocationStationControl;
		CButton	m_LocationTracksideControl;
		CButton	m_LocationTrainControl;
		CDateTimeCtrl	m_DatePickerControl;
		CButton	m_ConditionOtherControl;
		CButton	m_ConditionPoorlyLitControl;
		CButton	m_ConditionWellLitControl;
		CButton	m_ConditionDirtyControl;
		CButton	m_ConditionCleanControl;
		CButton	m_ConditionWetControl;
		CButton	m_ConditionDryControl;
		CButton	m_CheckBoxCausedByOthers;
		CButton	m_CausedByTkCct;
		CButton	m_CausedBySignal;
		CButton	m_CausedByPoint;
		CButton	m_CausedByPAX;
		CButton	m_CausedByBLS;
		CButton	m_CausedByESP;
		CString	m_SubmittedBy;
		CString	m_EP;
		CString	m_ActionTaken;
		CString	m_TypeOthers;
		CString	m_LocationOthers;
		CString	m_ConditionOther;
		CString	m_CausedByOthers;
		CString	m_StaffInvolved;
		CString	m_Description;
		BOOL	m_bTypeOthers;
		BOOL	m_bLocationOthers;
		BOOL	m_bConditionOthers;
		BOOL	m_bCausedByOthers;
		CTime	m_DateTime;
		//}}AFX_DATA
		
		
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(StationReportDialog)
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL
		
		// Implementation
	protected:
		bool m_bDateEnable;
		
		// Generated message map functions
		//{{AFX_MSG(StationReportDialog)
		afx_msg void onCancel();
		afx_msg void onSave();
		afx_msg void OnCheckTypeOthers();
		afx_msg void OnCheckLocationOthers();
		afx_msg void OnCheckConditionOthers();
		afx_msg void OnCheckCausedByOthers();
		afx_msg long OnRefreshDisplay(WPARAM wParam = 0, LPARAM lParam = 0);
		virtual BOOL OnInitDialog();
		afx_msg void OnDatetimechangeDatepickerDate(NMHDR* pNMHDR, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
			
	public:
		void setDateCtrlToReadOnly(const int dlgID, CStatic &staticCtrl);
		
		/**
		* Constructor
		* @param stationReport    The station report data to be displayed on this dialog
		* @param editable    Sets the controls of the dialog to be editable if editable
		* is true; make them all read-only otherwise.
		 * @param incidentRights
		 * @param userMessage
		 * @param controlClass
		 */
		StationReportDialog(ItaStationReportData& stationReport, 
							bool editable, 
							ItaIncidentRights& incidentRights, 
							TA_Base_Bus::ItaUserMessage& userMessage,
							CWnd *pParent = NULL );
		
		virtual ~StationReportDialog();

		/**
		 * This just checks every controls in the dialog against the original data.
		 * Returns true if anything is modified.  It doesn't actually sets the value in
		 * m_report.
		 * @return true if anything is modified.
		 */
		bool isModified();
		//window message type to allow incident report dialog to notify station report
		//diaglog to update the incident data
		static const UINT WM_APP_REFRESHDISPLAY;

	private:
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
		/*
		 * Enable date/time controls in the dialog 
		 * @return      none
		 * @param       bEnable: true or false
		 */
		void enableTimeControls(bool bEnable = true);
		//flag to inidicate whether the dialog is editable or not
		bool m_bEditable;

		/**
		 * Contains the station report data.
		 */
		ItaStationReportData& m_report;
		//contains the user message
		TA_Base_Bus::ItaUserMessage& m_userMessage;
		//contains the user right(privilege)
		ItaIncidentRights& m_incidentRights;
    
		static const int IS_EDIT_BOX;
		static const int IS_COMBO_BOX;
		static const int IS_CHECK_OR_RADIO_BUTTON;
		static const int IS_DATE_TIME_CONTROL;

    	// TD13540 - add constants for the date and time format
    	//static const std::string DATE_FORMAT; // TD12474

      	TA_Base_Core::DateFormat m_DateFormat; // TD12474
	};
}// end TA_IRS_App
#endif // !defined(StationReportDialog_2632499D_317D_44da_A52C_1F671C522661__INCLUDED_)
