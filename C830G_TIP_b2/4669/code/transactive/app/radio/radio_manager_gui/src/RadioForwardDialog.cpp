/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioForwardDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/02/10 16:25:11 $
  * Last modified by:  $Author: builder $
  *
  */
// RadioForwardDialog.cpp : implementation file
//

#include "stdafx.h"
#include "RadioManager.h"
#include "RadioForwardDialog.h"
#include "RadioManagerDlg.h"

#include "core/utilities/src/RunParams.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioForwardDialog dialog


CRadioForwardDialog::CRadioForwardDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRadioForwardDialog::IDD, pParent), m_forwardEnabled(false), m_forwardAddress("")
{
	//{{AFX_DATA_INIT(CRadioForwardDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRadioForwardDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioForwardDialog)
	DDX_Control(pDX, IDC_TSI_COMBO, m_TSICombo);
	DDX_Control(pDX, IDC_ID_COMBO, m_IDCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRadioForwardDialog, CDialog)
	//{{AFX_MSG_MAP(CRadioForwardDialog)
	ON_BN_CLICKED(IDOK, onOK)
	ON_BN_CLICKED(IDC_RADIO1, onRadio1)
	ON_BN_CLICKED(IDC_RADIO2, onRadio2)
	ON_BN_CLICKED(IDC_RADIO3, onRadio3)
	ON_CBN_SELCHANGE(IDC_TSI_COMBO, onSelchangeTsiCombo)
	ON_CBN_EDITCHANGE(IDC_TSI_COMBO, onEditchangeTsiCombo)
	ON_CBN_EDITCHANGE(IDC_ID_COMBO, onEditchangeIdCombo)
	ON_CBN_SELCHANGE(IDC_ID_COMBO, onSelchangeIdCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioForwardDialog message handlers

BOOL CRadioForwardDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // Get the parent class and retrieve the reference to the radio manager
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(GetParent());

    TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails_var details;
    try
    {
        CORBA_CALL_RETURN( details, manager->getRadioSessionCorbaDef(), getCallForwarding, () );//limin
    }
    catch(CORBA::Exception& ex)
    {
        std::string reason = "Exception thrown while attempting to retrieve forwarding details: " +
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
        LOG_EXCEPTION_CATCH(SourceInfo,"CORBA::Exception",reason.c_str());

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "retrieving the forwarding details from";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632014);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            "An error occurred while retrieving the forwarding details from the Radio Agent",
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);*/
		RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            errMsg,
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);
		// ++ TD14164 

		EndDialog(IDCANCEL);
        return FALSE;
    }
    catch(...)
    {
        std::string reason = "Exception thrown while attempting to retrieve forwarding details";
        LOG_EXCEPTION_CATCH(SourceInfo,"Unknown",reason.c_str());

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "retrieving the forwarding details from";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632014);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            "An error occurred while retrieving the forwarding details from the Radio Agent",
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);*/
		RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            errMsg,
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);
		// ++ TD14164 

		EndDialog(IDCANCEL);
        return FALSE;
    }

	// Load the subscribers
	loadSubscriberList();

    if (details->forwardAction != TA_Base_Bus::IRadioSessionCorbaDef::NeverForward)
    {
        // If it's not "NeverForward", then forwarding is enabled
        m_forwardEnabled = true;
        if (details->forwardAction == TA_Base_Bus::IRadioSessionCorbaDef::ForwardAlways)
        {
            onRadio1();
        }
        else
        {
            onRadio2();
        }

        m_forwardAddress = details->destinationTSI;
        if (m_TSICombo.FindStringExact(0,m_forwardAddress)==CB_ERR)
		{
			// Not in list so add it!
			m_TSICombo.AddString(m_forwardAddress);
		}
        m_TSICombo.SelectString(0,m_forwardAddress);
		onEditchangeTsiCombo();
    }
    else
    {
        // Forwarding is disabled - check the "NeverForward" button.
        onRadio3();
    }


	// Check rights to see if we are able to update forwarding
	bool enable = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_CONFIGURE_CALL_FORWARD);
	if (!enable)
	{
		m_TSICombo.EnableWindow(enable);
		m_IDCombo.EnableWindow(enable);
		GetDlgItem(IDC_RADIO1)->EnableWindow(false);
		GetDlgItem(IDC_RADIO2)->EnableWindow(false);
		GetDlgItem(IDC_RADIO3)->EnableWindow(false);
	    GetDlgItem(IDOK)->EnableWindow(false);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRadioForwardDialog::onOK() 
{
	// Perform the forward setting

    // Is forwarding enabled? If so, the "Never Forward" radio button is NOT selected
    bool currentlyForwarded = m_forwardEnabled;
	m_forwardEnabled = (!IsDlgButtonChecked(IDC_RADIO3));

    // Get the paraent class and retrieve the reference to the radio manager
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(GetParent());

    // And create a details object.
    TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails details;

    // So, if forwarding HAS been enabled...
    if (m_forwardEnabled)
    {
        // Retrieve the selected address to forward TO
        m_TSICombo.GetWindowText(m_forwardAddress);

        // And set it
        details.destinationTSI = m_forwardAddress;

        // Then set the forward action. As at this point we know that "Never Forward" was NOT clicked, that only 
        // leaves two options.
        details.forwardAction = this->IsDlgButtonChecked(IDC_RADIO1) ? TA_Base_Bus::IRadioSessionCorbaDef::ForwardAlways 
            : TA_Base_Bus::IRadioSessionCorbaDef::ForwardNotReachable;

        try
        {
            CORBA_CALL( manager->getRadioSessionCorbaDef(), setCallForwarding, ( details,
                    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
        }
        catch (...)
        {
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName = "sending the forwarding settings to";
			userMsg << actionName;
			CString errMsg = userMsg.constructMessage(IDS_UW_632014);
            /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
                "An error occurred while sending the forwarding settings to the Radio Agent.",
                "Radio Manager",
                MB_OK | MB_ICONEXCLAMATION);*/
			RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
                errMsg,
                "Radio Manager",
                MB_OK | MB_ICONEXCLAMATION);
			// ++ TD14164 
        }
    }
    else if (currentlyForwarded)    // Disable the forwarding
    {
        // Set the address to null (it's not used)
        details.destinationTSI = std::string("").c_str();

        // Then set the forward action. As at this point we know that "Never Forward" was NOT clicked, that only 
        // leaves two options.
        details.forwardAction = TA_Base_Bus::IRadioSessionCorbaDef::NeverForward;

        try
        {
            CORBA_CALL( manager->getRadioSessionCorbaDef(), setCallForwarding, ( details,
                        TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
        }
        catch (...)
        {
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName = "sending the forwarding settings to";
			userMsg << actionName;
			CString errMsg = userMsg.constructMessage(IDS_UW_632014);
            /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
                "An error occurred while sending the forwarding settings to the Radio Agent.",
                "Radio Manager",
                MB_OK | MB_ICONEXCLAMATION);*/
            RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
                errMsg,
                "Radio Manager",
                MB_OK | MB_ICONEXCLAMATION);
			// ++ TD14164 
        }
    }

	EndDialog(IDOK);
}

void CRadioForwardDialog::onRadio1() 
{
	this->CheckRadioButton(IDC_RADIO1,IDC_RADIO3,IDC_RADIO1);
	m_TSICombo.EnableWindow(true);
	m_IDCombo.EnableWindow(true);
	EnableOKButton();
}

void CRadioForwardDialog::onRadio2() 
{
	this->CheckRadioButton(IDC_RADIO1,IDC_RADIO3,IDC_RADIO2);
	m_TSICombo.EnableWindow(true);
	m_IDCombo.EnableWindow(true);
	EnableOKButton();
}

void CRadioForwardDialog::onRadio3() 
{
	this->CheckRadioButton(IDC_RADIO1,IDC_RADIO3,IDC_RADIO3);
	//TD17305
	m_TSICombo.SetWindowText("");
	m_IDCombo.SetWindowText("");
	//TD17305
	m_TSICombo.EnableWindow(false);
	m_IDCombo.EnableWindow(false);
	EnableOKButton();
}


void CRadioForwardDialog::onSelchangeTsiCombo() 
{
	char    selectedTSI[50]="";
	m_TSICombo.GetLBText(m_TSICombo.GetCurSel(),selectedTSI);
	SynchroniseIDwithTSI(selectedTSI);
	EnableOKButton();
}

void CRadioForwardDialog::onEditchangeTsiCombo() 
{
	char    selectedTSI[50];
	m_TSICombo.GetWindowText(selectedTSI,sizeof(selectedTSI));
	SynchroniseIDwithTSI(selectedTSI);
	EnableOKButton();
}


void CRadioForwardDialog::onSelchangeIdCombo() 
{
	char    selectedID[50]="";
	m_IDCombo.GetLBText(m_IDCombo.GetCurSel(),selectedID);
	SynchroniseTSIwithID(selectedID);	
	EnableOKButton();
}

void CRadioForwardDialog::onEditchangeIdCombo() 
{
	char    selectedID[50];
	m_IDCombo.GetWindowText(selectedID,sizeof(selectedID));
	SynchroniseTSIwithID(selectedID);
	EnableOKButton();
}


//*******************
// Helper Functions *
//*******************


void CRadioForwardDialog::EnableOKButton()
{
	bool   isEnabled = true;

	if (!IsDlgButtonChecked(IDC_RADIO3))
	{
		// See if any TSI is entered
		CString selectedTSI;
		m_TSICombo.GetWindowText(selectedTSI);
		isEnabled = !selectedTSI.IsEmpty();

		// if an item is selected GetWindowText seems to return nothing for some reason
		// the stupid thing is in onSelchangeTsiCombo it returns the actual string
		if (!isEnabled && (m_TSICombo.GetCurSel() > -1) )
		{
			m_TSICombo.GetLBText(m_TSICombo.GetCurSel(), selectedTSI);
			isEnabled = !selectedTSI.IsEmpty();
		}
	}

	// Set the button
	CWnd* pWnd = GetDlgItem(IDOK);
	TA_ASSERT(pWnd!=NULL,"OK button not found.");
	pWnd->EnableWindow(isEnabled);
}

void CRadioForwardDialog::loadSubscriberList() 
{
    // Get the parent class and retrieve the reference to the readio manager
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());  
    TA_Base_Bus::IRadioCorbaDef::SubscriberList_var subscribers; 
    //TA_Base_Bus::IRadioDirectoryCorbaDef::SubscriberList_var groups; 
    try
    {
        CORBA_CALL_RETURN( subscribers, manager->getRadioCorbaDef(), getIndividualSubscribers, () );//limin
		// Add all the subscriber's to the vector
		unsigned long iLoop;
		for (iLoop=0;iLoop<subscribers->length();iLoop++)
		{
			m_subscribers.push_back(subscriberEntry(std::string(subscribers[iLoop].subscriberName),
				                    std::string(subscribers[iLoop].TSI)));

			// Add to the combo drop down lists
			m_TSICombo.AddString(subscribers[iLoop].TSI);
			m_IDCombo.AddString(subscribers[iLoop].subscriberName);
		}
    }
    catch(CORBA::Exception& ex)
    {
        std::string reason = "Exception thrown while attempting to load subscriber list: " +
            TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
        LOG_EXCEPTION_CATCH(SourceInfo,"CORBA::Exception",reason.c_str());

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString description = "loading subscribers";
		userMsg << description;
		CString errMsg = userMsg.constructMessage(IDS_UW_632015);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            "An error occurred while loading subscribers",
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);*/
		RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            errMsg,
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);
		// ++ TD14164 
    }
    catch(...)
    {
        std::string reason = "Exception thrown while attempting to to load subscriber list";
        LOG_EXCEPTION_CATCH(SourceInfo,"Unknown",reason.c_str());

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString description = "loading subscribers";
		userMsg << description;
		CString errMsg = userMsg.constructMessage(IDS_UW_632015);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            "An error occurred while loading subscribers",
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);*/
        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            errMsg,
            "Radio Manager", 
            MB_OK | MB_ICONEXCLAMATION);
		// ++ TD14164 
    }

}


void CRadioForwardDialog::SynchroniseIDwithTSI(const char* selectedTSI)
{
	// Find the associated ID
	std::vector<subscriberEntry >::iterator  it;
	for (it=m_subscribers.begin();it!=m_subscribers.end();it++)
	{
		// Set associated ID
		if (strcmp(it->second.c_str(),selectedTSI)==0)
		{
			m_IDCombo.SelectString(0,it->first.c_str());
			return;
		}
	}

	// If we get here then we did not find a match
	m_IDCombo.SetWindowText("");
}

void CRadioForwardDialog::SynchroniseTSIwithID(const char* selectedID)
{
	// Find the associated TSI
	std::vector<subscriberEntry >::iterator  it;
	for (it=m_subscribers.begin();it!=m_subscribers.end();it++)
	{
		// Set associated TSI
		if (strcmp(it->first.c_str(),selectedID)==0)
		{
			m_TSICombo.SelectString(0,it->second.c_str());
			return;
		}
	}

	// If we get here then we did not find a match
	m_TSICombo.SetWindowText("");
}


