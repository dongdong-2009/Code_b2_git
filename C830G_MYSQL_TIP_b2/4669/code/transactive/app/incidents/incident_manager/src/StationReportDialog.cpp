/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/StationReportDialog.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This dialog is responsible for displaying the details of the station report.
  * This is a modal dialog.
  */
#include "app/incidents/incident_manager/src/stdafx.h"
#include "app/incidents/incident_manager/src/StationReportDialog.h"
#include "app/incidents/incident_manager/src/IncidentReportDialog.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
// TD13540 - this header file is not used
// #include "bus/mfc_extensions/src/bitmap_and_text_push_button/FooButton.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/exceptions/src/TransactiveException.h"

using namespace TA_Base_Bus;
using namespace TA_Base_Core;

namespace TA_IRS_App
{
	
	const UINT StationReportDialog::WM_APP_REFRESHDISPLAY = WM_APP + 1;
	const int StationReportDialog::IS_EDIT_BOX = 1;
	const int StationReportDialog::IS_COMBO_BOX = 2;
	const int StationReportDialog::IS_CHECK_OR_RADIO_BUTTON = 3;
	const int StationReportDialog::IS_DATE_TIME_CONTROL = 4;
	
    //TD13540 - add constants for the date and time format
    //const std::string StationReportDialog::DATE_FORMAT = "dd'-'MMM'-'yyy"; // TD12474
	
