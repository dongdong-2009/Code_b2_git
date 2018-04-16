/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/generic_gui/templates/Dialog/src/TemplateDialogDlg.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  *
  */

#include "stdafx.h"
#include "TemplateDialog.h"
#include "resource.h"
#include "TemplateDialogDlg.h"

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
using TA_Base_Bus::TransActiveDialog;

/////////////////////////////////////////////////////////////////////////////
// CTemplateDialogDlg dialog

CTemplateDialogDlg::CTemplateDialogDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/)
	: TransActiveDialog(genericGUICallback, CTemplateDialogDlg::IDD, pParent)
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

void CTemplateDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	TransActiveDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateDialogDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTemplateDialogDlg, TransActiveDialog)
	//{{AFX_MSG_MAP(CTemplateDialogDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateDialogDlg message handlers

BOOL CTemplateDialogDlg::OnInitDialog()
{
	TransActiveDialog::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

