/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/templates/PropertySheet/src/TemplatePropertySheetDlg.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */

#include "stdafx.h"
#include "TemplatePropertySheet.h"
#include "resource.h"
#include "TemplatePropertySheetDlg.h"

#include "bus\application_types\src\apptypes.h"
#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\DebugUtil.h"

#include "bus\user_settings\src\SettingsMgr.h"
#include "core\exceptions\src\UserSettingsException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;
using TA_Base_Bus::TransActivePropertySheet;

/////////////////////////////////////////////////////////////////////////////
// CTemplatePropertySheetDlg dialog
IMPLEMENT_DYNAMIC(CTemplatePropertySheetDlg, TransActivePropertySheet)

CTemplatePropertySheetDlg::CTemplatePropertySheetDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/)
	: TransActivePropertySheet(genericGUICallback, CTemplatePropertySheetDlg::IDD, pParent)
{
	// If necessary this code can go in OnInitDialog before the call to TransActiveDialog::OnInitDialog()
    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minWidth = -1;
    properties.minHeight = -1;
    setResizingProperties(properties);

}

void CTemplatePropertySheetDlg::DoDataExchange(CDataExchange* pDX)
{
	TransActivePropertySheet::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplatePropertySheetDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTemplatePropertySheetDlg, TransActivePropertySheet)
	//{{AFX_MSG_MAP(CTemplatePropertySheetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplatePropertySheetDlg message handlers

BOOL CTemplatePropertySheetDlg::OnInitDialog()
{
	TransActivePropertySheet::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

