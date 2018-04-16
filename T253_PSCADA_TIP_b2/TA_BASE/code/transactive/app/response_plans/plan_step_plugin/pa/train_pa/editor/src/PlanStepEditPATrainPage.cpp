/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditPATrainPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/10/15 17:41:35 $
  * Last modified by:  $Author: qi.huang $
  *
  * Facilitates the editing of Train PA step parameters.
  *
  */

#include "StdAfx.h"

#ifdef _MSC_VER
#pragma warning(disable: 4503 4786)
#endif

#include <sstream>;
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/pa/src/PaTrainDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa/src/IPaTrainDvaMessage.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanStepEditPATrainPage.h"
#include "TrainPAStep.h"

namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditPATrainPage dialog

static const CString TRAIN					= "Single train";
static const CString IN_TRAINS			= "Inbound trains";
static const CString OUT_TRAINS			= "Outbound trains";
static const CString ALL_TRAINS			= "All trains";
const unsigned short TIS_UNDEFINED_TRAIN_ID = 0xFFFF;
const std::string NO_MESSAGE = "No message selected";
const std::string PA_TRAIN_MESSAGE_TYPE    = "Train Messages";

CPlanStepEditPATrainPage::CPlanStepEditPATrainPage(TrainPAStep* step) :
    CPlanStepEditParameterPage(IDD_PLAN_STEP_PA_TRAIN_DLG, step)
{
}

CPlanStepEditPATrainPage::~CPlanStepEditPATrainPage()
{
}

void CPlanStepEditPATrainPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlanStepEditPATrainPage)
	DDX_Control(pDX, IDC_TRAINPA_PA_TRAINS_COMBO, m_trainsCombo);
	DDX_Control(pDX, IDC_TRAINPA_PA_TRAIN_MSG_DLG_BTN, m_messageBtn);
	DDX_Control(pDX, IDC_TRAINPA_PA_TRAIN_MESSAGE_TXT, m_messageTxt);
	DDX_Control(pDX, IDC_TRAINPA_PA_TRAIN_GROUPES_COMBO, m_trainGroupsCombo);
	DDX_Control(pDX, IDC_TRAINPA_DURATION_DTPICKER, m_durationDTPicker);
	DDX_Control(pDX, IDC_TRAINPA_CYCLIC_CHECK, m_cyclicBtn);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPlanStepEditPATrainPage, CPlanStepEditParameterPage)
	//{{AFX_MSG_MAP(CPlanStepEditPATrainPage)
	ON_CBN_SELCHANGE(IDC_TRAINPA_PA_TRAIN_GROUPES_COMBO, OnSelchangePaTrainGroupesCombo)
	ON_BN_CLICKED(IDC_TRAINPA_PA_TRAIN_MSG_DLG_BTN, OnPaTrainMsgDlgBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditPATrainPage message handlers

BOOL CPlanStepEditPATrainPage::OnInitDialog ()
{
	if (!CPlanStepEditParameterPage::OnInitDialog ())
	{
		return FALSE;
	}

	PopulateTrainGroupsCombo ();

	// The duration date/time picker control will only be used for hours and minutes,
	// so set the format such that the date and the seconds are hidden.
	m_durationDTPicker.SetFormat("HH:mm");

	// Populate the train list. The contents will remain unchanged.
	m_trainsCombo.ResetContent();
	std::vector<ListItem> vecKeyItems;
	std::vector<unsigned int> vecSelected;
	unsigned int nMaxSelection = 0u;
	GetPlanStep()->getActivityParameter().getListPicker( ParamTrainId)->getInputListItems( ParamTrainId, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_trainsCombo.AddString( itLoop->Item.c_str() );
		m_trainsCombo.SetItemData(nIdx, itLoop->ID );
	}

	return TRUE;
}

void  CPlanStepEditPATrainPage::DisplayParameters()
{
	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamTrainId ) == 0 )
		{
			int trianId = atoi(itLoop->Item.c_str());
			int itemWidth = 2;
			std::stringstream ss;
			ss.clear();
			ss.str("");
			ss << std::setw(itemWidth) << std::setfill('0') << trianId;

			if (trianId > 0)
			{
				m_trainsCombo.SetCurSel (m_trainsCombo.FindStringExact(-1, ss.str().c_str()));
			}
		}
		else if ( itLoop->Name.compare( ParamTrainGroup ) == 0 )
		{
			m_trainGroupsCombo.SetCurSel (m_trainGroupsCombo.FindStringExact (0, itLoop->Item.c_str()));
            m_trainsCombo.EnableWindow((itLoop->Item.c_str() == TRAIN) ? TRUE : FALSE);
		}
		else if ( itLoop->Name.compare( ParamMessage ) == 0 )
		{
			m_messageTxt.SetWindowText( itLoop->Item.c_str() );
		}
	}

	fillTimePicker( ParamDuration, m_durationDTPicker );
	fillCheckbox( ParamCyclic, m_cyclicBtn );
}

void CPlanStepEditPATrainPage::UpdateParameters()
{
	std::vector<unsigned int> vecSelectedKeys;
	CString currentTrainGroup;
	m_trainGroupsCombo.GetWindowText(currentTrainGroup);
	if (TRAIN == currentTrainGroup)
	{
		int curSel = m_trainsCombo.GetCurSel ();
		unsigned int key = m_trainsCombo.GetItemData (curSel);
		vecSelectedKeys.push_back(key);
	}

	std::string strDisplayValue = "";
	GetPlanStep()->getActivityParameter().getListPicker(ParamTrainId)->pickFromList(ParamTrainId,vecSelectedKeys,strDisplayValue);

	int curSel = m_trainGroupsCombo.GetCurSel ();
	unsigned int key = m_trainGroupsCombo.GetItemData (curSel);
	vecSelectedKeys.clear();
	vecSelectedKeys.push_back(key);
	GetPlanStep()->getActivityParameter().getListPicker(ParamTrainGroup)->pickFromList(ParamTrainGroup,vecSelectedKeys,strDisplayValue);

	updateBooleanValue( ParamCyclic, m_cyclicBtn );
	updateTimeValue( ParamDuration, m_durationDTPicker );
}

void CPlanStepEditPATrainPage::OnSelchangePaTrainGroupesCombo()
{
	if (m_trainGroupsCombo.GetCurSel () == m_trainGroupsCombo.FindStringExact (0, TRAIN))
	{
		m_trainsCombo.EnableWindow (TRUE);
		m_trainsCombo.SetCurSel (0);
	}
	else
	{
		m_trainsCombo.SetCurSel (-1);
		m_trainsCombo.EnableWindow (FALSE);
	}
}

void CPlanStepEditPATrainPage::PopulateTrainGroupsCombo ()
{
	m_trainGroupsCombo.ResetContent();
	std::vector<ListItem> vecKeyItems;
	std::vector<unsigned int> vecSelected;
	unsigned int nMaxSelection = 0u;
	GetPlanStep()->getActivityParameter().getListPicker( ParamTrainGroup)->getInputListItems( ParamTrainGroup, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_trainGroupsCombo.AddString( itLoop->Item.c_str() );
		m_trainGroupsCombo.SetItemData(nIdx, itLoop->ID );
	}

	m_trainGroupsCombo.SetCurSel (0);
}

void CPlanStepEditPATrainPage::OnPaTrainMsgDlgBtn()
{
	popupList(ParamMessage, ParamMessage, m_messageTxt);
}
}