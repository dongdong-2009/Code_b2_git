/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/DirectLineConfigurePage.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/31 14:50:51 $
  * Last modified by:  $Author: builder $
  *
  */
// FacilitiesPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DirectLineConfigurePage.h"
#include "DirectLineCache.h"
#include "RadioManagerDlg.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CDirectLineConfigurePage, CDialog)




/////////////////////////////////////////////////////////////////////////////
// CConfigurePage property page

CDirectLineConfigurePage::CDirectLineConfigurePage() 
                         : CDialog(CDirectLineConfigurePage::IDD),
 						   m_buttonNumber(0),
					   	   m_isHotButton(false)

{
	//{{AFX_DATA_INIT(CConfigurePage)
	//}}AFX_DATA_INIT
}

CDirectLineConfigurePage::~CDirectLineConfigurePage()
{
}

void CDirectLineConfigurePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirectLineConfigurePage)
	DDX_Control(pDX, IDC_TAB1, m_TabControl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDirectLineConfigurePage, CDialog)
	//{{AFX_MSG_MAP(CDirectLineConfigurePage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	ON_BN_CLICKED(IDC_SETBUTTON, OnSetButton)
	ON_COMMAND_RANGE(IDC_HOT_BUTTON1,IDC_HOT_BUTTON8,OnHotButtonPress)
	ON_COMMAND_RANGE(IDC_DIRECT_LINE_BUTTON_1,IDC_DIRECT_LINE_BUTTON_36,OnDirectButtonPress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CDirectLineConfigurePage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Setup the tab control
	m_TabControl.InsertItem(0,"User");
	m_TabControl.InsertItem(1,"Stations");
	m_TabControl.InsertItem(2,"OCR");
	m_TabControl.InsertItem(3,"DCR");
	m_TabControl.InsertItem(4,"Others");
	

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDirectLineConfigurePage::Init()
{
	// Load the buttons for the selected tab
	DisplayButtons();

	// Load the hot buttons
	SetupHotButtons();

	// Limit the label to 10 characters
	CWnd* pWnd = GetDlgItem(IDC_EDITLABEL);
	if (pWnd!=NULL)
	{
		CEdit*  pEdit = reinterpret_cast<CEdit*>(pWnd);
		if (pEdit!=NULL) pEdit->SetLimitText(10);
	}
	// Limit ITSI to 32 characters
	pWnd = GetDlgItem(IDC_EDITTETRAID);
	if (pWnd!=NULL)
	{
		CEdit*  pEdit = reinterpret_cast<CEdit*>(pWnd);
		if (pEdit!=NULL) pEdit->SetLimitText(32);
	}

	onUpdateRights();
}

void CDirectLineConfigurePage::onUpdateRights()
{
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

    int iTab = m_TabControl.GetCurSel();
	if (iTab==0)
	{
		GetDlgItem(IDC_SETBUTTON)->EnableWindow(manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_CONFIGURE_USER_DIRECT_LINE));
	}
	else
	{
		GetDlgItem(IDC_SETBUTTON)->EnableWindow(manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_CONFIGURE_SHARED_DIRECT_LINE));
	}		
}


afx_msg void CDirectLineConfigurePage::OnCancel()
{
	// Do nothing
}

afx_msg void CDirectLineConfigurePage::OnOK()
{
	// Do nothing
}

/**
  * DisplayButtons
  *
  * This method will setup the buttons for the selected tab page
  * Note:  It assumes that the buttons on the dialog are sequentially
  *        numbered starting with IDC_DIRECT_LINE_BUTTON_1
  *
  */
void CDirectLineConfigurePage::DisplayButtons()
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
    		pWnd->SetWindowText(buttonLabel);
    		pWnd->ShowWindow(SW_NORMAL);
    	}
    }
    SILENT_RM_CATCH_HANDLER("CDirectLineConfigurePage::DisplayButtons");

}

/**
  * SetupHotButtons
  *
  * This method will setup the hot buttons
  * Note:  It assumes that the buttons on the dialog are sequentially
  *        numbered starting with IDC_HOT_BUTTON1
  *
  */
void CDirectLineConfigurePage::SetupHotButtons()
{
	CString		buttonLabel;

    try
    {
    	CDirectLineCache*  pLines = CDirectLineCache::GetInstance();
    
    	// Display the buttons
    	for (int iButton = 0;iButton < (1+IDC_HOT_BUTTON8-IDC_HOT_BUTTON1);iButton++)
    	{
    		// Get the button label
    		buttonLabel.Empty();
    		if (iButton<pLines->GetHotButtonsInSet())
    		{
    			buttonLabel = pLines->GetHotButtonLabel(iButton);
    		}
    
    		// Update the button
            CWnd*  pWnd = GetDlgItem(IDC_HOT_BUTTON1+iButton);
    		ASSERT (pWnd!=NULL);
    		pWnd->ShowWindow(SW_NORMAL);
    		pWnd->SetWindowText(buttonLabel);
    	}
    }
    SILENT_RM_CATCH_HANDLER("CDirectLineConfigurePage::SetupHotButtons");
}


void CDirectLineConfigurePage::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Update the displayed buttons
	DisplayButtons();

	// Clear the label and Tetra ID text boxes
	SetDlgItemText(IDC_EDITLABEL, "");
	SetDlgItemText(IDC_EDITTETRAID, "");

	onUpdateRights();

	*pResult = 0;
}

