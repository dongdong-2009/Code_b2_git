/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/incidents/incident_manager/src/IncidentReportDialog.cpp $
  * @author:  Song Toan
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This dialog is responsible for displaying the incident report.  It's a modeless
  * dialog to allow several of it to be opened at a time.
  */

#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/AlarmAccessFactory.h"
#include "core/data_access_interface/src/IAlarmData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"

#include "app/incidents/incident_manager/src/stdafx.h"
#include "app/incidents/incident_manager/src/CMainFrame.h"
#include "app/incidents/incident_manager/src/CIncidentManagerListView.h"
#include "app/incidents/incident_manager/src/PlanDialog.h"
#include "app/incidents/incident_manager/src/IncidentReportDialog.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/application_types/src/apptypes.h"
// TD13540 - this header file is not used
// #include "bus/mfc_extensions/src/bitmap_and_text_push_button/FooButton.h" 

namespace TA_IRS_App
{
	const CTime IncidentReportDialog::Time_Lower_Limit(1); //TD18725
	const std::string IncidentReportDialog::STRING_TO_LAUNCH_REPORT_MANAGER = "--PrintIncident=1";
	const std::string IncidentReportDialog::FILE_NAME_PARAM = " --file=";
	const std::string IncidentReportDialog::REPORT_ID_PARAM = " --IncidentId=";
	//Mintao ++
	//Bug 643
	const std::string IncidentReportDialog::DB_CONNECTION_STRING =" --db-connection=";
	//Mintao ++
	//Bug 643
	const std::string IncidentReportDialog::RPARAM_REPORTTEMPLATE = "ReportTemplate";

		//libo
	const std::string IncidentReportDialog::DB_CONNECTION_FILE =" --db-connection-file=";
	//libo 

	const int IncidentReportDialog::IS_EDIT_BOX = 1;
	const int IncidentReportDialog::IS_COMBO_BOX = 2;
	const int IncidentReportDialog::IS_CHECK_OR_RADIO_BUTTON = 3;
	const int IncidentReportDialog::IS_DATE_TIME_CONTROL = 4;

	//TD13540  - add constants for the date and time format
	//const std::string IncidentReportDialog::DATE_FORMAT = "dd'-'MMM'-'yyy"; // TD12474
	const std::string IncidentReportDialog::TIME_FORMAT = "HH':'mm':'ss";

	using namespace std;
	using namespace TA_Base_Core;
	using namespace TA_Base_Bus;

	//can not do the general casting here, so add a flag to indicate which control it is
	void IncidentReportDialog::setReadOnly(const int itemId, const int controlType)
	{
		FUNCTION_ENTRY( "setReadOnly" );
		CWnd* wnd = this->GetDlgItem(itemId);
		TA_ASSERT(wnd != NULL, "The control cannot be found");

		if (controlType == IS_EDIT_BOX)
		{
			// edit box
			CEdit* edit = reinterpret_cast<CEdit*>(wnd);
			if (edit != NULL)
			{
				edit->SetReadOnly(TRUE);
    			FUNCTION_EXIT;
				return;
			}
		}
		else if (controlType == IS_COMBO_BOX)
		{
			// id of the resource, can be any number since we're not going to do anything with it apart from
			// displaying it as read-only
			int newResourceId = 1; 

			// combo box
			CComboBox* combo = reinterpret_cast<CComboBox*>(wnd);
			if (combo != NULL)
			{
				CString strControlText;
				combo->GetWindowText(strControlText);

				// Get edit control which happens to be the first child window
				CEdit* edit = reinterpret_cast<CEdit*>(combo->GetWindow(GW_CHILD));
				if (edit != NULL)
				{
					// Set read only is combo box is disabled
					edit->SetReadOnly(TRUE);
        			FUNCTION_EXIT;
					return;
				}

				// otherwise this is a drop down list, which has a static control rather than edit control, so
				// it's difficult for us to make it read-only.  So we'll just hide this control and put a new
				// drop down combo in its place
				
				// TD #12425
				// SOLUTION 1:
				// Disable the combo box, and overlay a CStatic label to display the text.
				// Issue with this: unsure how to get the bounds of the combo box's 3d border,
				// so needed to reduce the size of the label using hardcoded values.
				combo->EnableWindow(false);
				CRect rect;
				combo->GetWindowRect(&rect);
				ScreenToClient(&rect);
				rect.top += 4;
				rect.bottom -= 2;
				rect.left +=4;
				rect.right -= 18;
				CStatic* comboText = new CStatic;
				CString text;
				combo->GetWindowText(text);
				comboText->Create(text, WS_CHILD|WS_VISIBLE, rect, this, newResourceId);

    			FUNCTION_EXIT;
				return;
			}
		}
		else if (controlType == IS_CHECK_OR_RADIO_BUTTON)
		{
			// check box or radio	 
			// lizettejl- No FooButton was used in this dialog
			//FooButton* buttonEx = reinterpret_cast<FooButton*>(wnd); 
			CButton* buttonEx = reinterpret_cast<CButton*>(wnd); 
			if (buttonEx != NULL)
			{
				//buttonEx->setReadOnly(true);
				//buttonEx->enable(false); 
				buttonEx->EnableWindow(FALSE);
    			FUNCTION_EXIT;
				return;
			}
		 
		}
		else if (controlType == IS_DATE_TIME_CONTROL)
		{
			CDateTimeCtrl* dateTimeCtrl = reinterpret_cast<CDateTimeCtrl*>(GetDlgItem(itemId));
			if (dateTimeCtrl != NULL)
			{
				// id of the resource, can be any number since we're not going to do anything with it apart from
				// displaying it as read-only
				int newResourceId = 1; 
				// otherwise it is read-only.  As we can't make the CDateTimeCtrl read-only, we have to substitute
				// it with a spinner edit (if it's time) or combo box (if it's date).

				// get the size of the actual control
				CRect rect;
				dateTimeCtrl->GetWindowRect(&rect);
				ScreenToClient(&rect);

				COleDateTime theTime;
            
				//TD15044 - when date and time controls are empty, no date should be displayed
				// DWORD dwResult = dateTimeCtrl->GetTime(theTime);
				// bool validTime = (dwResult != FALSE);
				BOOL validTime  = dateTimeCtrl->GetTime(theTime);

				if ((dateTimeCtrl->GetStyle() & DTS_TIMEFORMAT) == DTS_TIMEFORMAT) // time -> change to edit with spinner
				{
					// dynamically create an edit control on the view
					CEdit* edit = new CEdit;
					
					// TD #11900
					// Use CreateEx() so we can get the 3d border with WS_EX_CLIENTEDGE.
					// Create() with WS_BORDER resulted in a flat, 1 pixel border
					edit->CreateEx(WS_EX_CLIENTEDGE, "Edit", "", WS_CHILD|WS_VISIBLE|WS_TABSTOP, rect, this, newResourceId);
					if (validTime)
					{
						edit->SetWindowText(theTime.Format("%H:%M:%S"));
					}
					edit->SetReadOnly(TRUE);

					CSpinButtonCtrl* spinner = new CSpinButtonCtrl;
					spinner->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|UDS_SETBUDDYINT|UDS_ALIGNRIGHT, rect, this, newResourceId);
					spinner->SetBuddy(edit);        
					spinner->EnableWindow(FALSE);

					// hide the actual control
					dateTimeCtrl->ShowWindow(SW_HIDE);    
				}
				else    // date -> change to combo
				{
					// TD #12425
					// SOLUTION 1:
					// (see above)
					dateTimeCtrl->EnableWindow(false);
					rect.top += 4;
					rect.bottom -= 2;
					rect.left +=4;
					rect.right -= 18;
					CStatic* comboText = new CStatic;
					comboText->Create(theTime.Format("%d-%b-%y"), WS_CHILD|WS_VISIBLE, rect, this, newResourceId);
				}

    			FUNCTION_EXIT;
				return;
			}    
		}
    