	BEGIN_MESSAGE_MAP(StationReportDialog, CDialog)
		//{{AFX_MSG_MAP(StationReportDialog)
		ON_BN_CLICKED(IDCANCEL, onCancel)
		ON_BN_CLICKED(IDOK, onSave)
		ON_BN_CLICKED(IDC_CHECK_TYPE_OTHERS, OnCheckTypeOthers)
		ON_BN_CLICKED(IDC_CHECK_LOCATION_OTHERS, OnCheckLocationOthers)
		ON_BN_CLICKED(IDC_CHECK_CONDITION_OTHERS, OnCheckConditionOthers)
		ON_BN_CLICKED(IDC_CHECK_CAUSED_BY_OTHERS, OnCheckCausedByOthers)
		ON_MESSAGE( WM_APP_REFRESHDISPLAY, OnRefreshDisplay )
		ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATEPICKER_DATE, OnDatetimechangeDatepickerDate)
		//}}AFX_MSG_MAP
		END_MESSAGE_MAP()
		
		void StationReportDialog::DoDataExchange(CDataExchange* pDX)
	{
		FUNCTION_ENTRY("DoDataExchange");
		
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(StationReportDialog)
		DDX_Control(pDX, IDC_DATE_STATIC, m_staticDate);
		DDX_Control(pDX, IDOK, m_SaveBtn);
		DDX_Control(pDX, IDC_CHECK_POWER, m_CausedByPower);
		DDX_Control(pDX, IDC_EDIT_EP, m_EditEPControl);
		DDX_Control(pDX, IDC_CHECK_TYPE_OTHERS, m_IncTypeOtherCrtl);
		DDX_Control(pDX, IDC_CHECK_HEATED_ARGUMENT, m_IncTypeHeatedArgCrtl);
		DDX_Control(pDX, IDC_CHECK_ASSAULT, m_IncTypeAssaultCrtl);
		DDX_Control(pDX, IDC_CHECK_ARREST_OF_PERSON, m_IncTypeArrestPersonCrtl);
		DDX_Control(pDX, IDC_CHECK_CRIMINAL_ACT, m_IncTypeCriminalActCrtl);
		DDX_Control(pDX, IDC_CHECK_TRESPASSING, m_IncTypeTrespassingCrtl);
		DDX_Control(pDX, IDC_CHECK_EQUIPMENT_MALFUNCTION, m_IncTypeEquipMalfCrtl);
		DDX_Control(pDX, IDC_CHECK_EQUIPMENT_ABNORMALITY, m_IncTypeEquipAbnormalCrtl);
		DDX_Control(pDX, IDC_CHECK_FIRE, m_IncTypeFireCrtl);
		DDX_Control(pDX, IDC_CHECK_DAMAGED_FACILITY, m_IncTypeDemagedFacilityCrtl);
		DDX_Control(pDX, IDC_CHECK_DERAILMENT, m_IncTypeDerailmentCrtl);
		DDX_Control(pDX, IDC_CHECK_TRAIN_SERVICE_DELAY, m_IncTypeTrainDelayCrtl);
		//TD15204 - reverting back the changes of TD15041
		DDX_Control(pDX, IDC_EDIT_ID, m_ReportIdControl); // ++ lizettejl
		DDX_Control(pDX, IDC_CHECK_LOCATION_OTHERS, m_LocationOtherControl);
		DDX_Control(pDX, IDC_CHECK_DEPOT, m_LocationDepotControl);
		DDX_Control(pDX, IDC_CHECK_STATION, m_LocationStationControl);
		DDX_Control(pDX, IDC_CHECK_TRACKSIDE, m_LocationTracksideControl);
		DDX_Control(pDX, IDC_CHECK_TRAIN, m_LocationTrainControl);
		DDX_Control(pDX, IDC_DATEPICKER_DATE, m_DatePickerControl);
		DDX_Control(pDX, IDC_CHECK_CONDITION_OTHERS, m_ConditionOtherControl);
		DDX_Control(pDX, IDC_CHECK_POORLY_LIT, m_ConditionPoorlyLitControl);
		DDX_Control(pDX, IDC_CHECK_WELL_LIT, m_ConditionWellLitControl);
		DDX_Control(pDX, IDC_CHECK_DIRTY, m_ConditionDirtyControl);
		DDX_Control(pDX, IDC_CHECK_CLEAN, m_ConditionCleanControl);
		DDX_Control(pDX, IDC_CHECK_WET, m_ConditionWetControl);
		DDX_Control(pDX, IDC_CHECK_DRY, m_ConditionDryControl);
		DDX_Control(pDX, IDC_CHECK_CAUSED_BY_OTHERS, m_CheckBoxCausedByOthers);
		DDX_Control(pDX, IDC_CHECK_TK_CCT, m_CausedByTkCct);
		DDX_Control(pDX, IDC_CHECK_SIGNAL, m_CausedBySignal);
		DDX_Control(pDX, IDC_CHECK_POINT, m_CausedByPoint);
		DDX_Control(pDX, IDC_CHECK_PAX, m_CausedByPAX);
		DDX_Control(pDX, IDC_CHECK_BLS, m_CausedByBLS);
		DDX_Control(pDX, IDC_CHECK_ESP, m_CausedByESP);
		DDX_Text(pDX, IDC_EDIT_SUBMITTED_BY, m_SubmittedBy);
		DDV_MaxChars(pDX, m_SubmittedBy, 100);
		DDX_Text(pDX, IDC_EDIT_EP, m_EP);
		DDV_MaxChars(pDX, m_EP, 100);
		DDX_Text(pDX, IDC_EDIT_ACTION_TAKEN, m_ActionTaken);
		DDV_MaxChars(pDX, m_ActionTaken, 500);
		DDX_Text(pDX, IDC_EDIT_TYPE_OTHERS, m_TypeOthers);
		DDV_MaxChars(pDX, m_TypeOthers, 100);
		DDX_Text(pDX, IDC_EDIT_LOCATION_OTHERS, m_LocationOthers);
		DDV_MaxChars(pDX, m_LocationOthers, 100);
		DDX_Text(pDX, IDC_EDIT_CONDITION_OTHERS, m_ConditionOther);
		DDV_MaxChars(pDX, m_ConditionOther, 100);
		DDX_Text(pDX, IDC_EDIT_CAUSED_BY_OTHERS, m_CausedByOthers);
		DDV_MaxChars(pDX, m_CausedByOthers, 100);
		DDX_Text(pDX, IDC_EDIT_STAFF_INVOLVED, m_StaffInvolved);
		DDV_MaxChars(pDX, m_StaffInvolved, 500);
		DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_Description);
		DDV_MaxChars(pDX, m_Description, 500);
		DDX_Check(pDX, IDC_CHECK_TYPE_OTHERS, m_bTypeOthers);
		DDX_Check(pDX, IDC_CHECK_LOCATION_OTHERS, m_bLocationOthers);
		DDX_Check(pDX, IDC_CHECK_CONDITION_OTHERS, m_bConditionOthers);
		DDX_Check(pDX, IDC_CHECK_CAUSED_BY_OTHERS, m_bCausedByOthers);
		DDX_DateTimeCtrl(pDX, IDC_DATEPICKER_DATE, m_DateTime);
		//}}AFX_DATA_MAP
		
		FUNCTION_EXIT;
	}
	
	StationReportDialog::~StationReportDialog()
	{
		FUNCTION_ENTRY("~StationReportDialog");
		FUNCTION_EXIT;
	}
	
	StationReportDialog::StationReportDialog(	ItaStationReportData& stationReport, 
		bool editable, 
		ItaIncidentRights& incidentRights, 
		TA_Base_Bus::ItaUserMessage& userMessage,
		CWnd* pParent /*=NULL*/)
		:CDialog(StationReportDialog::IDD, pParent),
		m_bEditable(editable),
		m_report(stationReport),
		m_userMessage (userMessage),
		m_incidentRights (incidentRights)
	{
		FUNCTION_ENTRY("StationReportDialog");
		
		//{{AFX_DATA_INIT(StationReportDialog)
		m_SubmittedBy = _T("");
		m_EP = _T("");
		m_ActionTaken = _T("");
		m_TypeOthers = _T("");
		m_LocationOthers = _T("");
		m_ConditionOther = _T("");
		m_CausedByOthers = _T("");
		m_StaffInvolved = _T("");
		m_Description = _T("");
		m_bTypeOthers = FALSE;
		m_bLocationOthers = FALSE;
		m_bConditionOthers = FALSE;
		m_bCausedByOthers = FALSE;
		m_DateTime = 0;
		//}}AFX_DATA_INIT
		
		m_bDateEnable = TRUE; // added for TD14371
		
		FUNCTION_EXIT;
	}
	
	bool StationReportDialog::isModified()
	{
		FUNCTION_ENTRY("isModified");
		UpdateData(TRUE);
		//check if submitted by is modified
		if (m_report.getSubmittedBy().compare(m_SubmittedBy.GetBuffer(m_SubmittedBy.GetLength())) != 0)
		{
			FUNCTION_EXIT;
			return true;
		}
		
		//check if EP is modified
		if (m_report.getEmployeeParticulars().compare(m_EP.GetBuffer(m_EP.GetLength())) != 0)
		{
			FUNCTION_EXIT;
			return true;
		}
		
		//check if date is modified
		CTime actualDate(m_report.getDate());
		
		if (actualDate != m_DateTime)
		{
			FUNCTION_EXIT;
			return true;
		}
		
		//check if Type is modified
		IncidentType incidentType = m_report.getType();
		
		if (    
            (incidentType.arrestOfPerson != (m_IncTypeArrestPersonCrtl.GetCheck() == 1?true:false)) || 
            (incidentType.assault != (m_IncTypeAssaultCrtl.GetCheck() == 1?true:false)) || 
            (incidentType.criminalAct != (m_IncTypeCriminalActCrtl.GetCheck() == 1?true:false)) || 
            (incidentType.damagedFacilityOrProperty != (m_IncTypeDemagedFacilityCrtl.GetCheck() == 1?true:false)) || 
            (incidentType.equipmentAbnormally != (m_IncTypeEquipAbnormalCrtl.GetCheck() == 1?true:false)) || 
            (incidentType.equipmentMalfunction != (m_IncTypeEquipMalfCrtl.GetCheck() == 1?true:false)) || 
            (incidentType.fire != (m_IncTypeFireCrtl.GetCheck() == 1?true:false)) || 
            (incidentType.heatedArgument != (m_IncTypeHeatedArgCrtl.GetCheck() == 1?true:false)) || 
            (incidentType.trainServiceDelay != (m_IncTypeTrainDelayCrtl.GetCheck() == 1?true:false)) || 
            (incidentType.trespassing != (m_IncTypeTrespassingCrtl.GetCheck() == 1?true:false)) || 
            (incidentType.other.compare(m_TypeOthers.GetBuffer(m_TypeOthers.GetLength()))) != 0            
			) 
		{
			// modified
			FUNCTION_EXIT;  
			return true;
		}
		
		//check if location is modified
		IncidentLocationType locType = m_report.getLocation();
		
		if (    (locType.train != (m_LocationTrainControl.GetCheck() == 1?true:false)) || 
            (locType.trackside != (m_LocationTracksideControl.GetCheck() == 1?true:false)) || 
            (locType.station != (m_LocationStationControl.GetCheck() == 1?true:false)) || 
            (locType.depot != (m_LocationDepotControl.GetCheck() == 1?true:false)) || 
            (locType.other.compare(m_LocationOthers.GetBuffer(m_LocationOthers.GetLength())) != 0)
			) 
		{
			//modified
			FUNCTION_EXIT;
			return true;
		}
		
		//check if caused by is modified
		IncidentCauseType cause = m_report.getCause();
		
		if (    (cause.bLS != (m_CausedByBLS.GetCheck() == 1?true:false)) || 
            (cause.eSP != (m_CausedByESP.GetCheck() == 1?true:false)) || 
            (cause.pax != (m_CausedByPAX.GetCheck() == 1?true:false)) || 
            (cause.point != (m_CausedByPoint.GetCheck() == 1?true:false)) || 
            (cause.power != (m_CausedByPower.GetCheck() == 1?true:false)) || 
            (cause.signal != (m_CausedBySignal.GetCheck() == 1?true:false)) || 
            (cause.tkCct != (m_CausedByTkCct.GetCheck() == 1?true:false)) || 
            (cause.other.compare(m_CausedByOthers.GetBuffer(m_CausedByOthers.GetLength())) != 0)
			) 
		{
			// modified
			FUNCTION_EXIT;
			return true;
		}
		
		//check if action taken is modified
		if (m_report.getAction().compare(m_ActionTaken.GetBuffer(m_ActionTaken.GetLength())) != 0)
		{
			FUNCTION_EXIT;
			return true;
		}
		
		//check if staff involved is modified
		if (m_report.getStaff().compare(m_StaffInvolved.GetBuffer(m_StaffInvolved.GetLength())) != 0)
		{
			FUNCTION_EXIT;
			return true;
		}
		
		//check if description is modified
		if (m_report.getDescription().compare(m_Description.GetBuffer(m_Description.GetLength())) != 0)
		{
			FUNCTION_EXIT;
			return true;
		}
		
		FUNCTION_EXIT;
		return false;
	}



	/////////////////////////////////////////////////////////////////////////////
	// StationReportDialog message handlers

	void StationReportDialog::onCancel() 
	{
		FUNCTION_ENTRY("onCancel");
		
		if(m_bEditable == false)
		{		
			//lizettejl
			//TD13867
			//send to the parent window the dialog's return value
			TA_ASSERT(m_pParentWnd != NULL, "Cannot create Station Report without an Incident Report");
			if(m_pParentWnd != NULL)
				m_pParentWnd->SendMessage(IncidentReportDialog::WM_STATIONREPORTDLG_END,IDCANCEL,IDD);
			
			CDialog::OnCancel();
		}
		else
		{
			if (isModified() == true)
			{
				//display warning message
				CString action = "quit without saving";
				TransActiveMessage userMsg;
				userMsg << action;
				UINT response = userMsg.showMsgBox(IDS_UW_010003);
				if (response == IDYES)
				{
					//ignore all changes and closed the dialog
					CDialog::OnCancel();
				}
				else if (response == IDNO)
				{
					//save the change then close the dialog
					onSave();
				}
			}
			else 
			{
				//lizettejl
				//TD13867
				//send to the parent window the dialog's return value
				TA_ASSERT(m_pParentWnd != NULL, "Cannot create Station Report without an Incident Report");
				if(m_pParentWnd != NULL)
					m_pParentWnd->SendMessage(IncidentReportDialog::WM_STATIONREPORTDLG_END,IDCANCEL,IDD);
				
				CDialog::OnCancel();
			}
		}
		
		FUNCTION_EXIT;
	}

	void StationReportDialog::onSave() 
	{
		FUNCTION_ENTRY("onSave");
		UpdateData(TRUE);
		if (m_incidentRights.isControlPermitted(&m_report) == false)
		{
			CString action = "save the incident report";
			TransActiveMessage userMsg;
			userMsg << action;
			UINT response = userMsg.showMsgBox(IDS_UE_010021);
			FUNCTION_EXIT;
			return;
		}
		else
		{
			try
			{
				//set action
				//LPTSTR p = value.GetBuffer( value.GetLength());
				//std::string convValue(p);
				std::string actionStr = m_ActionTaken.GetBuffer(m_ActionTaken.GetLength());
				m_report.setAction(actionStr);
				
				//set cuase
				IncidentCauseType cause;        
				cause.bLS    = (m_CausedByBLS.GetCheck() == 1?true:false);
				cause.eSP    = (m_CausedByESP.GetCheck() == 1?true:false);
				cause.pax    = (m_CausedByPAX.GetCheck() == 1?true:false);
				cause.point  = (m_CausedByPoint.GetCheck() == 1?true:false);
				cause.power  = (m_CausedByPower.GetCheck() == 1?true:false);
				cause.signal = (m_CausedBySignal.GetCheck() == 1?true:false);
				cause.tkCct = (m_CausedByTkCct.GetCheck() == 1?true:false);
				
				if ((m_CheckBoxCausedByOthers.GetCheck() == 1?true:false) == true)
				{
					cause.other = m_CausedByOthers.GetBuffer(m_CausedByOthers.GetLength());
				}
				m_report.setCause(cause);
				
				//condition
				{
					IncidentConditionType condType;
					
					condType.clean = (m_ConditionCleanControl.GetCheck() == 1?true:false);
					condType.dirty = (m_ConditionDirtyControl.GetCheck() == 1?true:false);
					condType.dry   = (m_ConditionDryControl.GetCheck() == 1?true:false);
					condType.poorlyLit = (m_ConditionPoorlyLitControl.GetCheck() == 1?true:false);
					condType.wellLit = (m_ConditionWellLitControl.GetCheck() == 1?true:false);
					condType.wet = (m_ConditionWetControl.GetCheck() == 1?true:false);
					
					if ((m_ConditionOtherControl.GetCheck() == 1?true:false) == true)
					{
						condType.other = m_ConditionOther.GetBuffer(m_ConditionOther.GetLength());
					}
					m_report.setCondition(condType);
				}
				
				//update date
				{
					CTime time;
					m_DatePickerControl.GetTime(time);
					m_report.setDate(time.GetTime());
				}
				
				//update description
				m_report.setDescription(m_Description.GetBuffer(m_Description.GetLength()));
				
				
				//update employee particulars
				m_report.setEmployeeParticulars(m_EP.GetBuffer(m_EP.GetLength()));
				
				
				//update location
				IncidentLocationType locType;
				
				locType.train = (m_LocationTrainControl.GetCheck() == 1?true:false);
				locType.depot = (m_LocationDepotControl.GetCheck() == 1?true:false);
				locType.station = (m_LocationStationControl.GetCheck() == 1?true:false);
				locType.trackside = (m_LocationTracksideControl.GetCheck() == 1?true:false);
				
				if ((m_LocationOtherControl.GetCheck() == 1?true:false) == true)
				{
					locType.other = m_LocationOthers.GetBuffer(m_LocationOthers.GetLength());
				}
				m_report.setLocation(locType);
				
				m_report.setStaff(m_StaffInvolved.GetBuffer(m_StaffInvolved.GetLength()));
				m_report.setSubmittedBy(m_SubmittedBy.GetBuffer(m_SubmittedBy.GetLength()));
				
				
				//update incident type
				{
					IncidentType incidentType;
					
					incidentType.arrestOfPerson            = (m_IncTypeArrestPersonCrtl.GetCheck() == 1?true:false);
					incidentType.assault                   = (m_IncTypeAssaultCrtl.GetCheck() == 1?true:false);
					incidentType.criminalAct               = (m_IncTypeCriminalActCrtl.GetCheck() == 1?true:false);
					incidentType.damagedFacilityOrProperty = (m_IncTypeDemagedFacilityCrtl.GetCheck() == 1?true:false);
					incidentType.derailment                = (m_IncTypeDerailmentCrtl.GetCheck() == 1?true:false);
					incidentType.equipmentAbnormally       = (m_IncTypeEquipAbnormalCrtl.GetCheck() == 1?true:false);
					incidentType.equipmentMalfunction      = (m_IncTypeEquipMalfCrtl.GetCheck() == 1?true:false);
					incidentType.fire                      = (m_IncTypeFireCrtl.GetCheck() == 1?true:false);
					incidentType.heatedArgument            = (m_IncTypeHeatedArgCrtl.GetCheck() == 1?true:false);
					incidentType.trainServiceDelay         = (m_IncTypeTrainDelayCrtl.GetCheck() == 1?true:false);
					incidentType.trespassing               = (m_IncTypeTrespassingCrtl.GetCheck() == 1?true:false);
					
					if ((m_IncTypeOtherCrtl.GetCheck() == 1?true:false) == true)
					{
						incidentType.other = m_TypeOthers.GetBuffer(m_TypeOthers.GetLength());
					}
					m_report.setType(incidentType);
				}
				
			}
			catch (TA_Base_Core::TransactiveException& e)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Transactive Exception: %s caught in StationReportDialog::OnSave()!", e.what());    
				FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
				return;
			}
			//save the report
			try
			{
				m_report.save();
				
				//lizettejl
				//TD13867
				// send to the parent window the dialog's return value
				TA_ASSERT(m_pParentWnd != NULL, "Cannot create Station Report without an Incident Report");
				if(m_pParentWnd != NULL)
					m_pParentWnd->SendMessage(IncidentReportDialog::WM_STATIONREPORTDLG_END,IDOK,IDD);
				
				CDialog::OnOK();
			}
			catch (TA_Base_Core::IncidentException)
			{
				CString action = "save";
				TransActiveMessage userMsg;
				userMsg << action;
				UINT response = userMsg.showMsgBox(IDS_UE_060093);
				FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
				return;
			}
			catch (TransactiveException)
			{
				CString action = "save";
				TransActiveMessage userMsg;
				userMsg << action;
				UINT response = userMsg.showMsgBox(IDS_UE_060092);
				FUNCTION_EXIT;  //TD13540 - specify the function's exit point in log
				return;
			}
		}
		
		FUNCTION_EXIT;
	}

	void StationReportDialog::OnCheckTypeOthers() 
	{
		FUNCTION_ENTRY("OnCheckTypeOthers");
		if (m_IncTypeOtherCrtl.GetCheck() == 1)
		{
			(GetDlgItem(IDC_EDIT_TYPE_OTHERS))->EnableWindow(TRUE);
		}
		else
		{
			(GetDlgItem(IDC_EDIT_TYPE_OTHERS))->EnableWindow(FALSE);
			(GetDlgItem(IDC_EDIT_TYPE_OTHERS))->SetWindowText("");
			
		}
		FUNCTION_EXIT;
	}

	void StationReportDialog::OnCheckLocationOthers() 
	{
		FUNCTION_ENTRY("OnCheckLocationOthers");
		UpdateData(TRUE);
		if(m_bLocationOthers)
		{
			(GetDlgItem(IDC_EDIT_LOCATION_OTHERS))->EnableWindow(TRUE);
		}
		else
		{
			(GetDlgItem(IDC_EDIT_LOCATION_OTHERS))->EnableWindow(FALSE);
			(GetDlgItem(IDC_EDIT_LOCATION_OTHERS))->SetWindowText("");
		}	
		UpdateData(TRUE);
		FUNCTION_EXIT;
	}

	void StationReportDialog::OnCheckConditionOthers() 
	{
		FUNCTION_ENTRY("OnCheckConditionOthers");
		
		if (m_ConditionOtherControl.GetCheck() == 1) 
		{
			(GetDlgItem(IDC_EDIT_CONDITION_OTHERS))->EnableWindow(TRUE);
		}
		else
		{
			(GetDlgItem(IDC_EDIT_CONDITION_OTHERS))->SetWindowText("");
			(GetDlgItem(IDC_EDIT_CONDITION_OTHERS))->EnableWindow(FALSE);
		}
		
		FUNCTION_EXIT;
	}

	void StationReportDialog::OnCheckCausedByOthers() 
	{
		FUNCTION_ENTRY("OnCheckCausedByOthers");
		//When the Check box is clicked
		UpdateData(TRUE);
		if(m_bCausedByOthers)
		{
			(GetDlgItem(IDC_EDIT_CAUSED_BY_OTHERS))->EnableWindow(TRUE);
		}
		else
		{
			(GetDlgItem(IDC_EDIT_CAUSED_BY_OTHERS))->EnableWindow(FALSE);
			(GetDlgItem(IDC_EDIT_CAUSED_BY_OTHERS))->SetWindowText("");
		}
		FUNCTION_EXIT;	
	}

	BOOL StationReportDialog::OnInitDialog() 
	{
		FUNCTION_ENTRY("OnInitDialog");
		CDialog::OnInitDialog();
		
		OnRefreshDisplay();
		
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	long StationReportDialog::OnRefreshDisplay( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY("OnRefreshDisplay");
		
		//set the vaue for all station report dialog controls
		//update action taken
		{
			m_ActionTaken = _T(m_report.getAction().c_str());
			(GetDlgItem(IDC_EDIT_ACTION_TAKEN))->SetWindowText(m_ActionTaken);
		}
		
		//update caused by
		{
			IncidentCauseType cause = m_report.getCause();    
			m_CausedByBLS.SetCheck(cause.bLS);
			m_CausedByESP.SetCheck(cause.eSP);
			m_CausedByPAX.SetCheck(cause.pax);
			m_CausedByPoint.SetCheck(cause.point);
			m_CausedByPower.SetCheck(cause.power);
			m_CausedBySignal.SetCheck(cause.signal);
			m_CausedByTkCct.SetCheck(cause.tkCct);
			
			if (!cause.other.empty())
			{
				m_CheckBoxCausedByOthers.SetCheck(true);
				m_CausedByOthers = cause.other.c_str();
				(GetDlgItem(IDC_EDIT_CAUSED_BY_OTHERS))->SetWindowText(m_CausedByOthers);
				(GetDlgItem(IDC_EDIT_CAUSED_BY_OTHERS))->EnableWindow(TRUE);
			}
			
		}    
		
		//update condition
		{
			IncidentConditionType condType = m_report.getCondition();
			
			m_ConditionPoorlyLitControl.SetCheck(condType.poorlyLit);
			m_ConditionWellLitControl.SetCheck(condType.wellLit);
			m_ConditionDirtyControl.SetCheck(condType.dirty);
			m_ConditionCleanControl.SetCheck(condType.clean);
			m_ConditionWetControl.SetCheck(condType.wet);
			m_ConditionDryControl.SetCheck(condType.dry);
			
			if (!condType.other.empty())
			{
				m_ConditionOtherControl.SetCheck(true);
				m_ConditionOther = condType.other.c_str();
				(GetDlgItem(IDC_EDIT_CONDITION_OTHERS))->SetWindowText(m_ConditionOther);
				(GetDlgItem(IDC_EDIT_CONDITION_OTHERS))->EnableWindow(TRUE);
			}
		}
		
		//update date
		{
			//TD18725, hongzhi, The selection date should be lesser than todays date / time
			//m_DatePickerControl.SetRange(NULL, &(CTime::GetCurrentTime()));//TD18908
			CTime reportdate(m_report.getDate());
			if ((m_report.isNew()) || (reportdate == 0))
			{
				enableTimeControls(false);
			}
			else
			{
				//TD15044-set date format before setting the actual date so
				// as not the system date format would be followed 
			
     			// TD12474 ++
    			//m_DatePickerControl.SetFormat(DATE_FORMAT.c_str());
    			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat6);
     			m_DatePickerControl.SetFormat(dateFormatStr.c_str());
     			// ++ TD12474

				//++lizettejl
				m_DatePickerControl.SetTime(&reportdate);
			}
        
		}
		//update description
		{
			m_Description = m_report.getDescription().c_str();
			(GetDlgItem(IDC_EDIT_DESCRIPTION))->SetWindowText(m_Description);
		}
		
		//update location
		{
			IncidentLocationType locType = m_report.getLocation();
			m_LocationTrainControl.SetCheck(locType.train);
			m_LocationTracksideControl.SetCheck(locType.trackside);
			m_LocationStationControl.SetCheck(locType.station);
			m_LocationDepotControl.SetCheck(locType.depot);
			
			if (!locType.other.empty() )
			{
				m_LocationOtherControl.SetCheck(true);
				m_LocationOthers = locType.other.c_str();
				(GetDlgItem(IDC_EDIT_LOCATION_OTHERS))->SetWindowText(m_LocationOthers);
				(GetDlgItem(IDC_EDIT_LOCATION_OTHERS))->EnableWindow(TRUE);
			}
			
		}
		
		//TD15204 - reverting back the changes of TD15041
		//update report id  
		(GetDlgItem(IDC_EDIT_ID))->SetWindowText(m_report.getReportId().c_str());
		// ++lizettejl
		
		//update staff involved
		{
			m_StaffInvolved = _T(m_report.getStaff().c_str());
			(GetDlgItem(IDC_EDIT_STAFF_INVOLVED))->SetWindowText(m_StaffInvolved);
		}
		
		//update submitted by
		{
			m_SubmittedBy = _T(m_report.getSubmittedBy().c_str());
			(GetDlgItem(IDC_EDIT_SUBMITTED_BY))->SetWindowText(m_SubmittedBy);
		}
		
		//update type
		{
			IncidentType incidentType = m_report.getType();
        
			m_IncTypeTrainDelayCrtl.SetCheck(incidentType.trainServiceDelay);
			m_IncTypeDerailmentCrtl.SetCheck(incidentType.derailment);
			m_IncTypeDemagedFacilityCrtl.SetCheck(incidentType.damagedFacilityOrProperty);
			m_IncTypeFireCrtl.SetCheck(incidentType.fire);
			m_IncTypeEquipAbnormalCrtl.SetCheck(incidentType.equipmentAbnormally);
			m_IncTypeEquipMalfCrtl.SetCheck(incidentType.equipmentMalfunction);
			m_IncTypeTrespassingCrtl.SetCheck(incidentType.trespassing);
			m_IncTypeCriminalActCrtl.SetCheck(incidentType.criminalAct);
			m_IncTypeArrestPersonCrtl.SetCheck(incidentType.arrestOfPerson);
			m_IncTypeAssaultCrtl.SetCheck(incidentType.assault);
			m_IncTypeHeatedArgCrtl.SetCheck(incidentType.heatedArgument);
			
			if (!incidentType.other.empty() )
			{
				m_IncTypeOtherCrtl.SetCheck(true);
				m_TypeOthers = incidentType.other.c_str();
				(GetDlgItem(IDC_EDIT_TYPE_OTHERS))->SetWindowText(m_TypeOthers);
				(GetDlgItem(IDC_EDIT_TYPE_OTHERS))->EnableWindow(TRUE);
			}
			
		}
		
		//update employee particulars
		m_EP = m_report.getEmployeeParticulars().c_str();
		m_EditEPControl.SetWindowText(m_EP);    
		//enable save button
		(GetDlgItem(IDOK))->EnableWindow(m_bEditable);
		
		// modify how the controls are being enabled/disabled 
		// and add special cases for setting dates to read only
		// no need to call the setReadOnly() method
		// related for the fix for TD15044 - lizettejl
		//if (m_bEditable)
		{
			
			//enable check boxes
			(GetDlgItem(IDC_CHECK_ESP))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_BLS))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_PAX))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_POINT))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_POWER))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_SIGNAL))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_TK_CCT))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_CAUSED_BY_OTHERS))->EnableWindow(m_bEditable);
			
			//enable check boxes
			(GetDlgItem(IDC_CHECK_DRY))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_WET))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_CLEAN))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_DIRTY))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_WELL_LIT))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_POORLY_LIT))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_CONDITION_OTHERS))->EnableWindow(m_bEditable);
			
			//enable checkboxes
			(GetDlgItem(IDC_CHECK_TRAIN))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_TRACKSIDE))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_STATION))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_DEPOT))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_LOCATION_OTHERS))->EnableWindow(m_bEditable);
			
			
			//enable check boxes
			(GetDlgItem(IDC_CHECK_TRAIN_SERVICE_DELAY))->EnableWindow(m_bEditable);
			(GetDlgItem(IDC_CHECK_EQUIPMENT_ABNORMALITY))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_ARREST_OF_PERSON))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_DERAILMENT))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_EQUIPMENT_MALFUNCTION))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_ASSAULT))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_DAMAGED_FACILITY))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_TRESPASSING))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_HEATED_ARGUMENT))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_FIRE))->EnableWindow(m_bEditable);        
			(GetDlgItem(IDC_CHECK_CRIMINAL_ACT))->EnableWindow(m_bEditable);
			(GetDlgItem(IDC_CHECK_TYPE_OTHERS))->EnableWindow(m_bEditable);	        
			
			if(!m_bEditable)
			{
				reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_ACTION_TAKEN))->SetReadOnly(TRUE);
				reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_DESCRIPTION))->SetReadOnly(TRUE);
				reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_STAFF_INVOLVED))->SetReadOnly(TRUE);
				reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_SUBMITTED_BY))->SetReadOnly(TRUE);
				reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_EP))->SetReadOnly(TRUE);
				
				reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_CAUSED_BY_OTHERS))->SetReadOnly(TRUE);
				reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_CONDITION_OTHERS))->SetReadOnly(TRUE);
				reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_LOCATION_OTHERS))->SetReadOnly(TRUE);
				reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_TYPE_OTHERS))->SetReadOnly(TRUE);
				
				setDateCtrlToReadOnly(IDC_DATEPICKER_DATE, m_staticDate);
			}
			else
			{
				(GetDlgItem(IDC_DATEPICKER_DATE))->EnableWindow(m_bEditable);      
				(GetDlgItem(IDC_EDIT_ACTION_TAKEN))->EnableWindow(m_bEditable);
				(GetDlgItem(IDC_EDIT_DESCRIPTION))->EnableWindow(m_bEditable);
				(GetDlgItem(IDC_EDIT_STAFF_INVOLVED))->EnableWindow(m_bEditable);
				(GetDlgItem(IDC_EDIT_SUBMITTED_BY))->EnableWindow(m_bEditable);
				(GetDlgItem(IDC_EDIT_EP))->EnableWindow(m_bEditable);
			}
		}
		
		FUNCTION_EXIT;
		return 0;
	}

	//TD15044 - removed the StationReportDialog::setReadOnly() function since
	// it is not used anymore. ++lizettejl

	void StationReportDialog::enableTimeControls(bool bEnable)
	{
		FUNCTION_ENTRY("enableStartDateTimeControls");
		LONG dwStyle = GetWindowLong( m_DatePickerControl.m_hWnd, GWL_STYLE );
		
		if ( bEnable )
		{
			//TD14371 - check if date and time is correctly updated	 
			
			if (!m_bDateEnable)
			{   m_DatePickerControl.SetTime(&(CTime::GetCurrentTime()));
			m_DateTime = CTime::GetCurrentTime();
			
			}
			// change format so that the control will display the date
			// TD13540 - add constants for the date and time format
			// m_DatePickerControl.SetFormat("dd'-'MMM'-'yyy");

     		// TD12474 ++
    		//m_DatePickerControl.SetFormat(DATE_FORMAT.c_str());		
    		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateFormat6);
     		m_DatePickerControl.SetFormat(dateFormatStr.c_str());
     		// ++ TD12474

			dwStyle |= DTS_SHOWNONE;
		}
		else
		{
			// display a bank instead
			DateTime_SetSystemtime( m_DatePickerControl.m_hWnd, GDT_NONE, NULL );
			m_DatePickerControl.SetFormat(" ");
			m_DateTime = 0;
			dwStyle &= ~DTS_SHOWNONE;
		}
		
		if ( SetWindowLong( m_DatePickerControl.m_hWnd, GWL_STYLE, dwStyle ) == 0 )
			DWORD dwError = GetLastError();
		
		m_bDateEnable = bEnable; // TD14371
		// m_DatePickerControl.Invalidate();
		FUNCTION_EXIT;
	}

	BOOL StationReportDialog::PreTranslateMessage(MSG* pMsg) 
	{
		FUNCTION_ENTRY("PreTranslateMessage");
		//Mintao ++
		//Bug 651
		if (m_bEditable)
		{
			m_SaveBtn.EnableWindow(isModified());
		}
		//Mintao ++
		//Bug 651
		if( pMsg->message == WM_KEYDOWN)
		{
			switch(pMsg->wParam)
			{
			case VK_ESCAPE:
				return 0;
				break;
				
			case VK_RETURN:
				return 0;
				break;
			}
		}
		
		FUNCTION_EXIT;
		return CDialog::PreTranslateMessage(pMsg);
	}


	void StationReportDialog::OnDatetimechangeDatepickerDate(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		
		FUNCTION_ENTRY("OnDatetimechangeDatepickerDate");
		SYSTEMTIME sysTime;
		enableTimeControls( m_DatePickerControl.GetTime(&sysTime) == GDT_VALID );
		
		FUNCTION_EXIT;
		*pResult = 0;
	}
	//-----------------------------------------------------------------------------
	// lizettejl
	// TD15044 - added this special method to handle the setting of the windows controls
	// to read only, yet with display text that are not grayed. this is achieved by using
	// the static control associated for the window control to be disable.
	void StationReportDialog::setDateCtrlToReadOnly(const int dlgID, CStatic &staticCtrl)
	{
		CWnd* pWnd = GetDlgItem(dlgID);
		if(pWnd != NULL)
		{	
			// disable control
			pWnd->EnableWindow(FALSE);	
			CRect rect;
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.top += 4;
			rect.bottom -= 2;
			rect.left +=4;
			rect.right -= 18;	
			staticCtrl.MoveWindow(rect);
			CString strDisplay;
			pWnd->GetWindowText(strDisplay);
			staticCtrl.SetWindowText(strDisplay);	
			staticCtrl.ShowWindow(SW_SHOW);		
			
		}	
	}
}// end TA_IRS_App