void CDirectLineConfigurePage::OnSetButton() 
{
	// TODO: make this method update hot buttons (and not just direct line buttons).
    try
    {
    	CDirectLineCache*  pLines = CDirectLineCache::GetInstance();
    	CString			   tsiNumber;	
    	CString			   buttonLabel;
    
    
    	GetDlgItemText(IDC_EDITLABEL, buttonLabel);
    	GetDlgItemText(IDC_EDITTETRAID, tsiNumber);

		//wagndan++
  		int len = tsiNumber.GetLength();
  		BOOL bIsValid = TRUE;
  		for (int i = 0; i < len; i++)
  		{
  			char temp = tsiNumber.GetAt(i);
  			if (temp < '0' || temp > '9')
  			{
  				bIsValid = FALSE;
  				break;
  			}
  		}
  		if (!bIsValid)
  		{
  			/*TA_Base_Bus::TransActiveMessage userMsg;
  			userMsg << "10";
  			userMsg.showMsgBox(IDS_UE_620006);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			UINT errMsg = userMsg.showMsgBox(IDS_UE_620021);
			//RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
  			return;
  		}
  		//validate the input number
		//++wangdan




	    // Update the button
	    if (m_isHotButton)
	    {
		    pLines->SetHotButtonNumber(m_buttonNumber, std::string(tsiNumber));
		    pLines->SetHotButtonLabel(m_buttonNumber, std::string(buttonLabel));

            // Apply the changes (write them to the agent)
            pLines->ApplyHotButtonChanges(m_buttonNumber);

		    SetDlgItemText(m_buttonNumber + IDC_HOT_BUTTON1, buttonLabel);
	    }
	    else
	    {
		    // Determine which button set is in use
		    int iTab = m_TabControl.GetCurSel();
		    pLines->SetButtonNumber(iTab, m_buttonNumber, std::string(tsiNumber));
		    pLines->SetButtonLabel(iTab, m_buttonNumber, std::string(buttonLabel));

            // Apply the changes (write them to the agent)
            pLines->ApplyButtonChanges(iTab,m_buttonNumber);

		    SetDlgItemText(m_buttonNumber + IDC_DIRECT_LINE_BUTTON_1, buttonLabel);
	    }
    }
    catch(...)
    {
        // An error occurred during the apply phase (most likely the agent contact failed)
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString errMsg = userMsg.constructMessage(IDS_UW_632007);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            "An error occurred while configuring the button.\nPlease try again, and contact support if the problem persists",
            "Save button error",
            MB_OK | MB_ICONEXCLAMATION);*/
		RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            errMsg,
            "Save button error",
            MB_OK | MB_ICONEXCLAMATION);
		// ++ TD14164 
    }
	

	// Send a message to the direct lines page so that it can refresh
	AfxGetApp()->m_pMainWnd->PostMessage(REFRESH_DIALOG_MSG, 0, 0);
}

/**
  * OnDirectButtonPress
  *
  * This method is called when a direct button is pressed.  It will
  * identify which hot button was activated and then display that
  * buttons details.
  */
afx_msg void CDirectLineConfigurePage::OnDirectButtonPress(UINT ctrlID)
{
    try
    {
    	CDirectLineCache*  pLines = CDirectLineCache::GetInstance();
    	CString			   tsiNumber;
    	CString            buttonLabel;
    
    	m_buttonNumber = ctrlID - IDC_DIRECT_LINE_BUTTON_1;
    	m_isHotButton  = false;
    
    	// Determine which button set is in use
        int iTab = m_TabControl.GetCurSel();
    
    	// Get the TSI associated with the button
    	tsiNumber = pLines->GetButtonNumber(iTab, m_buttonNumber);
    
    	// Get the button label
    	GetDlgItemText(ctrlID, buttonLabel);
    
    	// Display the TSI and button label
    	SetDlgItemText(IDC_EDITLABEL, buttonLabel);
    	SetDlgItemText(IDC_EDITTETRAID, tsiNumber);

		onUpdateRights();
    }
    SILENT_RM_CATCH_HANDLER("CDirectLineConfigurePage::OnDirectButtonPress");
}

/**
  * OnHotButtonPress
  *
  * This method is called when a hot button is pressed.  It will
  * identify which hot button was activated and get the TSI
  * number associated with it.  The details will then be displayed
  * in the SelectedArea.
  */
afx_msg void CDirectLineConfigurePage::OnHotButtonPress(UINT ctrlID)
{

    try
    {
    	UINT               buttonNumber = ctrlID - IDC_HOT_BUTTON1;
    	CDirectLineCache*  pLines = CDirectLineCache::GetInstance();
    	CString			   tsiNumber;
    	CString            buttonLabel;
    
    	m_buttonNumber = buttonNumber;
    	m_isHotButton  = true;

    	// Get the TSI associated with the button
    	tsiNumber = pLines->GetHotButtonNumber(buttonNumber);
    
    	// Get the button label
    	GetDlgItemText(ctrlID,buttonLabel);
    
    	// Display the TSI and button label
    	SetDlgItemText(IDC_EDITLABEL, buttonLabel);
    	SetDlgItemText(IDC_EDITTETRAID, tsiNumber);

		CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
		
		GetDlgItem(IDC_SETBUTTON)->EnableWindow(manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_CONFIGURE_SHARED_DIRECT_LINE));

    }
    SILENT_RM_CATCH_HANDLER("CDirectLineConfigurePage::OnHotButtonPress");

}
