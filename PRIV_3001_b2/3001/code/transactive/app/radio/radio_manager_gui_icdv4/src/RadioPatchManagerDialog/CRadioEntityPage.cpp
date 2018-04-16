/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioPatchManagerDialog/CRadioEntityPage.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioPatchManagerDialog/CRadioEntityPage.h"
//#include "app/radio/radio_manager_gui_icdv4/src/RadioPatchManagerDialog/PatchOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/GenericDialpadDialog/CGenericDialpadDlg.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "core/data_access_interface/entity_access/src/RadioProfileEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioEntityPage dialog


CRadioEntityPage::CRadioEntityPage(CWnd* pParent /*=NULL*/)
	: CDialog(CRadioEntityPage::IDD, pParent), 
	  m_recentDialedList(NULL),
	  m_launchedFromRadio(false),
	  m_radioResource(NULL)
	  //m_telresource(NULL)
{
	//{{AFX_DATA_INIT(CRadioEntityPage)
	m_RadioEntityDescriptionStr = _T("");
	m_RadioEntityIDStr = _T("");
	//}}AFX_DATA_INIT

}


void CRadioEntityPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioEntityPage)
	DDX_Control(pDX, IDC_PATCHMANAGER_RADIO_RELEASE, m_RadioEntityReleaseButton);
	DDX_Control(pDX, IDC_PATCHMANAGER_RADIO_DIALPAD, m_RadioEntityDisplayDialpadButton);
	DDX_Control(pDX, IDC_PATCHMANAGER_RADIO_CALL, m_RadioEntityCallButton);
	DDX_Control(pDX, IDC_ENTITY_NUMBER, m_RadioEntityIDCBCtrl);
	DDX_Control(pDX, IDC_ENTITY_DESCRIPTION, m_RadioEntityDescriptionCBCtrl);
	DDX_CBString(pDX, IDC_ENTITY_DESCRIPTION, m_RadioEntityDescriptionStr);
	DDX_CBString(pDX, IDC_ENTITY_NUMBER, m_RadioEntityIDStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRadioEntityPage, CDialog)
	//{{AFX_MSG_MAP(CRadioEntityPage)
	ON_CBN_SELCHANGE(IDC_ENTITY_DESCRIPTION, OnDropdownRadioEntityDescription)
	ON_CBN_SELCHANGE(IDC_ENTITY_NUMBER, OnDropdownRadioEntityID)
	ON_BN_CLICKED(IDC_PATCHMANAGER_RADIO_CALL, OnRadioEntityCall)
	ON_BN_CLICKED(IDC_PATCHMANAGER_RADIO_RELEASE, OnRadioEntityRelease)
	ON_BN_CLICKED(IDC_PATCHMANAGER_RADIO_DIALPAD, OnRadioEntityDialpad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioEntityPage message handlers

BOOL CRadioEntityPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_radioResource->type == RR_NIL)
	{
		initialize(RR_RADIO);
	}
	else
	{
		initialize(m_radioResource->type);
	}
	
	HBITMAP lBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_DIAL), IMAGE_BITMAP,
		0, 0, LR_SHARED | LR_DEFAULTSIZE);

	m_RadioEntityDisplayDialpadButton.SetBitmap(lBitmap);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CRadioEntityPage::OnDropdownRadioEntityDescription() 
{
	int index = m_RadioEntityDescriptionCBCtrl.GetCurSel();
	
	if (index != CB_ERR)
	{
		m_RadioEntityIDCBCtrl.ResetContent();
		
		std::vector<RadioResource>::iterator iter;

		CString desc;
		m_RadioEntityDescriptionCBCtrl.GetLBText(index,desc);

		for (iter=m_resourceList.begin(); iter!=m_resourceList.end();iter++)
		{
			if (desc == iter->alias.c_str())
			{
				m_radioResource->id    = iter->id;
				m_radioResource->alias = iter->alias;
				m_radioResource->type  = iter->type;
				m_radioResource->ssi   = iter->ssi;

				std::ostringstream unitId;
				unitId << iter->ssi;
				
				m_RadioEntityIDCBCtrl.InsertString(0,unitId.str().c_str());
				m_RadioEntityIDCBCtrl.SetCurSel(0);
				break;
			}
		}
	}
	UpdateData();
}


void CRadioEntityPage::OnDropdownRadioEntityID() 
{
	UpdateData();
}


