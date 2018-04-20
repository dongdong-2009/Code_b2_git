/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioFacilities.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// RadioFacilities.cpp : implementation file
//

#include "stdafx.h"
#include "radiomanager.h"
#include "RadioFacilities.h"
#include "SearchPageHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioFacilities

CRadioFacilities::CRadioFacilities() : CTabDialogCtrl(),
m_SearchPage(TA_IRS_App::SearchPageHelper::getInstance())
{
	AddPage(m_DirectLinePage,IDD_FACILITYLINESv2,CString("Direct lines"));
	AddPage(m_SearchPage,IDD_RADIO_FACILITYSEARCH,CString("Search"));
	AddPage(m_ConfigurePage,IDD_FACILITYCONFIG,CString("Configure"));
	AddPage(m_groupPatchPage,IDD_FACILITYGROUPPATCH,CString("Group Patch"));

    TA_IRS_App::SearchPageHelper::getInstance().setSearchPage(m_SearchPage);
}

CRadioFacilities::~CRadioFacilities()
{
}

void CRadioFacilities::Init()
{
	m_DirectLinePage.Init();
	m_SearchPage.Init();
	m_ConfigurePage.Init();
	m_groupPatchPage.Init();
}

void CRadioFacilities::onUpdateRights()
{
	m_DirectLinePage.onUpdateRights();
	m_SearchPage.onUpdateRights();
	m_ConfigurePage.onUpdateRights();
	m_groupPatchPage.onUpdateRights();
}


BEGIN_MESSAGE_MAP(CRadioFacilities, CTabDialogCtrl)
	//{{AFX_MSG_MAP(CRadioFacilities)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioFacilities message handlers
