/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/TelephoneFacilitiesTab.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// TelephoneFacilitiesTab.cpp: implementation of the TelephoneFacilitiesTab class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "telephone_manager.h"
#include "TelephoneFacilitiesTab.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(TelephoneFacilitiesTab, CTabDialogCtrl)
    // {{AFX_MSG_MAP(TelephoneFacilitiesTab)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TelephoneFacilitiesTab::TelephoneFacilitiesTab() : CTabDialogCtrl()
{
    FUNCTION_ENTRY( "TelephoneFacilitiesTab" );

    this->AddPage(m_directLineDialog,IDD_DIRECT_LINE,CString("Direct Lines"));
    this->AddPage(m_directorySearchDialog,IDD_FACILITY_SEARCH,CString("Search"));
    this->AddPage(m_configureDirectLinesDialog,IDD_FACILITY_CONFIG_LINES,CString("Config Lines"));
    this->AddPage(m_configureDirectoryDialog,IDD_FACILITY_CONFIG_DIRECTORY,CString("Config Directory"));

    FUNCTION_EXIT;
}


TelephoneFacilitiesTab::~TelephoneFacilitiesTab()
{
    FUNCTION_ENTRY( "~TelephoneFacilitiesTab" );
    FUNCTION_EXIT;
}