void CRadioEntityPage::OnRadioEntityCall() 
{
	if (m_radioResource == NULL)
	{
		MessageBox("Please select a valid Radio resource.", "Tips");
		return;
	}
	// if launched by TelephoneManager
	if (!m_launchedFromRadio)
	{
		//The_PatchOperations::instance()->setPatchType(PatchOperations::P_DLTS_RADIO);
		// Step 1 of P_DLTS_RADIO
		// DLTS SAT - Hold the call when a private call has already been established
/*		try
		{
			// use HOLD, forward operation
			The_PatchOperations::instance()->holdDLTSCall(*m_telresource,false); 
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			MessageBox(e.what(),"Error");
		}*/
		
		// Step 2 of P_DLTS_RADIO
		if (m_radioResource->type == RR_RADIO)
		{
			if (!The_RadioOperations::instance()->getResource_isInPrivateCall(*m_radioResource))
			{
				The_RadioOperations::instance()->makePrivateCall(*m_radioResource);
			}
			else
			{
				MessageBox("Radio resource is in private call", "Error");
			}
		}
		else if ((m_radioResource->type==RR_TALKGROUP) || (m_radioResource->type==RR_DYNGROUP))
		{
			//The_RadioOperations::instance()->selectSpeaker(*m_radioResource);
		}
		else
		{
			MessageBox("Radio resource type is invalid", "Error");
		}

	}
}


void CRadioEntityPage::OnRadioEntityRelease() 
{
	if (m_radioResource->type == RR_RADIO)
	{
		The_RadioOperations::instance()->endPrivateCall(*m_radioResource);
	}
	else if ((m_radioResource->type==RR_TALKGROUP) || (m_radioResource->type==RR_DYNGROUP))
	{
		//The_RadioOperations::instance()->unselectSpeaker(*m_radioResource);
	}
}


void CRadioEntityPage::OnRadioEntityDialpad() 
{
    int maxDialLength = 0;
    int maxHistoryLength = 0;

    try
    {
        maxDialLength = The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::DIALPAD_MAXNUMBER_LENGTH); 
		maxHistoryLength = The_RadioSystemStatus::instance()->getConfigValue(TA_Base_Core::RadioProfileEntityData::DIALPAD_HISTORY_LENGTH);
    }
    catch (...)
    {
    }

	CGenericDialpadDlg dlg(this, maxDialLength, maxHistoryLength);

	dlg.setRecentlyDialedList(m_recentDialedList);
    dlg.DoModal();	

	CString sNumber = dlg.userDialedNumber();
	
	if ( !sNumber.IsEmpty() )
	{
		m_RadioEntityIDCBCtrl.InsertString(0,sNumber);
		m_RadioEntityIDCBCtrl.SetCurSel(0);

		m_RadioEntityIDStr = sNumber;
	}
}


void CRadioEntityPage::setRecentlyDialedRadioEntity(std::list<std::string>* recentDialedList)  
{
	m_recentDialedList = recentDialedList;
}


void CRadioEntityPage::setRadioResource(RadioResource* resource)
{
	m_radioResource = resource;

	if (resource->type != RR_NIL)
	{
		m_launchedFromRadio = true;
	}
}


void CRadioEntityPage::initialize(RadioResourceType type)
{
	// populates m_RadioEntityDescriptionCBCtrl
	m_RadioEntityDescriptionCBCtrl.ResetContent();
	m_RadioEntityIDCBCtrl.ResetContent();

	m_resourceList = The_RadioSystemStatus::instance()->getResourceCollectioForType(type);

	std::vector<RadioResource>::iterator iter;

	int index = 0;

	for (iter=m_resourceList.begin(); iter != m_resourceList.end();++iter)
	{
		m_RadioEntityDescriptionCBCtrl.AddString((*iter).alias.c_str());

		if ( (m_launchedFromRadio) && (iter->id == m_radioResource->id))
		{
			index = m_RadioEntityDescriptionCBCtrl.GetCount() - 1;
		}
	}

	if (m_launchedFromRadio)
	{
		m_RadioEntityDescriptionCBCtrl.SetCurSel(index);

		std::ostringstream num;
		num << m_radioResource->ssi;
		
		m_RadioEntityIDCBCtrl.AddString(num.str().c_str());
		m_RadioEntityIDCBCtrl.SetCurSel(0);
	}

/*	// Display description & id of launched from SelectedEntityDetailsArea dialog
	if ( !m_isLaunchedFromToolbar )
	{
		m_RadioEntityIDCBCtrl.SetCurSel(0);
		m_RadioEntityDescriptionCBCtrl.SetCurSel(0);
		
		UpdateData();
	}*/
}