		// not necessary to make an assertion if the control ID is not defined
		// TA_ASSERT(false, "Don't know how to make the control read only");
	}


	//-----------------------------------------------------------------------------
	void IncidentReportDialog::getStartDateTimeFromDialog( time_t& datetimeStart ) const
	{
		FUNCTION_ENTRY("getStartDateTimeFromDialog");
		//TD14371 - changed for additional checking when saving time ++lizettejl
		//if ( GetDlgItem( IDC_TIMEPICKER_START_TIME )->IsWindowEnabled() ) 
		if(m_ctrlStartDate.IsWindowEnabled() && (m_timeStartDate.GetYear() > 0))
		{		
			datetimeStart = CTime(  m_timeStartDate.GetYear(),
				m_timeStartDate.GetMonth(),
				m_timeStartDate.GetDay(),
				m_timeStartTime.GetHour(), 
				m_timeStartTime.GetMinute(), 
				m_timeStartTime.GetSecond() ).GetTime();
			
		}
		else
		{
			datetimeStart = 0;
		}
		FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::getEndDateTimeFromDialog( time_t& datetimeEnd ) const
	{
		FUNCTION_ENTRY("getEndDateTimeFromDialog");
		//TD14371 - changed for additional checking when saving time ++lizettejl
		// if ( GetDlgItem( IDC_TIMEPICKER_END_TIME )->IsWindowEnabled() )
 		if(m_ctrlEndDate.IsWindowEnabled() && (m_timeEndDate.GetYear() > 0))  
		{
			datetimeEnd = CTime(    m_timeEndDate.GetYear(),
									m_timeEndDate.GetMonth(),
									m_timeEndDate.GetDay(),
									m_timeEndTime.GetHour(), 
									m_timeEndTime.GetMinute(), 
									m_timeEndTime.GetSecond() ).GetTime();
		}
		else
		{
			datetimeEnd = 0;
		}
		FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::enableStartDateTimeControls(bool bEnable)
	{
		FUNCTION_ENTRY("enableStartDateTimeControls");
		LONG dwStyle = GetWindowLong( m_ctrlStartDate.m_hWnd, GWL_STYLE );
		// TD14371 - initialize m_startDateEnable later as we need to use 
		// it when setting the default date for the control when enabling it
		// m_startDateEnable = bEnable;

		if ( bEnable )
		{
			//if the control is just enable, show the current time
			m_timeStartDate.SetStatus(COleDateTime::valid);
			//TD14371 - use the m_startDateEnable to check if the 
			// control is just enabled
			//if (m_timeStartDate == COleDateTime())
			if(!m_startDateEnable)
			{
				m_ctrlStartDate.SetTime(COleDateTime::GetCurrentTime());
				m_ctrlStartTime.SetTime(COleDateTime::GetCurrentTime());
			}
			//TD14371 ++lizettejl
			// change format so that the control will display the date
			//TD13540  - add constants for the date and time format
			//m_ctrlStartDate.SetFormat("dd'-'MMM'-'yyy");
			//m_ctrlStartTime.SetFormat("HH':'mm':'ss");

			// TD12474 ++
			//m_ctrlStartDate.SetFormat(DATE_FORMAT.c_str());
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat6);
			m_ctrlStartDate.SetFormat(dateFormatStr.c_str());
			// ++ TD12474 

			m_ctrlStartTime.SetFormat(TIME_FORMAT.c_str());
			//set to the current time
			m_ctrlStartTime.EnableWindow(TRUE);
			dwStyle |= DTS_SHOWNONE;
		}
		else
		{
			// display a bank instead
			DateTime_SetSystemtime( m_ctrlStartDate.m_hWnd, GDT_NONE, NULL );
			m_ctrlStartDate.SetFormat(" ");
			m_ctrlStartTime.SetFormat(" ");
			m_timeStartDate = COleDateTime::GetCurrentTime();
			m_ctrlStartTime.EnableWindow(FALSE);
			dwStyle &= ~DTS_SHOWNONE;
		}

		// TD14371 - moved initialization here 
		m_startDateEnable = bEnable;

		if ( SetWindowLong( m_ctrlStartDate.m_hWnd, GWL_STYLE, dwStyle ) == 0 )
			DWORD dwError = GetLastError();

		//m_ctrlStartDate.Invalidate();
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::enableEndDateTimeControls(bool bEnable)
	{
		FUNCTION_ENTRY("enableEndDateTimeControls");
		LONG dwStyle = GetWindowLong( m_ctrlEndDate.m_hWnd, GWL_STYLE );
		// TD14371 - initialize m_endDateEnable later as we need to use 
		// it when setting the default date for the control when enabling it
		// m_endDateEnable = bEnable;

		if ( bEnable )
		{
			//if the control is just enable, show the current time
			m_timeEndDate.SetStatus(COleDateTime::valid);
			//TD14371 - use the m_endDateEnable to check if the 
			// control is just enabled
			//if (m_timeEndDate == COleDateTime())
			if(!m_endDateEnable)
			{
				m_ctrlEndDate.SetTime(COleDateTime::GetCurrentTime());
				m_ctrlEndTime.SetTime(COleDateTime::GetCurrentTime());
			}
			//TD14371 ++lizettejl
			// change format so that the control will display the date
			//TD13540  - add constants for the date and time format
			//m_ctrlEndDate.SetFormat("dd'-'MMM'-'yyy");
			//m_ctrlEndTime.SetFormat("HH':'mm':'ss");

			// TD12474 ++
			//m_ctrlEndDate.SetFormat(DATE_FORMAT.c_str());
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat6);
			m_ctrlEndDate.SetFormat(dateFormatStr.c_str());
			// ++ TD12474
		
			m_ctrlEndTime.SetFormat(TIME_FORMAT.c_str());
			//set to the current time
			m_ctrlEndTime.EnableWindow(TRUE);
			dwStyle |= DTS_SHOWNONE;
		}
		else
		{
			// display a bank instead
			DateTime_SetSystemtime( m_ctrlEndDate.m_hWnd, GDT_NONE, NULL );
			m_ctrlEndDate.SetFormat(" ");
			m_ctrlEndTime.SetFormat(" ");
			m_timeEndDate = COleDateTime::GetCurrentTime();
			m_ctrlEndTime.EnableWindow(FALSE);
			dwStyle &= ~DTS_SHOWNONE;
		}

		// TD14371 - moved initialization here
		m_endDateEnable = bEnable;

		if ( SetWindowLong( m_ctrlEndDate.m_hWnd, GWL_STYLE, dwStyle ) == 0 )
			DWORD dwError = GetLastError();

		m_ctrlEndDate.Invalidate();
		FUNCTION_EXIT;
	}

	//-----------------------------------------------------------------------------
	void IncidentReportDialog::addPlanTypeToListBox( CListBox& lb, PlanType& planType )
	{
		FUNCTION_ENTRY("addPlanTypeToListBox");

		int iItem = lb.AddString( planType.c_str() );

		// allocate a buffer to hold the plan type then hook it to the 
		// list box item so that we can search/retrieve later
		PlanType *pPlanType = new PlanType;
		*pPlanType = planType;
		lb.SetItemData( iItem, (DWORD) pPlanType );
    
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::getPlanTypesFromListBox( CListBox &lb, vector<PlanType> &arrayPlanTypes )
	{
		FUNCTION_ENTRY("getPlanTypesFromListBox");

		arrayPlanTypes.clear();

		int nCount = lb.GetCount();
		for ( int iItem = 0; iItem < nCount; ++iItem )
		{
			PlanType *pPlanType = (PlanType *) lb.GetItemData(iItem);
			arrayPlanTypes.push_back( *pPlanType );
		}

		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::cleanUpPlanTypeListBox( CListBox &lb )
	{
		FUNCTION_ENTRY("cleanUpPlanTypeListBox");
    
		// delete all plan types hooked to the list box's items
		int nCount = lb.GetCount();
		for ( int iItem = 0; iItem < nCount; ++iItem )
		{
			// delete the plan type hooked with this item
			PlanType *pPlanType = (PlanType *) lb.GetItemData(iItem);
			delete pPlanType;
			//Bugzilla Bug 180: Mintao
			pPlanType = NULL;
		}
		// remove all strings
		lb.ResetContent();
    
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	const UINT IncidentReportDialog::WM_APP_PREPAREFORCLOSE = WM_APP + 1;
	const UINT IncidentReportDialog::WM_STATIONREPORTDLG_END = WM_APP + 2;

	//-----------------------------------------------------------------------------
	BEGIN_MESSAGE_MAP(IncidentReportDialog, CDialog)
		//{{AFX_MSG_MAP(IncidentReportDialog)
   		ON_BN_CLICKED(IDC_BUTTON_SAVE, onSave)
		ON_BN_CLICKED(IDC_BUTTON_EDIT_STATION_REPORT, onEditStationReport)
		ON_BN_CLICKED(IDC_BUTTON_VIEW_STATION_REPORT, onViewStationReport)
		ON_BN_CLICKED(IDC_BUTTON_PRINT, onPrint)
		ON_BN_CLICKED(IDC_BUTTON_ADD_PLAN, onAddPlan)
		ON_BN_CLICKED(IDC_BUTTON_ASSET, onAssetSelection)
		ON_BN_CLICKED(IDCANCEL, onCancel)
		ON_MESSAGE( WM_APP_PREPAREFORCLOSE, OnPrepareForClose )
		ON_MESSAGE( WM_STATIONREPORTDLG_END, OnStationReportDlgEnd ) //TD13867  
		ON_BN_CLICKED(IDC_BUTTON_REMOVE_PLAN, onRemovePlan)
		ON_LBN_SELCHANGE(IDC_LIST_PLAN, OnListPlanSelChange)
		ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATEPICKER_END_DATE, OnEndDateChange)
		ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATEPICKER_START_DATE, OnStartDateChange)
		ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIMEPICKER_START_TIME, OnStartTimeChange)
		ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIMEPICKER_END_TIME, OnEndTimeChange)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::OnStartDateChange(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY("OnStartDateChange");
		SYSTEMTIME sysTime;
		enableStartDateTimeControls( m_ctrlStartDate.GetTime(&sysTime) == GDT_VALID );
		SetTimeRange(false,true); //TD18725

		*pResult = 0;
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::OnEndDateChange(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY("OnEndDateChange");
		SYSTEMTIME sysTime;
		enableEndDateTimeControls( m_ctrlEndDate.GetTime(&sysTime) == GDT_VALID );
		SetTimeRange(true,true); //TD18725

		*pResult = 0;
		FUNCTION_EXIT;
	}
	void IncidentReportDialog::OnStartTimeChange(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY("OnEndDateChange");
		SYSTEMTIME sysTime;
		enableEndDateTimeControls( m_ctrlEndDate.GetTime(&sysTime) == GDT_VALID );

		SetTimeRange(false,false); //TD18725

		*pResult = 0;
		FUNCTION_EXIT;
	}
	void IncidentReportDialog::OnEndTimeChange(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY("OnEndDateChange");
		SYSTEMTIME sysTime;
		enableEndDateTimeControls( m_ctrlEndDate.GetTime(&sysTime) == GDT_VALID );

		SetTimeRange(true,false); //TD18725
		*pResult = 0;
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY("DoDataExchange");
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(IncidentReportDialog)
		DDX_Control(pDX, IDC_COMBOSTATIC2, m_staticLocation2);
		DDX_Control(pDX, IDC_COMBOSTATIC1, m_staticLocation1);
		DDX_Control(pDX, IDC_START_TIMESTATIC, m_staticStartTime);
		DDX_Control(pDX, IDC_START_DATESTATIC, m_staticStartDate);
		DDX_Control(pDX, IDC_END_TIMESTATIC, m_staticEndTime);
		DDX_Control(pDX, IDC_END_DATESTATIC, m_staticEndDate);
		DDX_Control(pDX, IDC_BUTTON_ADD_PLAN, m_buttonAddPlan);
		DDX_Control(pDX, IDC_BUTTON_ASSET, m_buttonAsset);
		DDX_Control(pDX, IDC_BUTTON_SAVE, m_buttonSave);
		DDX_Control(pDX, IDC_BUTTON_REMOVE_PLAN, m_buttonRemovePlan);
		DDX_Control(pDX, IDC_BUTTON_PRINT, m_buttonPrint);
		DDX_Control(pDX, IDC_BUTTON_VIEW_STATION_REPORT, m_buttonViewStationReport);
		DDX_Control(pDX, IDC_BUTTON_EDIT_STATION_REPORT, m_buttonEditStationReport);
		DDX_Control(pDX, IDC_LIST_PLAN, m_listPlan);
		DDX_Text(pDX, IDC_EDIT_TITLE, m_sTitle);
		DDV_MaxChars(pDX, m_sTitle, 100);  // (TD13865) 
		DDX_Text(pDX, IDC_EDIT_TRAIN_ID, m_sTrain);
		DDV_MaxChars(pDX, m_sTrain, 10); // (TD13865)
		DDX_Text(pDX, IDC_EDIT_RECOMMENDED_ACTION, m_sRecommendedAction);
	DDV_MaxChars(pDX, m_sRecommendedAction, 1200);// (TD15870)
 		DDX_Text(pDX, IDC_EDIT_OPERATORS, m_sOperator);
		DDV_MaxChars(pDX, m_sOperator, 500); // (TD13865)
		DDX_Text(pDX, IDC_EDIT_COMMENTS, m_sComment);
	DDV_MaxChars(pDX, m_sComment, 1200);// (TD15870)
		DDX_Text(pDX, IDC_EDIT_ASSET, m_sAsset);
		DDX_Text(pDX, IDC_EDIT_CREATOR, m_sCreator);
		DDX_Text(pDX, IDC_EDIT_CREATOR_PROFILE, m_sCreatorProfile);
		DDX_Text(pDX, IDC_EDIT_CREATION_DATE, m_sCreationDate);
		DDX_Text(pDX, IDC_EDIT_CREATION_TIME, m_sCreationTime);
		DDX_Text(pDX, IDC_EDIT_MODIFIER, m_sModifier);
		DDX_Text(pDX, IDC_EDIT_MODIFIED_DATE, m_sModifiedDate);
		DDX_Text(pDX, IDC_EDIT_MODIFIED_TIME, m_sModifiedTime);
		DDX_Text(pDX, IDC_EDIT_MODIFIER_PROFILE, m_sModifierProfile);
		DDX_Check(pDX, IDC_CHECK_FALSE_INCIDENT, m_bFalseIncident);
		DDX_Check(pDX, IDC_CHECK_INNER_TRACK, m_bInnerTrack);
		DDX_Check(pDX, IDC_CHECK_OUTER_TRACK, m_bOuterTrack);
		DDX_DateTimeCtrl(pDX, IDC_DATEPICKER_START_DATE, m_timeStartDate);
		DDX_DateTimeCtrl(pDX, IDC_DATEPICKER_END_DATE, m_timeEndDate);
		DDX_DateTimeCtrl(pDX, IDC_TIMEPICKER_END_TIME, m_timeEndTime);
		DDX_DateTimeCtrl(pDX, IDC_TIMEPICKER_START_TIME, m_timeStartTime);
		DDX_Text(pDX, IDC_EDIT_ASSOCIATED_ALARM, m_sAssociatedAlarm);
		DDX_Text(pDX, IDC_EDIT_ADDITIONAL_ALARM_DESCRIPTIONS, m_sAdditionalAlarmDescription);
		DDV_MaxChars(pDX, m_sAdditionalAlarmDescription, 500);
		DDX_CBString(pDX, IDC_COMBO_LOCATION1, m_sLocation1);
		DDX_CBString(pDX, IDC_COMBO_LOCATION2, m_sLocation2);
   		DDX_Control(pDX, IDC_DATEPICKER_START_DATE, m_ctrlStartDate);
		DDX_Control(pDX, IDC_DATEPICKER_END_DATE, m_ctrlEndDate);
		DDX_Control(pDX, IDC_TIMEPICKER_START_TIME, m_ctrlStartTime);
		DDX_Control(pDX, IDC_TIMEPICKER_END_TIME, m_ctrlEndTime);
		DDX_Text(pDX, IDC_EDIT_ID, m_uID); //++lizettejl
		DDX_Text(pDX, IDC_EDIT_KILOMETRE_LOSS, m_uKilometreLoss);
		DDV_MaxChars(pDX, m_uKilometreLoss, 9);
		DDX_Text(pDX, IDC_EDIT_ACCUMULATED_DELAY_TIME_HOURS, m_uAccumulatedDelayHours);
		DDV_MaxChars(pDX, m_uAccumulatedDelayHours, 3);//TD-15805
		DDX_Text(pDX, IDC_EDIT_ACCUMULATED_DELAY_TIME_MINUTES, m_uAccumulatedDelayMinutes);
		DDV_MaxChars(pDX, m_uAccumulatedDelayMinutes, 2);
		DDX_Text(pDX, IDC_EDIT_ACCUMULATED_DELAY_TIME_SECONDS, m_uAccumulatedDelaySeconds);
		DDV_MaxChars(pDX, m_uAccumulatedDelaySeconds, 2);
		DDX_Text(pDX, IDC_EDIT_INITIAL_DELAY_TIME_HOURS, m_uInitialDelayHours);
		DDV_MaxChars(pDX, m_uInitialDelayHours, 3);//TD-15805
		DDX_Text(pDX, IDC_EDIT_INITIAL_DELAY_TIME_MINUTES, m_uInitialDelayMinutes);
		DDV_MaxChars(pDX, m_uInitialDelayMinutes, 2);
		DDX_Text(pDX, IDC_EDIT_INITIAL_DELAY_TIME_SECONDS, m_uInitialDelaySeconds);
		DDV_MaxChars(pDX, m_uInitialDelaySeconds, 2);
	//}}AFX_DATA_MAP
		FUNCTION_EXIT;
	}

	IncidentReportDialog::IncidentReportDialog( const ItaIncidentReportData *report, 
												bool editable, 
												ItaIncidentRights& incidentRights, 
												ItaLocationAccessFactory& locationAccessFactory, 
												TA_Base_Bus::ItaUserMessage& userMessage,
												CWnd *pParent ) :
						CDialog( IncidentReportDialog::IDD, pParent ), 
						m_bEditable( editable ),
						m_incidentRights( incidentRights ),
						m_locationAccessFactory( locationAccessFactory ),
						m_userMessage( userMessage ),
						m_StationReportDialog(NULL)
	{
		FUNCTION_ENTRY("IncidentReportDialog");

		if(report != NULL)
		{
			m_report = report->clone();
		}
		else
		{
        
			CWnd* app = FindWindow(NULL, _T("IncidentManager"));
			CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(app);
			//Bugzilla Bug 180: Mintao
			TA_ASSERT(pMainFrame != NULL, "Initialize Incident Dialog has NULL pointer CMainFrame");
			CIncidentManagerListView *pListView = (CIncidentManagerListView *) pMainFrame->GetActiveView();
			TA_ASSERT( pListView != NULL, "CView is NULL in prepareForClose!")
			// get document from main frame
			IncidentManagerDoc * pDoc = (IncidentManagerDoc *) pListView->GetDocument();
			m_report = pDoc->getIncidentReportFactory().createIncidentReport();
		}
    
		m_alarmId = "";
		// The rest will be done at OnInitDialog()
		FUNCTION_EXIT;
	}

	IncidentReportDialog::IncidentReportDialog( const std::string& alarmId, 
												const std::string &alarmValue,
												bool editable, 
												ItaIncidentRights& incidentRights, 
												ItaLocationAccessFactory& locationAccessFactory,
												TA_Base_Bus::ItaUserMessage& userMessage,
												CWnd *pParent ):
						CDialog( IncidentReportDialog::IDD, pParent ),
						m_incidentRights( incidentRights ),
						m_locationAccessFactory( locationAccessFactory ),
						m_userMessage( userMessage ),
						m_StationReportDialog(NULL),
						m_alarmId(alarmId),
						m_alarmValue(alarmValue),
						m_bEditable(editable)
	{
		FUNCTION_ENTRY("IncidentReportDialog");

			CWnd* app = FindWindow(NULL, _T("IncidentManager"));
			CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(app);
			//Bugzilla Bug 180: Mintao
			TA_ASSERT(pMainFrame != NULL, "Initialize Incident Dialog has NULL pointer CMainFrame");
			CIncidentManagerListView *pListView = (CIncidentManagerListView *) pMainFrame->GetActiveView();
			TA_ASSERT( pListView != NULL, "CView is NULL in prepareForClose!")
			// get document from main frame
			IncidentManagerDoc * pDoc = (IncidentManagerDoc *) pListView->GetDocument();
			m_report = pDoc->getIncidentReportFactory().createIncidentReport(alarmId, alarmValue);

		 // The rest will be done at OnInitDialog() automatically
		FUNCTION_EXIT;
	}

	IncidentReportDialog::~IncidentReportDialog()
	{
		FUNCTION_ENTRY("~IncidentReportDialog");
		delete m_report;
		m_report = NULL;

		FUNCTION_EXIT;
	}

	BOOL IncidentReportDialog::OnInitDialog() 
	{
		FUNCTION_ENTRY( "OnInitDialog" );

		//{{AFX_DATA_INIT(IncidentReportDialog)
		m_sTitle = _T("");
		m_sTrain = _T("");
		m_sRecommendedAction = _T("");
		m_sOperator = _T("");
		m_sComment = _T("");
		m_sAsset = _T("");
		m_sCreator = _T("");
		m_sCreatorProfile = _T("");
		m_sCreationDate = _T("");
		m_sCreationTime = _T("");
		m_sModifier = _T("");
		m_sModifiedDate = _T("");
		m_sModifiedTime = _T("");
		m_sModifierProfile = _T("");
		m_bFalseIncident = FALSE;
		m_bInnerTrack = FALSE;
		m_bOuterTrack = FALSE;
		m_timeStartDate = COleDateTime::GetCurrentTime();
		m_timeEndDate = COleDateTime::GetCurrentTime();    
		m_timeEndTime = COleDateTime::GetCurrentTime();
		m_timeStartTime = COleDateTime::GetCurrentTime();
		m_sAssociatedAlarm = _T("");
		m_sAdditionalAlarmDescription = _T("");
		m_sLocation1 = _T("");  
		m_sLocation2 = _T("");  
		//TD15204 - reverting back the changes of TD15041
		m_uID = _T(""); //++lizettejl
		m_uKilometreLoss = _T("");
		m_uAccumulatedDelayHours = _T("");
		m_uAccumulatedDelayMinutes = _T("");
		m_uAccumulatedDelaySeconds = _T("");
		m_uInitialDelayHours = _T("");
		m_uInitialDelayMinutes = _T("");
		m_uInitialDelaySeconds = _T("");
		//}}AFX_DATA_INIT
		CDialog::OnInitDialog();
		// set text limits for edit controls
		((CEdit*) GetDlgItem(IDC_EDIT_TITLE))->SetLimitText(100);
		((CEdit*) GetDlgItem(IDC_EDIT_TRAIN_ID))->SetLimitText(10);
		((CEdit*) GetDlgItem(IDC_EDIT_ADDITIONAL_ALARM_DESCRIPTIONS))->SetLimitText(500);
		((CEdit*) GetDlgItem(IDC_EDIT_OPERATORS))->SetLimitText(500);
		((CEdit*) GetDlgItem(IDC_EDIT_COMMENTS))->SetLimitText(1200);
		((CEdit*) GetDlgItem(IDC_EDIT_RECOMMENDED_ACTION))->SetLimitText(1200);
		((CEdit*) GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_HOURS))->SetLimitText(3);//TD-15805
		((CEdit*) GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_MINUTES))->SetLimitText(2);
		((CEdit*) GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_SECONDS))->SetLimitText(2);
		((CEdit*) GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_HOURS))->SetLimitText(3);//TD-15805
		((CEdit*) GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_MINUTES))->SetLimitText(2);
		((CEdit*) GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_SECONDS))->SetLimitText(2);
		
		//
		// populate Location1 and Location2 combo boxes
		//
		//vector<string*> arrayLocations = m_locationAccessFactory.getAllLocations();
		//++Support Location Display name
		m_mapLocations = m_locationAccessFactory.getAllLocations();

		CComboBox *pcomboboxLocation1 = (CComboBox *) GetDlgItem( IDC_COMBO_LOCATION1 );
		CComboBox *pcomboboxLocation2 = (CComboBox *) GetDlgItem( IDC_COMBO_LOCATION2 );
		//Bugzilla Bug 180: Mintao
		TA_ASSERT(pcomboboxLocation1 != NULL, "refreshDisplay has NULL pointer CComboBox");
		pcomboboxLocation1->ResetContent();
		//Bugzilla Bug 180: Mintao
		TA_ASSERT(pcomboboxLocation2 != NULL, "refreshDisplay has NULL pointer CComboBox");
		pcomboboxLocation2->ResetContent();

		//++Support Location Display name
		T_LocDisplayMap::iterator itr = m_mapLocations.begin();
		for( itr; itr!=m_mapLocations.end(); itr++ )
		{
			pcomboboxLocation1->AddString( itr->second.c_str() );
			pcomboboxLocation2->AddString( itr->second.c_str() );
		}


		// set values for controls
		refreshDisplay();

		FUNCTION_EXIT;

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::refreshDisplay()
	{
		FUNCTION_ENTRY("refreshDisplay");
		// set values for all controls
		//Bugzilla Bug 180: Mintao
		TA_ASSERT(m_report != NULL, "refreshDisplay has NULL pointer ItaIncidentReportData");
		//Mintao ++
		//Bug 652 (TD12313)
		if (m_report->isNew())
		{
			m_sCreator = "";
			m_sCreatorProfile = "";
			m_sCreationDate = "";
			m_sCreationTime = "";
		}
		else
		{
			m_sCreator = m_report->getCreator().c_str();
			m_sCreatorProfile = m_report->getCreatorProfile().c_str();
        
			COleDateTime createDate(m_report->getCreatedDateTime());
			m_sCreationDate = createDate.Format("%d-%b-%Y");

			COleDateTime createTime(m_report->getCreatedDateTime());
			m_sCreationTime = createTime.Format("%H:%M:%S");
		 
		}
		//Mintao++
		//Bug 646 
    
		//TD15204 - reverting back the changes of TD15041
		//ID of the incident report
		//m_uID = CString(m_report->getReportId().c_str()); //++lizettejl

		//Shiva++
		//DTL-1174 - display of 7 digit number
		//m_uID = CString(m_report->getDisplayReportId().c_str());

		///Shiva++
		///Changed to save 7 digit number to database also
		/// Assumed incident report will be created only in OCC location.
		/// So only one location and not possible to have same id for different location suitation.
		m_uID = CString(m_report->getReportId().c_str());
		///Shiva++

		//Title of the incident report
		//Mintao ++
		m_sTitle = m_report->getTitle().c_str();
    
		//Alarm information of the incident report
		m_sAssociatedAlarm = m_report->getAssociatedAlarmDetail().c_str();
		m_sAssociatedAlarm.Replace("OMFlag:N,","");//TD 15870


		m_sAdditionalAlarmDescription= m_report->getAdditionalAlarmDescription().c_str();
		//Mintao++
		//Bug 646 
		//Modifier of the incident report
		m_sModifier = m_report->getModifier().c_str();

		//Mintao ++
		//Bug 647
		//Modifier Profile, Modifier Data/Time of the incident report
		if ( m_sModifier.GetLength() )
		{
			m_sModifierProfile = m_report->getModifierProfile().c_str();
			COleDateTime modifiedDate(m_report->getModifiedDateTime());
			m_sModifiedDate = modifiedDate.Format("%d-%b-%Y");;

			COleDateTime modifiedTime(m_report->getModifiedDateTime());
			m_sModifiedTime = modifiedTime.Format("%H:%M:%S");
		}
		//Mintao ++
		//Bug 647

		//False incident of the incident report
		m_bFalseIncident = m_report->isFalseIncident();
    

		//Asset of the incident report 	
		m_Asset = m_report->getAsset();   
		m_sAsset = m_Asset.description.c_str();

		m_sTrain = m_report->getTrain().c_str();
		m_bInnerTrack = m_report->getInnerTrack();
		m_bOuterTrack = m_report->getOuterTrack();
		ostringstream kmlossstr;
		kmlossstr << m_report->getKilometreLoss();
		m_uKilometreLoss = CString(kmlossstr.str().c_str());

		//TD14370 - location can be empty from database
		T_LocDisplayMap::iterator itFind1 = m_mapLocations.find(m_report->getLocation1());
		T_LocDisplayMap::iterator itFind2 = m_mapLocations.find(m_report->getLocation2());
		m_sLocation1 = itFind1!=m_mapLocations.end()?itFind1->second.c_str():"";
		m_sLocation2 = itFind2!=m_mapLocations.end()?itFind2->second.c_str():"";

		bool bStartDateTimeIsNULL = false;
		bool bEndDateTimeIsNULL = false;

		// change format so that the control will display the date
		//TD13540  - add constants for the date and time format

		// TD12474 ++
		//m_ctrlEndDate.SetFormat(DATE_FORMAT.c_str());
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat6);
		m_ctrlEndDate.SetFormat(dateFormatStr.c_str());
		// ++ TD12474
		m_ctrlStartTime.SetFormat(TIME_FORMAT.c_str());
		if (m_report->getStartDateTime() != 0)
		{
			m_timeStartDate = m_timeStartTime = COleDateTime(m_report->getStartDateTime());
		}
		else
		{		
			bStartDateTimeIsNULL = true;
			//TD15044 - removed initialization of date to the
			// current date when it is empty in the first place ++lizettejl
		}
		// TD12474 ++
		dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat6);
		m_ctrlStartDate.SetFormat(dateFormatStr.c_str());
		// ++ TD12474
    
		m_ctrlEndTime.SetFormat(TIME_FORMAT.c_str());
		if (m_report->getEndDateTime() != 0)
		{
			m_timeEndDate = m_timeEndTime = COleDateTime(m_report->getEndDateTime());
		}
		else
		{
			bEndDateTimeIsNULL = true;
			//TD15044 - removed initialization of date to the
			// current date when it is empty in the first place ++lizettejl
		}

		//Mintao++
		//Bug 758 
		ostringstream delayhourstr;
		delayhourstr << (m_report->getInitialDelay() / 3600);
		m_uInitialDelayHours = CString(delayhourstr.str().c_str());   
    
		ostringstream delayminstr;
		delayminstr << ((m_report->getInitialDelay() % 3600 ) / 60);
		m_uInitialDelayMinutes = delayminstr.str().c_str();

		ostringstream delaysecstr;
		delaysecstr << (m_report->getInitialDelay() % 60);
		m_uInitialDelaySeconds = delaysecstr.str().c_str();


    
		ostringstream accumhourstr;
		accumhourstr << (m_report->getAccumulatedDelay() / 3600);
		m_uAccumulatedDelayHours = accumhourstr.str().c_str();

		ostringstream accumminstr;
		accumminstr << ((m_report->getAccumulatedDelay() % 3600 ) / 60);
		m_uAccumulatedDelayMinutes =  accumminstr.str().c_str();

		ostringstream accumsecstr;
		accumsecstr << (m_report->getAccumulatedDelay() % 60);
		m_uAccumulatedDelaySeconds = accumsecstr.str().c_str();
		//Mintao++
		//Bug 758 
		{
			// plans executed list box
			// clean up first
			m_listPlan.ResetContent();
			vector<PlanType> arrayPlanTypes = m_report->getPlansExecuted();
			int nCount = arrayPlanTypes.size();
			for ( int i = 0; i < nCount; ++i )
				addPlanTypeToListBox( m_listPlan, arrayPlanTypes[i] );
			//Bugzilla Bug 170: Mintao
			if (m_bEditable)
			{
				m_buttonRemovePlan.EnableWindow( m_listPlan.GetCount() != 0 );
			}
			else
			{
				m_buttonRemovePlan.EnableWindow(FALSE);   
			}
		}

		m_sOperator = m_report->getOperators().c_str();
		m_sComment = m_report->getComments().c_str();
		m_sRecommendedAction = m_report->getRecommendation().c_str();

		UpdateData(FALSE);

		if ( m_report->isNew() )
		{
			m_buttonEditStationReport.EnableWindow(FALSE);
			m_buttonViewStationReport.EnableWindow(FALSE);
		}
		else
		{
			//Mintao++
			//Bug 778(TD12858)
			if (m_incidentRights.isViewPermitted(m_report->getStationReport()) && (!m_report->getStationId().empty()))
			{
				m_buttonViewStationReport.EnableWindow(TRUE);
			}
			else
			{
				m_buttonViewStationReport.EnableWindow(FALSE);
			}
			//Mintao++
			//Bug 778(TD12858)
			m_buttonEditStationReport.EnableWindow( m_incidentRights.isControlPermitted( &m_report->getStationReport() ) );
		}

		if ( !m_bEditable )
		{
			// make all controls read-only
			setReadOnly(IDC_EDIT_TITLE, IS_EDIT_BOX);
			setReadOnly(IDC_CHECK_FALSE_INCIDENT, IS_CHECK_OR_RADIO_BUTTON);
			setReadOnly(IDC_EDIT_ASSET, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_TRAIN_ID, IS_EDIT_BOX);
			setReadOnly(IDC_CHECK_INNER_TRACK, IS_CHECK_OR_RADIO_BUTTON);
			setReadOnly(IDC_CHECK_OUTER_TRACK, IS_CHECK_OR_RADIO_BUTTON);
			setReadOnly(IDC_EDIT_KILOMETRE_LOSS, IS_EDIT_BOX);

			// call one function instead
			setControlsToReadOnly();
			// ++lizettejl
			setReadOnly(IDC_EDIT_INITIAL_DELAY_TIME_HOURS, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_INITIAL_DELAY_TIME_MINUTES, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_INITIAL_DELAY_TIME_SECONDS, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_ACCUMULATED_DELAY_TIME_HOURS, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_ACCUMULATED_DELAY_TIME_MINUTES, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_ACCUMULATED_DELAY_TIME_SECONDS, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_ASSOCIATED_ALARM, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_ADDITIONAL_ALARM_DESCRIPTIONS, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_OPERATORS, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_COMMENTS, IS_EDIT_BOX);
			setReadOnly(IDC_EDIT_RECOMMENDED_ACTION, IS_EDIT_BOX);
			//Bugzilla Bug 170: Mintao
			//disable the asset button
			m_buttonAsset.EnableWindow(FALSE);
			// disable the Save, Add/Remove Plan, Edit Station Report button
			m_buttonSave.EnableWindow(FALSE);
			m_buttonAddPlan.EnableWindow(FALSE);
			m_listPlan.EnableWindow(FALSE); //TD15807
			m_buttonRemovePlan.EnableWindow(FALSE);
			//Mintao ++
			//Bug 771(TD12948)
			//Mintao++
			//Bug 778(TD12858)
			if (m_incidentRights.isViewPermitted(m_report->getStationReport()) && (!m_report->getStationId().empty()))
			{
				m_buttonViewStationReport.EnableWindow(TRUE);
			}
			else
			{
				m_buttonViewStationReport.EnableWindow(FALSE);
			}
			//TD18717, Hongzhi, now station report rights have nothing do do with incident rights
			m_buttonEditStationReport.EnableWindow( m_incidentRights.isControlPermitted( &m_report->getStationReport() ) );
			//Mintao++
			//Bug 778(TD12858)
			//Mintao ++
			//Bug 771(TD12948)
		}
		else
		{
			GetDlgItem( IDC_EDIT_TITLE )->EnableWindow(TRUE);
			GetDlgItem( IDC_CHECK_FALSE_INCIDENT )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_ASSET )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_TRAIN_ID )->EnableWindow(TRUE );
			GetDlgItem( IDC_CHECK_INNER_TRACK )->EnableWindow(TRUE );
			GetDlgItem( IDC_CHECK_OUTER_TRACK )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_KILOMETRE_LOSS )->EnableWindow(TRUE );
			GetDlgItem( IDC_COMBO_LOCATION1 )->EnableWindow(TRUE );
			GetDlgItem( IDC_COMBO_LOCATION2 )->EnableWindow(TRUE );
			GetDlgItem( IDC_DATEPICKER_START_DATE )->EnableWindow(TRUE );
			GetDlgItem( IDC_TIMEPICKER_START_TIME )->EnableWindow(TRUE );
			GetDlgItem( IDC_DATEPICKER_END_DATE )->EnableWindow(TRUE );
			GetDlgItem( IDC_TIMEPICKER_END_TIME )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_INITIAL_DELAY_TIME_HOURS )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_INITIAL_DELAY_TIME_MINUTES )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_INITIAL_DELAY_TIME_SECONDS )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_ACCUMULATED_DELAY_TIME_HOURS )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_ACCUMULATED_DELAY_TIME_MINUTES )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_ACCUMULATED_DELAY_TIME_SECONDS )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_ASSOCIATED_ALARM )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_ADDITIONAL_ALARM_DESCRIPTIONS )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_OPERATORS )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_COMMENTS )->EnableWindow(TRUE );
			GetDlgItem( IDC_EDIT_RECOMMENDED_ACTION )->EnableWindow(TRUE );
			m_buttonAsset.EnableWindow(TRUE);
			// enable the Save, Add/Remove Plan, Edit Station Report button
			//Mintao ++
			//Bug 651
			if ( !m_report->isNew() )
			{
				m_buttonSave.EnableWindow(TRUE);
				m_buttonPrint.EnableWindow(TRUE);
				//Mintao ++
				//Bug 771(TD12948)
				if (m_incidentRights.isViewPermitted(m_report->getStationReport()) && (!m_report->getStationId().empty()))
				{
					m_buttonViewStationReport.EnableWindow(TRUE);
				}
				else
				{
					m_buttonViewStationReport.EnableWindow(FALSE);
				}
				m_buttonEditStationReport.EnableWindow( m_incidentRights.isControlPermitted( &m_report->getStationReport() ) );
				//Mintao ++
				//Bug 771(TD12948)
			}
			//Mintao ++
			//Bug 651
			m_buttonAddPlan.EnableWindow(TRUE);
			m_listPlan.EnableWindow(TRUE); //TD15807
		}

		if ( m_report->isNew() && m_bEditable )
		{
			// disable the Start/End Date/Time controls until user selects one day
			enableStartDateTimeControls(false);
			enableEndDateTimeControls(false);
		}

		// disable Start/End date/time controls if given date/time is NULL
		if ( bStartDateTimeIsNULL )
		{
			enableStartDateTimeControls(false);
		}

		if ( bEndDateTimeIsNULL )
		{
			enableEndDateTimeControls(false);
		}
		if (m_report->isNew())
		{
			//Begin--TD18725
			m_bIsReportNew = true; 
			CTime CurrentTime;
			CurrentTime = CTime::GetCurrentTime();
			m_ctrlStartDate.SetRange(&Time_Lower_Limit,&CurrentTime);
			m_ctrlStartTime.SetRange(&Time_Lower_Limit,&CurrentTime);
			m_ctrlEndDate.SetRange(&Time_Lower_Limit,&CurrentTime);
			m_ctrlEndTime.SetRange(&Time_Lower_Limit,&CurrentTime);
			m_ctrlStartDate.SetTime(&CurrentTime);
			m_ctrlStartTime.SetTime(&CurrentTime);
			m_ctrlEndDate.SetTime(&CurrentTime);
			m_ctrlEndTime.SetTime(&CurrentTime);
		}
		else
		{
			m_bIsReportNew = false;
			CTime reportTime(m_report->getCreatedDateTime());
			m_ctrlStartDate.SetRange(&Time_Lower_Limit,&reportTime);
			m_ctrlStartTime.SetRange(&Time_Lower_Limit,&reportTime);
			m_ctrlEndDate.SetRange(&Time_Lower_Limit,&reportTime);
			m_ctrlEndTime.SetRange(&Time_Lower_Limit,&reportTime);
			//End-- TD18725
		}
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::onEditStationReport()
	{
		FUNCTION_ENTRY("onEditStationReport");
		//Bugzilla Bug 180: Mintao
		TA_ASSERT(m_report != NULL, "onEditStationReport has NULL pointer ItaIncidentReportData");

		// lizettejl - TD13867 
		if (m_StationReportDialog != NULL)
		{
			delete m_StationReportDialog;
		}

		m_StationReportDialog = new StationReportDialog(m_report->getStationReport(),
									true,	// can edit station report
									m_incidentRights,
									m_userMessage,
									this);
		//TD18725 LiDan  29,5,2007
		//m_StationReportDialog->m_DatePickerControl.SetRange(&CTime(m_report->getStartDateTime()),&CTime(m_report->getEndDateTime()));
		m_StationReportDialog->Create(IDD_STATION_REPORT_DIALOG,this);
		m_StationReportDialog->ShowWindow(SW_SHOW);
		m_StationReportDialog->m_DatePickerControl.SetRange(&CTime(m_report->getStartDateTime()),&CTime(m_report->getEndDateTime()));//TD18908;
		this->EnableWindow(FALSE); 
 		

		// lizettejl - TD13867 

		// lizettejl - removed the following lines because these will cause the program to crash when 
		// the user tries to close the main application with a Station Report Dialog still visible
		/*
		StationReportDialog dlg(m_report->getStationReport(), 
								m_bEditable, 
								m_incidentRights, 
								m_userMessage, 
								this );

		m_StationReportDialog = &dlg;
		if (dlg.DoModal() == IDOK)
		{
			try
			{
				m_report->updateStationReportId(m_report->getStationReport().getReportId());
			}
			catch (TransactiveException& e)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Transactive Exception: %s caught in IncidentReportDialog::onEditStationReport()!", e.what());    
			}
		}
		*/
		// lizettejl - TD13867

		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::onViewStationReport()
	{
		FUNCTION_ENTRY("onViewStationReport");
		//Bugzilla Bug 180: Mintao
		TA_ASSERT(m_report != NULL, "onEditStationReport has NULL pointer ItaIncidentReportData");
		// Opens the station report dialog in read only mode.

		// lizettejl - TD13867 
		if (m_StationReportDialog != NULL)
		{
			delete m_StationReportDialog;
		}

		m_StationReportDialog = new StationReportDialog(m_report->getStationReport(),
									false,          // when view, cannot edit
									m_incidentRights,
									m_userMessage,
									this);

		m_StationReportDialog->Create(IDD_STATION_REPORT_DIALOG,this);
		m_StationReportDialog->ShowWindow(SW_SHOW);
		this->EnableWindow(FALSE); 

		// lizettejl - TD13867 

		// lizettejl - removed the following lines because these will cause the program to crash when 
		// the user tries to close the main application with a Station Report Dialog still visible
		/*
		StationReportDialog dlg(m_report->getStationReport(),
								false,          // view only
								m_incidentRights, 
								m_userMessage,
								this);

		m_StationReportDialog = &dlg;
		dlg.DoModal();
		*/

		FUNCTION_EXIT;
	}

	void IncidentReportDialog::onPrint()
	{
		FUNCTION_ENTRY("onPrint");
		try
		{
			//Mintao ++
			//Bug 643
			string dbstring = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTION);
			if (dbstring.empty())
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Can not get the DB connection string.");
				return;
			}

			//libo
			string dbConectinFile = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
			if (dbConectinFile.empty())
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Can not get the DB connection file.");
				return;
			}
			//libo
        
			//get the path from the ctaRuntime 
			std::string pathname = TA_Base_Core::RunParams::getInstance().get(RPARAM_REPORTTEMPLATE.c_str());
        
			if (pathname.empty())
			{
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"Error occurs while trying get the template path for the report template. Can not print!");
				return;
			}

			ostringstream idstring;
			idstring << m_report->getReportId();
			string idtest = idstring.str();

			//get the actual run parameters for the reporting manager
			string appparam = STRING_TO_LAUNCH_REPORT_MANAGER + FILE_NAME_PARAM + pathname + REPORT_ID_PARAM + idstring.str()  + DB_CONNECTION_STRING + dbstring  + DB_CONNECTION_FILE + dbConectinFile;
			CPoint pt(GetMessagePos());
			RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_GenericGui::SCREEN_CURRENT, TA_GenericGui::AREA_SCHEMATIC, pt.x);
			TA_Base_Bus::AppLauncher::getInstance().launchApplication(REPORT_MANAGER_GUI_APPTYPE, 
																	   appparam,
																	   TA_GenericGui::POS_NONE,
																	   TA_GenericGui::ALIGN_HORIZONTAL,
																	   NULL,
																	   &boundary);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Launch Reporting Manager for printing Incident ID: %d", m_report->getReportId());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ReportManger_Paramter = %s",appparam.c_str());
			//Mintao ++
			//Bug 643
		}
		catch (exception& e)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"Error(%s) occurs while trying to print the incident report.", e.what());
		}

		FUNCTION_EXIT;
	}

	void IncidentReportDialog::onAddPlan()
	{
		FUNCTION_ENTRY("onAddPlan");

		PlanDialog dlg( m_bEditable, this );
		if (dlg.DoModal() == IDOK)
		{
			PlanType planType = dlg.getSelectedPlanType();

			// check if given plan type is already in the list?
			bool bFound  = false;
			int nCount = m_listPlan.GetCount();
			for ( int iItem = 0; iItem < nCount; ++iItem )
			{
				PlanType *pPlanType = (PlanType *) m_listPlan.GetItemData(iItem);
				//Bugzilla Bug 180: Mintao
				TA_ASSERT(pPlanType != NULL, "onAddPlan has NULL pointer PlanType");
				if ( pPlanType->compare(planType) == 0 )
				{
					bFound = true;
					break;
				}
			}

			if ( !bFound )  // not in the list box
			{
				// add the given plan into the list box
				addPlanTypeToListBox( m_listPlan, planType );
			}
		}   

		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::onRemovePlan() 
	{
		FUNCTION_ENTRY("onRemovePlan");

		// get current selected item
		int iItem = m_listPlan.GetCurSel();
		if(LB_ERR != iItem)//make sure the selection is there
		{

			// delete the plan type hooked with this item
			PlanType *pPlanType = (PlanType *) m_listPlan.GetItemData(iItem);
			delete pPlanType;
			//Bugzilla Bug 180: Mintao
			pPlanType =NULL;

			// remove the item from the list box
			m_listPlan.DeleteString(iItem);
			//Bugzilla Bug 170: Mintao
			if (m_bEditable)
			{
				m_buttonRemovePlan.EnableWindow( m_listPlan.GetCount() != 0 );
			}
		}

		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	//
	// Opens up the asset selection dialog
	//
	void IncidentReportDialog::onAssetSelection()
	{
		FUNCTION_ENTRY("onAssetSelection");
		m_Asset = m_report->getAsset();

		AssetDialog dlg( m_bEditable, m_Asset, this );
		if ( dlg.DoModal() == IDOK )
		{
			m_Asset = dlg.getSelectedAsset();

			// update the control			
			 m_sAsset = m_Asset.description.c_str();			
			//Bugzilla Bug 166: Mintao
			GetDlgItem(IDC_EDIT_ASSET)->SetWindowText(m_sAsset);
		}
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	//
	// Saves the incident report.
	//	void onSave();
	//
	void IncidentReportDialog::onSave()
	{
		FUNCTION_ENTRY("onSave");

		if ( !m_incidentRights.isControlPermitted( m_report ) )
		{
			CString action = "save the incident report";
			TransActiveMessage userMsg;
			userMsg << action;
			UINT response = userMsg.showMsgBox(IDS_UE_010021);
			FUNCTION_EXIT;
			return;
		}
    
		UpdateData(); // transfer values to variables

		//Mintao ++
		//Bug 746
		if (m_startDateEnable && m_endDateEnable)
		{
			if (!checkStartAndEndDateTime())
			{
				CString fieldname = "End Date/Time";
				TransActiveMessage userMsg;
				userMsg << fieldname;
				UINT response = userMsg.showMsgBox(IDS_UE_060094);
				return;
			}
		}

		//Mintao ++
		//Bug 746
		if (m_uKilometreLoss.IsEmpty())
		{
			CString fieldname = "Kilometer Loss(Empty)";
			TransActiveMessage userMsg;
			userMsg << fieldname;
			UINT response = userMsg.showMsgBox(IDS_UE_060094);
			GetDlgItem(IDC_EDIT_KILOMETRE_LOSS)->SetFocus();
			return;
		}
    
		if (m_uInitialDelayHours.IsEmpty())
		{
			CString fieldname = "Initial Delay Hour(Empty)";
			TransActiveMessage userMsg;
			userMsg << fieldname;
			UINT response = userMsg.showMsgBox(IDS_UE_060094);
			GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_HOURS)->SetFocus();
			return;
		}
		if (m_uInitialDelayMinutes.IsEmpty())
		{
			CString fieldname = "Initial Delay Minutes(Empty)";
			TransActiveMessage userMsg;
			userMsg << fieldname;
			UINT response = userMsg.showMsgBox(IDS_UE_060094);
			GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_MINUTES)->SetFocus();
			return;
		}
		if (m_uInitialDelaySeconds.IsEmpty())
		{
			CString fieldname = "Initial Delay Seconds(Empty)";
			TransActiveMessage userMsg;
			userMsg << fieldname;
			UINT response = userMsg.showMsgBox(IDS_UE_060094);
			GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_SECONDS)->SetFocus();
			return;
		}

		if (m_uAccumulatedDelayHours.IsEmpty())
		{
			CString fieldname = "Accumulated Delay Hour(Empty)";
			TransActiveMessage userMsg;
			userMsg << fieldname;
			UINT response = userMsg.showMsgBox(IDS_UE_060094);
			GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_HOURS)->SetFocus();
			return;
		}
		if (m_uAccumulatedDelayMinutes.IsEmpty())
		{
			CString fieldname = "Accumulated Delay Minutes(Empty)";
			TransActiveMessage userMsg;
			userMsg << fieldname;
			UINT response = userMsg.showMsgBox(IDS_UE_060094);
			GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_MINUTES)->SetFocus();
			return;
		}
		if (m_uAccumulatedDelaySeconds.IsEmpty())
		{
			CString fieldname = "Accumulated Delay Seconds(Empty)";
			TransActiveMessage userMsg;
			userMsg << fieldname;
			UINT response = userMsg.showMsgBox(IDS_UE_060094);
			GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_SECONDS)->SetFocus();
			return;
		}

		int m_delayhour = atoi(m_uInitialDelayHours.GetBuffer(m_uInitialDelayHours.GetLength()));
		int m_delaymins = atoi(m_uInitialDelayMinutes.GetBuffer(m_uInitialDelayMinutes.GetLength()));
		int m_delaysecs = atoi(m_uInitialDelaySeconds.GetBuffer(m_uInitialDelaySeconds.GetLength()));

		//if (( m_delayhour > 23 ) || ( m_delaymins > 59 ) || ( m_delaysecs > 59 ))
		if (( m_delayhour > 999 ) || ( m_delaymins > 59 ) || ( m_delaysecs > 59 ))//TD-15805
		{

			CString fieldname = "Initial delay time is incorrect.\n Hours must be from 0 to 999\n Minutes must be from 0 to 59\nSeconds must be from 0 to 59\n";//TD-15805
			TransActiveMessage userMsg;
			userMsg << fieldname;
			UINT response = userMsg.showMsgBox(IDS_UE_060094);

			//Bugzilla Bug 166: Mintao
			//if (m_uInitialDelayHours > 23)
			if ( atoi(m_uInitialDelayHours.GetBuffer(0)) > 999)//TD-15805
			{
				GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_HOURS)->PostMessage(EM_SETSEL, 0, -1);
				GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_HOURS)->SetFocus();
			}
			else if ( atoi(m_uInitialDelayMinutes.GetBuffer(0))  > 59)
			{
				GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_MINUTES)->PostMessage(EM_SETSEL, 0, -1);
				GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_MINUTES)->SetFocus();
			}
			else if (atoi(m_uInitialDelaySeconds.GetBuffer(0))  > 59)
			{
				GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_SECONDS)->PostMessage(EM_SETSEL, 0, -1);
				GetDlgItem(IDC_EDIT_INITIAL_DELAY_TIME_SECONDS)->SetFocus();
			}
			return;
		}

		int m_accumhour = atoi(m_uAccumulatedDelayHours.GetBuffer(m_uAccumulatedDelayHours.GetLength()));
		int m_accummins = atoi(m_uAccumulatedDelayMinutes.GetBuffer(m_uAccumulatedDelayMinutes.GetLength()));
		int m_accumsecs = atoi(m_uAccumulatedDelaySeconds.GetBuffer(m_uAccumulatedDelaySeconds.GetLength()));

		//if (( m_accumhour > 23 ) || ( m_accummins > 59 ) || ( m_accumsecs > 59 ))
		if (( m_accumhour > 999 ) || ( m_accummins > 59 ) || ( m_accumsecs > 59 ))//TD-15805
		{
			CString fieldname = "Accumulated delay time is incorrect.\nHours must be from 0 to 999\nMinutes must be from 0 to 59\nSeconds must be from 0 to 59\n";//TD-15805
			TransActiveMessage userMsg;
			userMsg << fieldname;
			UINT response = userMsg.showMsgBox(IDS_UE_060094);

			//Bugzilla Bug 166: Mintao
			//if (m_uAccumulatedDelayHours > 23)
			if ( atoi(m_uAccumulatedDelayHours.GetBuffer(0)) > 999)//TD-15805
			{
				GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_HOURS)->PostMessage(EM_SETSEL, 0, -1);
				GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_HOURS)->SetFocus();
			}
		//	else if (m_uAccumulatedDelayHours > 59)
			else if ( atoi(m_uAccumulatedDelayMinutes.GetBuffer(0)) > 59) //TD-15805
			{
				GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_MINUTES)->PostMessage(EM_SETSEL, 0, -1);
				GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_MINUTES)->SetFocus();
			}
			else if ( atoi( m_uAccumulatedDelaySeconds.GetBuffer()) > 59)
			{
				GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_SECONDS)->PostMessage(EM_SETSEL, 0, -1);
				GetDlgItem(IDC_EDIT_ACCUMULATED_DELAY_TIME_SECONDS)->SetFocus();
			}
			return;
		}
		//Bugzilla Bug 180: Mintao
		TA_ASSERT(m_report != NULL, "onSave has NULL pointer ItaIncidentReportData");

		m_report->setTitle( (LPCTSTR) m_sTitle );

		m_report->setFalseIncident( m_bFalseIncident == TRUE );
		m_report->setAsset( &m_Asset );
		m_report->setTrain( (LPCTSTR) m_sTrain );
		m_report->setInnerTrack( m_bInnerTrack == TRUE );
		m_report->setOuterTrack( m_bOuterTrack == TRUE );
		m_report->setKilometreLoss( atol(m_uKilometreLoss.GetBuffer(m_uKilometreLoss.GetLength())));
		
		//Support Display name
		m_report->setLocation1( (LPCTSTR) getLocationName(m_sLocation1) );
		m_report->setLocation2( (LPCTSTR) getLocationName(m_sLocation2) );


		// start date/time
		time_t dateTime = 0;
		getStartDateTimeFromDialog( dateTime );
		m_report->setStartDateTime( dateTime );

		// end Date/Time
		getEndDateTimeFromDialog( dateTime );
		m_report->setEndDateTime( dateTime );

		m_report->setInitialDelay( 60 * ( 60 * m_delayhour + m_delaymins ) + m_delaysecs );
		m_report->setAccumulatedDelay( 60 * ( 60 * m_accumhour + m_accummins ) + m_accumsecs );
		m_report->setAdditionalAlarmDescription( (LPCTSTR) m_sAdditionalAlarmDescription ); 

		{
			// retrieve plans from the list box
			vector<PlanType> arrayPlanTypes;
			getPlanTypesFromListBox( m_listPlan, arrayPlanTypes );
			m_report->setPlansExecuted( arrayPlanTypes );
		}

		m_report->setOperators( (LPCTSTR) m_sOperator );
		m_report->setComments( (LPCTSTR) m_sComment );
		m_report->setRecommendation( (LPCTSTR) m_sRecommendedAction );

		try
		{
			m_report->save();

			//DTL-1023 - start
			//Dialog needs to close right after saving as per SWDS requirements
			//CDialog::OnOK();
			//DTL-1023 - end
		}
		catch (TA_Base_Core::IncidentException)
		{
			CString action = "save";
			TransActiveMessage userMsg;
			userMsg << action;
			UINT response = userMsg.showMsgBox(IDS_UE_060093);
			return;
		}
		catch (TransactiveException)
		{
			CString action = "save";
			TransActiveMessage userMsg;
			userMsg << action;
			UINT response = userMsg.showMsgBox(IDS_UE_060092);
			return;
		}

		cleanupDialog();

		/*if ( m_incidentRights.isControlPermitted( &m_report->getStationReport() ) )
			GetDlgItem(IDC_BUTTON_EDIT_STATION_REPORT)->EnableWindow(TRUE);

		if ( m_incidentRights.isViewPermitted( *m_report ) )
			GetDlgItem(IDC_BUTTON_VIEW_STATION_REPORT)->EnableWindow(TRUE);
    
		refreshDisplay();*/
		//Mintao ++
		//Bug 752(TD12921) 
		//removed unnecessary code  
		//Mintao ++
		//Bug 752(TD12921) 
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	//
	// Check to see if there's anything that's unsaved.  If there is, then prompt the
	// operator.  Otherwise just close the dialog.
	//
	void IncidentReportDialog::onCancel()
	{
		FUNCTION_ENTRY("onCancel");
		if(m_bEditable == true)
		{
			if (isModified() == true)
			{
				//display warning message
				CString action = "quit without saving";
				TransActiveMessage userMsg;
				userMsg << action;
				UINT response = userMsg.showMsgBox(IDS_UW_010003);
				if (response == IDNO)
				{
					//save the change then close the dialog
					//onSave();
					return;		//TD15868, cancel operation when click "no"
				}
			}

		}

		cleanupDialog();

		FUNCTION_EXIT;
	}

	void IncidentReportDialog::cleanupDialog()
	{
		FUNCTION_ENTRY("cleanupDialog");
		// clean up the plan types in the list box
		cleanUpPlanTypeListBox(m_listPlan);
		//Mintao++
		//Bug 770(TD12864)
		try
		{
			CWnd* app = FindWindow(NULL, _T("IncidentManager"));
			CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(app);
			//Bugzilla Bug 180: Mintao
			TA_ASSERT(pMainFrame != NULL, "onCancel has NULL pointer CMainFrame");
			CIncidentManagerListView *pListView = (CIncidentManagerListView *) pMainFrame->GetActiveView();
			//Bugzilla Bug 180: Mintao
			TA_ASSERT(pListView != NULL, "onCancel has NULL pointer CIncidentManagerListView");
			pListView->removeIncidentReportDialog( this );
		}
		catch (...) 
		{
			TA_THROW(IncidentException("Error while try to close the incident report!", IncidentException::GENERAL_ERROR));
		}
		//Mintao++
		//Bug 770(TD12864)


		CDialog::OnCancel();
		DestroyWindow();
		delete this;
		FUNCTION_EXIT;
	}

	//-----------------------------------------------------------------------------
	void IncidentReportDialog::OnListPlanSelChange() 
	{
		//Bugzilla Bug 170: Mintao
		if (m_bEditable)
		{
			m_buttonRemovePlan.EnableWindow( m_listPlan.GetCurSel() != LB_ERR );
		}
	}
	//-----------------------------------------------------------------------------
	long IncidentReportDialog::OnPrepareForClose( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY("OnPrepareForClose");
		//Bugzilla Bug 163: Mintao
		DestroyWindow();
		FUNCTION_EXIT;
		return 0;
	}
		//Mintao ++
	//-----------------------------------------------------------------------------
	//TD13540 - modified parameter to follow programming guidelines
	void IncidentReportDialog::onIncidentRemoved(const std::string& incidentId )
	{
		FUNCTION_ENTRY("onIncidentRemoved");
    
		if ( incidentId == m_report->getReportId() )
		{
			CWnd* app = FindWindow(NULL, _T("IncidentManager"));
			CMainFrame *pMainFrame = dynamic_cast<CMainFrame*>(app);
			//Bugzilla Bug 180: Mintao
			TA_ASSERT(pMainFrame != NULL, "onIncidentRemoved has NULL pointer CMainFrame");
			CIncidentManagerListView *pListView = (CIncidentManagerListView *) pMainFrame->GetActiveView();
			//Bugzilla Bug 180: Mintao
			TA_ASSERT(pListView != NULL, "onIncidentRemoved has NULL pointer CIncidentManagerListView");
			pListView->removeIncidentReportDialog( this );

			CDialog::OnCancel();
			DestroyWindow();
   			delete this;
		}
    
		FUNCTION_EXIT;
	}
		//Mintao ++
	//-----------------------------------------------------------------------------
	void IncidentReportDialog::onIncidentUpdate( ItaIncidentReportData *incident )
	{

		FUNCTION_ENTRY("onIncidentUpdate");
		// if incident is null, return without any update
		if (incident == NULL)
		{
			return;
		}
		if ( incident->getReportId() == m_report->getReportId() )    // ID matchs
		{        
			//Mintao ++
			//Bug 752(TD12921) 
			//removed unnecessary code  
			//Mintao ++
			//Bug 752(TD12921) 
			//Bugzilla Bug 173: Mintao
			ItaIncidentReportData* theclone = incident->clone();
			m_report = theclone;
			if(this->IsWindowVisible())
			{
				refreshDisplay();
			}
			else 
			{
				if((m_StationReportDialog != NULL))
				{
					//Bugzilla Bug 180: Mintao
					TA_ASSERT(m_StationReportDialog != NULL, "onIncidentUpdate has NULL pointer StationReportDialog");
					// post message to refresh display of station report dialog
					m_StationReportDialog->PostMessage( StationReportDialog::WM_APP_REFRESHDISPLAY );
				}
			}
		}
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	bool IncidentReportDialog::isModified()
	{
		FUNCTION_ENTRY("isModified");

		UpdateData();   // transfer values from controls to member variables

		// Then check one by one against original values
		// 
		// Note: We don't need to check following controls because they are 
		//      read-only all the time:
		//
		//      creator, creator profile, creation date/time, 
		//      modifier, modifier profile, modified date/time
		//      associated alarm

		//Bugzilla Bug 180: Mintao
		TA_ASSERT(m_report != NULL, "isModified has NULL pointer ItaIncidentReportData");

		// title
		if ( m_sTitle != m_report->getTitle().c_str() )
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		// false incident
		if ( m_report->isFalseIncident() != (m_bFalseIncident == TRUE) )
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		// asset
		if ( m_Asset.id != m_report->getAsset().id )
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		// train
		if ( m_sTrain != m_report->getTrain().c_str() )
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		// inner track
		if ( m_report->getInnerTrack() != ( m_bInnerTrack == TRUE ) )
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		// outer track
		if ( m_report->getOuterTrack() != ( m_bOuterTrack == TRUE ) ) 
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		// kilometer loss
		if ( m_report->getKilometreLoss() != atol(m_uKilometreLoss.GetBuffer(m_uKilometreLoss.GetLength())) ) 
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		// location 1
		// TD14370 - location can be empty	
		// if (m_sLocation1 == _T("<Unallocated>"))
		// {
		//     m_sLocation1 = "";
		// }
		// std::string storedloc1 = m_report->getLocation1().c_str();
		// if (storedloc1 == _T("<Unallocated>"))
		// {
		//    storedloc1 = "";
		// }
		// if ( m_sLocation1 != storedloc1.c_str())
		T_LocDisplayMap::iterator itFind1 = m_mapLocations.find(m_report->getLocation1());
		std::string strTemp1 = itFind1!=m_mapLocations.end()?itFind1->second:"";
		if (m_sLocation1 != strTemp1.c_str())
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}
    
		// location 2
		// TD14370 - location can be empty
		//if (m_sLocation2 == _T("<Unallocated>"))
		//{
		//    m_sLocation2 = "";
		//}
		//std::string storedloc2 = m_report->getLocation2().c_str();
		//if (storedloc2 == _T("<Unallocated>"))
		//{
		//    storedloc2 = "";
		//}
		// if ( m_sLocation2 != storedloc2.c_str() )
		T_LocDisplayMap::iterator itFind2 = m_mapLocations.find(m_report->getLocation2());
		std::string strTemp2 = itFind2!=m_mapLocations.end()?itFind2->second:"";
		if ( m_sLocation2 != strTemp2.c_str() )
 		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		// start date/time
		time_t dateTime = 0;
		getStartDateTimeFromDialog(dateTime);
		if ( dateTime != m_report->getStartDateTime() )
			return true;

		// end date/time
		getEndDateTimeFromDialog(dateTime);
		if ( dateTime != m_report->getEndDateTime() )
 		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		int m_delayhour = atoi(m_uInitialDelayHours.GetBuffer(m_uInitialDelayHours.GetLength()));
		int m_delaymins = atoi(m_uInitialDelayMinutes.GetBuffer(m_uInitialDelayMinutes.GetLength()));
		int m_delaysecs = atoi(m_uInitialDelaySeconds.GetBuffer(m_uInitialDelaySeconds.GetLength()));

		int m_accumhour = atoi(m_uAccumulatedDelayHours.GetBuffer(m_uAccumulatedDelayHours.GetLength()));
		int m_accummins = atoi(m_uAccumulatedDelayMinutes.GetBuffer(m_uAccumulatedDelayMinutes.GetLength()));
		int m_accumsecs = atoi(m_uAccumulatedDelaySeconds.GetBuffer(m_uAccumulatedDelaySeconds.GetLength()));
		// initial delay time
		if ( 60 * ( 60 * m_delayhour + m_delaymins ) + m_delaysecs != m_report->getInitialDelay() )
  		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}
    
		// accumulated delay time
		if ( 60 * ( 60 * m_accumhour + m_accummins ) + m_accumsecs != m_report->getAccumulatedDelay() )
  		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		if ( m_sAdditionalAlarmDescription != m_report->getAdditionalAlarmDescription().c_str() )
 		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		{
			// plans executed

			// from the list box
			vector<PlanType> arrayPlanTypes;
			getPlanTypesFromListBox( m_listPlan, arrayPlanTypes );

			// and original
			vector<PlanType> arrayOriginalPlanTypes = m_report->getPlansExecuted();

			// compare them
			if ( arrayPlanTypes != arrayOriginalPlanTypes )
			{
				FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
				return true;	// different!
			}
		}

		if ( m_sOperator != m_report->getOperators().c_str() )
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		if ( m_sComment != m_report->getComments().c_str() )
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		if ( m_sRecommendedAction != m_report->getRecommendation().c_str() )
 		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return true;
		}

		FUNCTION_EXIT;

		return false;
	}
	//-----------------------------------------------------------------------------
	ItaIncidentReportData* IncidentReportDialog::getIncident()
	{
		FUNCTION_ENTRY("getIncident");
		FUNCTION_EXIT;
		//Bugzilla Bug 180: Mintao
		TA_ASSERT(m_report != NULL, "getIncident has NULL pointer ItaIncidentReportData");

		return m_report;	// temporary
	}

	//Mintao ++
	//Bug 651
	BOOL IncidentReportDialog::PreTranslateMessage(MSG* pMsg) 
	{
		FUNCTION_ENTRY("PreTranslateMessage");
		// removed for TD14577 - lizettejl
		// Save is always enabled accdg to the GDN
		//if (m_bEditable)
		//{
		//    m_buttonSave.EnableWindow(isModified());
		//}

		FUNCTION_EXIT;
		return CDialog::PreTranslateMessage(pMsg);
	}
	//Mintao ++
	//Bug 651

	//Mintao ++
	//Bug 746
	bool IncidentReportDialog::checkStartAndEndDateTime()
	{
		FUNCTION_ENTRY("checkStartAndEndDateTime");
		UpdateData(TRUE);
		m_ctrlStartDate.GetTime(m_timeStartDate);
		m_ctrlEndDate.GetTime(m_timeEndDate);
		if(
			(m_timeEndDate.GetYear() < m_timeStartDate.GetYear()) || 
			((m_timeEndDate.GetYear() == m_timeStartDate.GetYear()) && (m_timeEndDate.GetMonth() < m_timeStartDate.GetMonth())) ||
			((m_timeEndDate.GetYear() == m_timeStartDate.GetYear()) && (m_timeEndDate.GetMonth() == m_timeStartDate.GetMonth()) && (m_timeEndDate.GetDay() < m_timeStartDate.GetDay()))
			)
		{
			FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
			return false;
		}

		if ((m_timeEndDate.GetYear() == m_timeStartDate.GetYear()) && (m_timeEndDate.GetMonth() == m_timeStartDate.GetMonth()) && (m_timeEndDate.GetDay() == m_timeStartDate.GetDay()))
		{
			m_ctrlStartTime.GetTime(m_timeStartTime);
			m_ctrlEndTime.GetTime(m_timeEndTime);
			if (
				(m_timeEndTime.GetHour() < m_timeStartTime.GetHour()) ||
				((m_timeEndTime.GetHour() == m_timeStartTime.GetHour()) && (m_timeEndTime.GetMinute() < m_timeStartTime.GetMinute())) ||
				((m_timeEndTime.GetHour() == m_timeStartTime.GetHour()) && (m_timeEndTime.GetMinute() == m_timeStartTime.GetMinute()) && (m_timeEndTime.GetSecond() < m_timeStartTime.GetSecond()))
				)
			{
				FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
				return false;
			}
		}

		FUNCTION_EXIT;
		return true;
	}
	//Mintao ++
	//Bug 746


	//-----------------------------------------------------------------------------
	// lizettejl
	// TD13867 - if a station report dialog is still not released, 
	// make sure to free it properly
	BOOL IncidentReportDialog::DestroyWindow() 
	{
		// TODO: Add your specialized code here and/or call the base class
		if (m_StationReportDialog != NULL) 
		{
			m_StationReportDialog->DestroyWindow();
			delete m_StationReportDialog;
			m_StationReportDialog = NULL;
		}
		
		return CDialog::DestroyWindow();
	}

	//-----------------------------------------------------------------------------
	// lizettejl
	// TD13867 - added this handler function for the special window message sent to the 
	// Incident report dialog when a station report dialog is closed 
	long IncidentReportDialog::OnStationReportDlgEnd( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY("OnStationReportDlgEnd");
		
		if(wParam==IDOK)
		{
			try
			{
				m_report->updateStationReportId(m_report->getStationReport().getReportId());
			}
			catch (TransactiveException& e)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Transactive Exception: %s caught in IncidentReportDialog::onEditStationReport()!", e.what());    
			}	
		}
		
		this->EnableWindow(true); 
		
		FUNCTION_EXIT;
		return 0;
	}

	//-----------------------------------------------------------------------------
	// lizettejl
	// TD15044 - added this method to handle the setting of the windows controls
	// to read only
	void IncidentReportDialog::setControlsToReadOnly()
	{
		UpdateData(TRUE);
		
		setReadOnlySpecial(IDC_COMBO_LOCATION1, m_staticLocation1, m_sLocation1);
		setReadOnlySpecial(IDC_COMBO_LOCATION2, m_staticLocation2, m_sLocation2);
		
		//COleDateTime selectedTime;
		CString strTime;
		if ((m_report != NULL) && (m_report->getStartDateTime() != 0))
		{		
			m_ctrlStartDate.GetWindowText(strTime);
			setReadOnlySpecial(IDC_DATEPICKER_START_DATE, m_staticStartDate,strTime);
			m_ctrlStartTime.GetWindowText(strTime);
			setReadOnlySpecial(IDC_TIMEPICKER_START_TIME, m_staticStartTime,strTime);		
		}
		else
		{
			setReadOnlySpecial(IDC_DATEPICKER_START_DATE, m_staticStartDate, "");
			setReadOnlySpecial(IDC_TIMEPICKER_START_TIME, m_staticStartTime,"");
		}
		
		if ((m_report != NULL) && (m_report->getEndDateTime() != 0))
		{	 
			m_ctrlEndDate.GetWindowText(strTime);
			setReadOnlySpecial(IDC_DATEPICKER_END_DATE, m_staticEndDate,strTime);
			m_ctrlEndTime.GetWindowText(strTime);
			setReadOnlySpecial(IDC_TIMEPICKER_END_TIME, m_staticEndTime,strTime);			 
		}
		else
		{
			setReadOnlySpecial(IDC_DATEPICKER_END_DATE, m_staticEndDate, "");
			setReadOnlySpecial(IDC_TIMEPICKER_END_TIME, m_staticEndTime, "");
		}
	}

	//-----------------------------------------------------------------------------
	// lizettejl
	// TD15044 - added this special method to handle the setting of the windows controls
	// to read only, yet with display text that are not grayed. this is achieved by using
	// the static control associated for the window control to be disable.
	void IncidentReportDialog::setReadOnlySpecial( const int dlgID, CStatic &staticCtrl, 
												  const CString &strDisplay )
	{
		CWnd* pWnd = GetDlgItem(dlgID);
		if(pWnd != NULL)
		{	
			// disable control
			pWnd->EnableWindow(FALSE);
			// 
			CRect rect;
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.top += 4;
			rect.bottom -= 2;
			rect.left +=4;
			rect.right -= 18;	
			staticCtrl.MoveWindow(rect);
			staticCtrl.SetWindowText(strDisplay);	
			staticCtrl.ShowWindow(SW_SHOW);		
			
		}	
	}
	//Begin-- TD18725
	void IncidentReportDialog::SetTimeRange(bool bIsEndTime,bool bIsDateChange)
	{
		CTime tmStartDate,tmStartTime,tmStartDateTime;
		///needs to get time value only if start date and time is enabled.
		if(m_startDateEnable)
		{
		m_ctrlStartDate.GetTime(tmStartDate);    //get the start date
		m_ctrlStartTime.GetTime(tmStartTime);	 //get the start time
		if (tmStartDate < 0 || tmStartTime < 0)
		{
			return;
		}
		tmStartDateTime = CTime(tmStartDate.GetYear(),tmStartDate.GetMonth(),tmStartDate.GetDay(),
								tmStartTime.GetHour(),tmStartTime.GetMinute(),tmStartTime.GetSecond());
		}
		CTime tmEndDate,tmEndTime,tmEndDateTime;
		if(m_endDateEnable)
		{
		m_ctrlEndDate.GetTime(tmEndDate);    //get the end date
		m_ctrlEndTime.GetTime(tmEndTime);	 //get the end time
		if (tmEndDate < 0 || tmEndTime < 0)
		{
			return;
		}
		tmEndDateTime = CTime(tmEndDate.GetYear(),tmEndDate.GetMonth(),tmEndDate.GetDay(),
								tmEndTime.GetHour(),tmEndTime.GetMinute(),tmEndTime.GetSecond());
		}

		if (m_bIsReportNew)   //new report
		{
			CTime tmUpperLimit;
			tmUpperLimit = CTime::GetCurrentTime();
			if (bIsEndTime)    //End time changed
			{
				if (tmEndDateTime > tmUpperLimit)
				{
					m_ctrlEndDate.SetTime(&tmUpperLimit);
					m_ctrlEndTime.SetTime(&tmUpperLimit);
				}
				else if (m_startDateEnable && (tmEndDateTime < tmStartDateTime))
				{
					m_ctrlEndDate.SetTime(&tmStartDateTime);
					m_ctrlEndTime.SetTime(&tmStartDateTime);
				}
				else
				{
					if (bIsDateChange) //date changed
					{
						m_ctrlEndTime.SetTime(&tmEndDateTime);
					}
				}		
				m_ctrlEndDate.SetRange(&tmStartDateTime,&tmUpperLimit);			
				m_ctrlEndTime.SetRange(&tmStartDateTime,&tmUpperLimit);
				m_ctrlStartDate.SetRange(&Time_Lower_Limit,&tmEndDateTime);
				m_ctrlStartTime.SetRange(&Time_Lower_Limit,&tmEndDateTime);
			}
			else   //start time changed
			{				
				if (tmStartDateTime > tmUpperLimit)
				{
					m_ctrlStartDate.SetTime(&tmUpperLimit);
					m_ctrlStartTime.SetTime(&tmUpperLimit);
				}
				else if (m_endDateEnable && (tmStartDateTime > tmEndDateTime))
				{
					m_ctrlStartDate.SetTime(&tmEndDateTime);
					m_ctrlStartTime.SetTime(&tmEndDateTime);
				}
				else
				{
					if(bIsDateChange)  //datechanged
					{
						m_ctrlStartTime.SetTime(&tmStartDateTime);
					}
				}
				m_ctrlStartDate.SetRange(&Time_Lower_Limit,&tmEndDateTime);
				m_ctrlStartTime.SetRange(&Time_Lower_Limit,&tmEndDateTime);
			}
		}
		else  //old report
		{
			CTime reportTime(m_report->getCreatedDateTime());
			if (bIsEndTime)   //End time changed
			{
				if (tmEndDateTime > reportTime)
				{
					m_ctrlEndDate.SetTime(&reportTime);
					m_ctrlEndTime.SetTime(&reportTime);
				}
				else if (m_startDateEnable && (tmEndDateTime < tmStartDateTime))
				{
					m_ctrlEndDate.SetTime(&tmStartDateTime);
					m_ctrlEndTime.SetTime(&tmStartDateTime);
				}
				else
				{
					if (bIsDateChange)   //date changed
					{
						m_ctrlEndTime.SetTime(&tmEndDateTime);
					}
				}
				
				m_ctrlEndDate.SetRange(&tmStartDateTime,&reportTime);
				m_ctrlEndTime.SetRange(&tmStartDateTime,&reportTime);
				m_ctrlStartDate.SetRange(&Time_Lower_Limit,&tmEndDateTime);
				m_ctrlStartTime.SetRange(&Time_Lower_Limit,&tmEndDateTime);
			}
			else  //start time changed
			{
				if (tmStartDateTime > reportTime)
				{
					m_ctrlStartDate.SetTime(&reportTime);
					m_ctrlStartTime.SetTime(&reportTime);					
					m_ctrlStartDate.SetRange(&Time_Lower_Limit,&reportTime);
					m_ctrlStartTime.SetRange(&Time_Lower_Limit,&reportTime);
				}
				else if (m_endDateEnable && (tmStartDateTime > tmEndDateTime))
				{
					m_ctrlStartDate.SetTime(&tmEndDateTime);
					m_ctrlStartTime.SetTime(&tmEndDateTime);
					m_ctrlStartDate.SetRange(&Time_Lower_Limit,&tmEndDateTime);
					m_ctrlStartTime.SetRange(&Time_Lower_Limit,&tmEndDateTime);
				}
				else
				{
					if (bIsDateChange)  //date changed
					{
						m_ctrlStartTime.SetTime(&tmStartDateTime);
					}
				}
			}
		}
	}
	//End-- TD18725

// CL18904++
// if the alarm in Alarm Manager does not exist in DB
// we can not create incident report for this alarm
// AlarmDetail is "" and AlarmStartTime is 0
// and AlarmId is not ""
// return true if alarm is exist
	BOOL IncidentReportDialog::isAlarmExists()
	{
		if (m_alarmId.length() > 0 && m_report->getAssociatedAlarmDetail().length() == 0
			&& m_report->getStartDateTime() == 0)
		{
			return false;
		}
		return true;
	}
// ++CL18904

	CString IncidentReportDialog::getLocationName(CString strLocDisplayName)
	{
		T_LocDisplayMap::iterator itr = m_mapLocations.begin();
		for (itr; itr != m_mapLocations.end(); itr++)
		{
			if (strcmpi(itr->second.c_str(), strLocDisplayName.GetBuffer(0)) == 0)
			{
				return CString(itr->first.c_str());
			}
		}

		return CString("");
	}
}// end TA_IRS_App
