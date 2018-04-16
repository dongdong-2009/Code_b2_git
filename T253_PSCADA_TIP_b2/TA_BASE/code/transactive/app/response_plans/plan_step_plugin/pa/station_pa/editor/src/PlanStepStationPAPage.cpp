//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepStationPAPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2014/10/15 17:41:35 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/pa/src/PaStationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/pa/src/IPaStation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/pa/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/pa/src/IPaZone.h"
#include "core/data_access_interface/pa/src/PaDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa/src/IPaDvaMessage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "PAMessagesDlg.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PAStationZones.h"
#include "PlanStepStationPAPage.h"

namespace TA_Base_App
{
static const std::string ALL_STATIONS     ("<All Stations>");
const unsigned int MIN_PA_MESSAGE_INTERVAL = 0;//seconds
const unsigned int MAX_PA_MESSAGE_INTERVAL = 10;//seconds
const std::string NO_MESSAGE = "No message selected";
const std::string PA_STATION_MESSAGE_TYPE = "PA Station Message";

/////////////////////////////////////////////////////////////////////////////
// CPlanStepStationPAPage property page

IMPLEMENT_DYNAMIC(CPlanStepStationPAPage, CPlanStepEditParameterPage)

CPlanStepStationPAPage::CPlanStepStationPAPage(StationPAStep* step)
  : CPlanStepEditParameterPage(IDD_PLAN_STEP_PA_STATION_DLG,step)
{
    FUNCTION_ENTRY( "CPlanStepStationPAPage" );
    FUNCTION_EXIT;
}

CPlanStepStationPAPage::~CPlanStepStationPAPage()
{
    FUNCTION_ENTRY( "~CPlanStepStationPAPage" );
    FUNCTION_EXIT;
}

void CPlanStepStationPAPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepStationPAPage)
    DDX_Control(pDX, IDC_STATIONPA_DURATION_DTPICKER, m_durationTime);
    DDX_Control (pDX, IDC_STATIONPA_INTERVAL_TXT, m_interval);
    DDX_Control(pDX, IDC_STATIONPA_PA_STATION_RADIO, m_stationBtn);
	DDX_Control(pDX, IDC_STATIONPA_ALL_PA_STATIONS_RADIO, m_allStationBtn);
    DDX_Control (pDX, IDC_STATIONPA_CHIME_CHECK, m_chimeBtn);
    DDX_Control (pDX,IDC_STATIONPA_CYCLIC_CHECK, m_cyclicBtn);
    DDX_Control (pDX,IDC_STATIONPA_TIS_SYNCH_CHECK, m_TISSynchBtn);
    DDX_Control(pDX, IDC_STATIONPA_STATIONS_COMBO, m_stationCombo);
    DDX_Control(pDX, IDC_STATIONPA_ZONE_LIST, m_stationZonesLst);
    DDX_Control (pDX, IDC_STATIONPA_MESSAGE_TXT, m_messageNameTxt);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}

BEGIN_MESSAGE_MAP(CPlanStepStationPAPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepStationPAPage)
        ON_BN_CLICKED (IDC_STATIONPA_PA_STATION_RADIO, OnStationBtn)
        ON_BN_CLICKED (IDC_STATIONPA_ALL_PA_STATIONS_RADIO, OnStationBtn)
        ON_CBN_SELCHANGE (IDC_STATIONPA_STATIONS_COMBO, OnStationChange)
        ON_BN_CLICKED (IDC_STATIONPA_ZONE_DLG__BTN, OnZonesBtn)
        ON_BN_CLICKED (IDC_STATIONPA_MSG_DLG_BTN, OnMessagesBtn)
        ON_BN_CLICKED (IDC_STATIONPA_CYCLIC_CHECK, OnCyclicBtn)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepStationPAPage message handlers

BOOL CPlanStepStationPAPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    std::vector<TA_Base_Core::ILocation*> vecLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
    for ( std::vector<TA_Base_Core::ILocation*>::iterator itLoop = vecLocations.begin();
        vecLocations.end() != itLoop; ++itLoop )
    {
        m_mapLocationKeyToName[(*itLoop)->getKey()] = (*itLoop)->getDisplayName();
        delete (*itLoop);
        (*itLoop) = NULL;
    }

	m_interval.SetFormat("ss");
    m_durationTime.SetFormat("    HH:mm:ss");
    // populate stations
    PopulateStationsCombo ();
	m_stationCombo.EnableWindow (m_stationBtn.GetCheck ());

    m_stationZonesLst.EnableWindow( TRUE );

    FUNCTION_EXIT;
    return TRUE;
}

void CPlanStepStationPAPage::EnableParameterControls(bool enable)
{
    FUNCTION_ENTRY( "EnableParameterControls" );
    FUNCTION_EXIT;
}

void CPlanStepStationPAPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamAllStations ) == 0 )
		{
			m_allStationBtn.SetCheck (itLoop->Item == "Yes" ? TRUE : FALSE);
		}
		else if ( itLoop->Name.compare( ParamStation ) == 0 )
		{
			if (itLoop->Item.empty())
			{
				m_stationBtn.SetCheck(FALSE);
				m_stationCombo.EnableWindow(FALSE);
			}
			else
			{
                m_stationBtn.SetCheck(TRUE);
				m_stationCombo.EnableWindow(TRUE);
				m_stationCombo.SetCurSel(m_stationCombo.FindStringExact( 0, itLoop->Item.c_str()));
			}
		}
		else if ( itLoop->Name.compare( ParamMessage ) == 0 )
		{
			m_messageNameTxt.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamZones) == 0 )
		{
			m_stationZonesLst.ResetContent();
			char *tokenPtr=strtok(const_cast<char*>(itLoop->Item.c_str()),",");
			while(tokenPtr!=NULL){
                m_stationZonesLst.AddString ( tokenPtr );
				tokenPtr=strtok(NULL,",");
			}
		}
	}

	fillTimePicker( ParamDuration, m_durationTime );
	fillTimePicker( ParamInterval, m_interval );
	fillCheckbox( ParamChime, m_chimeBtn );
	fillCheckbox( ParamCyclic, m_cyclicBtn );
	fillCheckbox( ParamTISSynch, m_TISSynchBtn );

	m_durationTime.EnableWindow (m_cyclicBtn.GetCheck ());
	m_interval.EnableWindow (m_cyclicBtn.GetCheck ());
	m_interval.EnableWindow  (m_cyclicBtn.GetCheck ());

	if (!m_cyclicBtn.GetCheck ())
	{
	  m_TISSynchBtn.SetCheck (!m_cyclicBtn.GetCheck ());
	}

	m_TISSynchBtn.EnableWindow(!m_cyclicBtn.GetCheck ());

    FUNCTION_EXIT;
}

void CPlanStepStationPAPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	std::vector<unsigned int> vecSelectedKeys;
	if (m_stationBtn.GetCheck() == BST_CHECKED)
	{
		int curSel = m_stationCombo.GetCurSel();
		unsigned int key = m_stationCombo.GetItemData(curSel);
		vecSelectedKeys.push_back(key);
	}
	std::string strDisplayValue = "";
	GetPlanStep()->getActivityParameter().getListPicker(ParamStation)->pickFromList(ParamStation,vecSelectedKeys,strDisplayValue);

	updateBooleanValue( ParamAllStations, m_allStationBtn );
	updateBooleanValue( ParamChime, m_chimeBtn);
	updateBooleanValue( ParamCyclic, m_cyclicBtn );
	updateBooleanValue( ParamTISSynch, m_TISSynchBtn );
	updateTimeValue( ParamDuration, m_durationTime );
	updateTimeValue( ParamInterval, m_interval );

    FUNCTION_EXIT;
}

void CPlanStepStationPAPage::OnStationBtn ()
{
    FUNCTION_ENTRY( "OnStationBtn" );

    int oldSel = m_stationCombo.GetCurSel ();
    if (m_stationBtn.GetCheck ())
    {
        m_stationCombo.SetCurSel (m_stationCombo.FindStringExact (0, ALL_STATIONS.c_str()) + 1);
    }
    else
    {
        m_stationCombo.SetCurSel (m_stationCombo.FindStringExact (0, ALL_STATIONS.c_str()));
    }

    m_stationCombo.EnableWindow (m_stationBtn.GetCheck ());

    // reset zones if selection changed
    if (oldSel != m_stationCombo.GetCurSel ())
    {
        m_stationZonesLst.ResetContent ();
        //m_zoneIds.SetSize (0);
    }

    UpdateData ();

    FUNCTION_EXIT;
}

void CPlanStepStationPAPage::OnStationChange ()
{
    FUNCTION_ENTRY( "OnStationChange" );

    int selectedStationIdx = m_stationCombo.GetCurSel();

    // reset zones
    m_stationZonesLst.ResetContent ();

    FUNCTION_EXIT;
}

void CPlanStepStationPAPage::OnZonesBtn ()
{
    FUNCTION_ENTRY( "OnZonesBtn" );

	popupList(ParamZones, ParamZones, m_stationZonesLst);

	std::string zoneValues = GetPlanStep()->getActivityParameter().displayValue(ParamZones);
	m_stationZonesLst.ResetContent();
	char *tokenPtr=strtok(const_cast<char*>(zoneValues.c_str()),",");
	while(tokenPtr!=NULL){
		m_stationZonesLst.AddString ( tokenPtr );
		tokenPtr=strtok(NULL,",");
	}
    FUNCTION_EXIT;
}

void CPlanStepStationPAPage::OnMessagesBtn ()
{
    FUNCTION_ENTRY( "OnMessagesBtn" );

    popupList(ParamMessage, ParamMessage, m_messageNameTxt);

    FUNCTION_EXIT;
}

void CPlanStepStationPAPage::OnCyclicBtn ()
{
    FUNCTION_ENTRY( "OnCyclicBtn" );

    m_durationTime.EnableWindow (m_cyclicBtn.GetCheck ());
    m_interval.EnableWindow (m_cyclicBtn.GetCheck ());
    m_TISSynchBtn.SetCheck (!m_cyclicBtn.GetCheck ());
    m_TISSynchBtn.EnableWindow (!m_cyclicBtn.GetCheck ());
    UpdateData ();

    FUNCTION_EXIT;
}

void CPlanStepStationPAPage::PopulateStationsCombo ()
{
    FUNCTION_ENTRY( "PopulateStationsCombo" );

	m_stationCombo.ResetContent();
	std::vector<ListItem> vecKeyItems;
	std::vector<unsigned int> vecSelected;
	unsigned int nMaxSelection = 0u;
	GetPlanStep()->getActivityParameter().getListPicker( ParamStation)->getInputListItems( ParamStation, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_stationCombo.AddString( itLoop->Item.c_str() );
		m_stationCombo.SetItemData(nIdx, itLoop->ID );
	}

    FUNCTION_EXIT;
}
}