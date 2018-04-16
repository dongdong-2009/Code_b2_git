/*
 * The soiurce code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/directline.cpp $
 * @author:  Andy Parker
 * @version: $Revision: #1 $
 *
 * Last mofification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This class manages the direct dial property page for the
 * radio and telephone sub systems.  It is implemented as 
 * eight fixed buttons (hotlines) and 36 pageable buttons switched
 * using a tab control.
 *
 * Each button has a label and associated number.
 *
 */

// System includes
#include "stdafx.h"
#include "resource.h"

// Application includes
#include "DirectLine.h"
#include "DirectLineCache.h"
#include "SelectedCall.h"
#include "RadioManagerDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


// MFC Message Mapping
IMPLEMENT_DYNCREATE(CDirectLinePage, CDialog)
BEGIN_MESSAGE_MAP(CDirectLinePage, CDialog)
	//{{AFX_MSG_MAP(CDirectLinePage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	ON_COMMAND_RANGE(IDC_HOT_BUTTON1,IDC_HOT_BUTTON8,OnHotButtonPress)
	ON_COMMAND_RANGE(IDC_DIRECT_LINE_BUTTON_1,IDC_DIRECT_LINE_BUTTON_36,OnDirectButtonPress)
    ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
  * CDirectLinePage Constructor
  *
  * This class constucts the class
  *
  */
CDirectLinePage::CDirectLinePage() : CDialog(CDirectLinePage::IDD)
{
	//{{AFX_DATA_INIT(CDirectLinePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/**
  * CDirectLinePage Destructor
  *
  * This class cleans up the class
  *
  */
CDirectLinePage::~CDirectLinePage()
{
}

/**
  * OnInitDialog
  *
  * This class is called when the dialog window is first created.
  *
  */
BOOL CDirectLinePage::OnInitDialog() 
{
	// Perform normal initialisation
	CDialog::OnInitDialog();

 	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDirectLinePage::Init()
{
	try
    {
        // Load the data into the cache
        CDirectLineCache::GetInstance()->InitData(m_radioName);
	
    	// Setup the tab control
    	SetupTabControl();
    	SetupHotButtons();
    	m_automaticCallCheck.SetCheck(1);
    
    	// Load the buttons for the selected tab
    	DisplayButtons();
    
    	onUpdateRights();
    }
    SILENT_RM_CATCH_HANDLER("CDirectLinePage::OnInitDialog");
}

void CDirectLinePage::OnDestroy() 
{
    try
    {
        CDirectLineCache::GetInstance()->UnInitData();
    }
    SILENT_RM_CATCH_HANDLER("CDirectLinePage::OnDestroy");
}


void CDirectLinePage::onUpdateRights()
{
    try
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
    
    	bool enable = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_ANSWER_CALL);
    
        m_automaticCallCheck.EnableWindow(enable);
    	if (!enable)
    	{
    		// Remove check if not allowed to make calls
    		m_automaticCallCheck.SetCheck(0);
    	}
    }
    SILENT_RM_CATCH_HANDLER("CDirectLinePage::onUpdateRights");
}


/**
  * DoDataExchange
  *
  * This method transfers the data between class variables and dislog controls
  *
  */
void CDirectLinePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirectLinePage)
	DDX_Control(pDX, IDC_AUTOMATIC_CALL, m_automaticCallCheck);
	DDX_Control(pDX, IDC_TAB1, m_TabControl);
	//}}AFX_DATA_MAP
}

afx_msg void CDirectLinePage::OnCancel()
{
	// Do nothing
}

afx_msg void CDirectLinePage::OnOK()
{
	// Do nothing
}


/**
  * SetupTabControl
  *
  * Load the headings for the tab control
  *
  */
void CDirectLinePage::SetupTabControl(void)
{
    try
    {
    	CDirectLineCache*  pLines = CDirectLineCache::GetInstance();
    
    	for (UINT iTab=0;iTab<pLines->GetButtonSetCount();iTab++)
    	{
    		CString setName = pLines->GetButtonSetName(iTab);
    		m_TabControl.InsertItem(iTab,setName);
    	}
    }
    SILENT_RM_CATCH_HANDLER("CDirectLinePage::SetupTabControl");
}

/**
  * SetupHotButtons
  *
  * This method will setup the hot buttons
  * Note:  It assumes that the buttons on the dialog are sequentially
  *        numbered starting with IDC_HOT_BUTTON1
  *
  */
void CDirectLinePage::SetupHotButtons(void)
{
	CString		buttonLabel;

    try
    {

    	CDirectLineCache*  pLines = CDirectLineCache::GetInstance();
    
    	// Display the buttons
    	for (int iButton=0;iButton<pLines->GetHotButtonsInSet();iButton++)
    	{
    		// Get the button label
    		buttonLabel = pLines->GetHotButtonLabel(iButton);
    
    		// Update the button
            CWnd*  pWnd = GetDlgItem(IDC_HOT_BUTTON1+iButton);
    		ASSERT (pWnd!=NULL);
    		if (buttonLabel.IsEmpty())
    		{
    			pWnd->ShowWindow(SW_HIDE);
    		}
    		else
    		{
    			pWnd->ShowWindow(SW_NORMAL);
    			pWnd->SetWindowText(buttonLabel);
    		}
    	}
    }
    SILENT_RM_CATCH_HANDLER("CDirectLinePage::SetupHotButtons");
}


/**
  * DisplayButtons
  *
  * This method will setup the buttons for the selected tab page
  * Note:  It assumes that the buttons on the dialog are sequentially
  *        numbered starting with IDC_DIRECT_LINE_BUTTON_1
  *
  */
void CDirectLinePage::DisplayButtons(void)
{
	int			iTab;
	UINT		iButton;
	CString		buttonLabel;

    try
    {
    	CDirectLineCache*  pLines = CDirectLineCache::GetInstance();
    
    	// Get the selected tab page
        iTab = m_TabControl.GetCurSel();
    
    	// Display the buttons
    	for (iButton=0;iButton<(1+IDC_DIRECT_LINE_BUTTON_36-IDC_DIRECT_LINE_BUTTON_1);iButton++)
    	{
    		if (iButton<pLines->GetButtonsPerSet(iTab))
    		{
    			// Get the button label
    			buttonLabel = pLines->GetButtonLabel(iTab,iButton);
    		}
    		else
    		{
    			buttonLabel = "";
    		}
    
    		// Update the button
            CWnd*  pWnd = GetDlgItem(IDC_DIRECT_LINE_BUTTON_1+iButton);
    		ASSERT (pWnd!=NULL);
    		if (buttonLabel.IsEmpty())
    		{
    			pWnd->ShowWindow(SW_HIDE);
    		}
    		else
    		{
    			pWnd->SetWindowText(buttonLabel);
    			pWnd->ShowWindow(SW_NORMAL);
    		}
    	}
    }
    SILENT_RM_CATCH_HANDLER("CDirectLinePage::DisplayButtons");

}


/**
  * OnSelchangeTab1
  *
  * This method is called by the framework when a tab selection is changed
  * for the direct line buttons.  It must re-draw the selected buttons.
  */
void CDirectLinePage::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Update the displayed buttons
	DisplayButtons();
	*pResult = 0;
}



/**
  * OnHotButtonPress
  *
  * This method is called when a hot button is pressed.  It will
  * identify which hot button was activated and get the TSI
  * number associated with it.  The details will then be displayed
  * in the SelectedArea.
  */
afx_msg void CDirectLinePage::OnHotButtonPress(UINT ctrlID)
{
	UINT               buttonNumber = ctrlID - IDC_HOT_BUTTON1;

    try
    {
	    CDirectLineCache*  pLines = CDirectLineCache::GetInstance();
	    CString			   tsiNumber;
	    CString            buttonLabel;
        
	    // Get the TSI associated with the button
	    tsiNumber = pLines->GetHotButtonNumber(buttonNumber);
        
	    // Get the button label
	    GetDlgItemText(ctrlID,buttonLabel);
        
	    // Place in selected area
	    CSelectedCall::GetInstance()->SetCall(tsiNumber, buttonLabel, true);
        
	    // Active the call automatically
	    if (m_automaticCallCheck.GetCheck()!=0)
	    {
	    	AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND,IDC_CALL);
	    }
    }
    SILENT_RM_CATCH_HANDLER("CDirectLinePage::OnHotButtonPress");

}

/**
  * OnDirectButtonPress
  *
  * This method is called when a direct button is pressed.  It will
  * identify which hot button was activated and get the TSI
  * number associated with it.  The details will then be displayed
  * in the SelectedArea.
  */
afx_msg void CDirectLinePage::OnDirectButtonPress(UINT ctrlID)
{
	UINT               buttonNumber = ctrlID - IDC_DIRECT_LINE_BUTTON_1;

    try
    {
    	CDirectLineCache*  pLines = CDirectLineCache::GetInstance();
    	CString			   tsiNumber;
    	CString            buttonLabel;
    
    	// Determine which button set is in use
        int iTab = m_TabControl.GetCurSel();
    
    	// Get the TSI associated with the button
    	tsiNumber = pLines->GetButtonNumber(iTab,buttonNumber);
    
    	// Get the button label
    	GetDlgItemText(ctrlID,buttonLabel);
    
    	// Place in selected area
    	CSelectedCall::GetInstance()->SetCall(tsiNumber, buttonLabel, false);
    
    	// Active the call automatically
    	if (m_automaticCallCheck.GetCheck()!=0)
    	{
    		AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND,IDC_CALL);
    	}
    }
    SILENT_RM_CATCH_HANDLER("CDirectLinePage::OnDirectButtonPress");

}
