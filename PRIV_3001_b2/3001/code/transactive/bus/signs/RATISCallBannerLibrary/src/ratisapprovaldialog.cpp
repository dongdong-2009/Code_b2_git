/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/signs/RATISCallBannerLibrary/src/ratisapprovaldialog.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class is used to display a dialog that allows the
  * the user to approve or reject RATIS messages.
  *
  */

#include "bus/banner_pages/call_page_library/src/stdafx.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 
#include "RatisCallEntry.h"
#include "RatisApprovalDialog.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_Bus
{

    BEGIN_MESSAGE_MAP(RatisApprovalDialog, CDialog)
	//{{AFX_MSG_MAP(RatisApprovalDialog)
	ON_BN_CLICKED(IDC_APPROVE, OnApprove)
	ON_BN_CLICKED(IDC_REJECT, OnReject)
	ON_EN_UPDATE(IDC_PRIORITY, OnUpdatePriority)
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    RatisApprovalDialog::RatisApprovalDialog(RATISCallEntry* callEntry, CWnd* pParent /* = NULL */)
	    : CDialog(RatisApprovalDialog::IDD, pParent),
        m_callEntry(callEntry)
    {
        TA_ASSERT(callEntry != NULL, "Call Entry must not be NULL");

	    //{{AFX_DATA_INIT(RatisApprovalDialog)
	m_priority = 0;
	//}}AFX_DATA_INIT

        Create(RatisApprovalDialog::IDD, pParent);
    }


    void RatisApprovalDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(RatisApprovalDialog)
	DDX_Control(pDX, IDC_EMERGENCY, m_emergencyRadio);
	DDX_Control(pDX, IDC_NORMAL, m_normalRadio);
	DDX_Control(pDX, IDC_PRIORITY6, m_priority6Radio);
	DDX_Control(pDX, IDC_EDIT3, m_destinationCtrl);
	DDX_Control(pDX, IDC_PRIORITY, m_priorityCtrl);
	DDX_Control(pDX, IDC_EDIT5, m_endTimeCtrl);
	DDX_Control(pDX, IDC_EDIT4, m_startTimeCtrl);
	DDX_Control(pDX, IDC_TYPE, m_typeCtrl);
	DDX_Control(pDX, IDC_TAG, m_ratisTagCtrl);
	DDX_Control(pDX, IDC_MESSAGE_CONTENT, m_contentCtrl);
	DDX_Text(pDX, IDC_PRIORITY, m_priority);
	DDV_MinMaxUInt(pDX, m_priority, 1, 8);
	//}}AFX_DATA_MAP
    }


    void RatisApprovalDialog::setCallEntry(RATISCallEntry* callEntry)
    {
        TA_ASSERT(callEntry != NULL, "Call entry must not be NULL");

        m_callEntry = callEntry;

        refreshFields();
    }


    RATISCallEntry& RatisApprovalDialog::getCallEntry()
    {
        return *m_callEntry;
    }


    BOOL RatisApprovalDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
        refreshFields();

        CenterWindow();
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void RatisApprovalDialog::refreshFields()
    {
        // Update (most of) the fields on the dialog
        m_contentCtrl.SetWindowText( m_callEntry->getInformation().c_str() );
        m_contentCtrl.SetLimitText( m_callEntry->getMessageContentMaximumLength() );
        m_ratisTagCtrl.SetWindowText( m_callEntry->getRATISTag().c_str() );
        m_typeCtrl.SetWindowText( m_callEntry->getRATISTypeAsString().c_str() );
        m_startTimeCtrl.SetWindowText( m_callEntry->getStartTime().c_str() );
        m_endTimeCtrl.SetWindowText( m_callEntry->getEndTime().c_str() );
        m_destinationCtrl.SetWindowText( m_callEntry->getDestination().c_str() );

        // Update the priority
        std::ostringstream priorityStr;
        priorityStr << m_callEntry->getPriority();
        m_priorityCtrl.SetWindowText( priorityStr.str().c_str() );
        m_priorityCtrl.SetLimitText(1); // for entering 1 - 8

        bool isClearRequest = m_callEntry->isClearRequest(); 
        // Enable the content and priority text fields if the call is not a clear request overridable
        bool isOverridable = !isClearRequest && m_callEntry->isOverridable();
        if (isOverridable)
        {
            m_contentCtrl.SetReadOnly(FALSE);
            m_priorityCtrl.SetReadOnly(FALSE);
            m_contentCtrl.EnableWindow(TRUE);
            m_priorityCtrl.EnableWindow(TRUE);
        }
        else
        {
            m_contentCtrl.SetReadOnly(TRUE);
            m_priorityCtrl.SetReadOnly(TRUE);
        }
        //
        // Enable or disable the clear type radio buttons, and initialise them
        // if necessary.
        // Enable them only when the message is clear and destined for TTIS 
        //
        // TD17573
		// As required, only when the message is overridable, then enable the buttons
		// TD17572
		// As required, when clear ratis to station, also should enable the buttons
		BOOL isClearType = isClearRequest && m_callEntry->isOverridable();
        // TD17573
        m_emergencyRadio.EnableWindow(isClearType);
        m_normalRadio.EnableWindow(isClearType);
        m_priority6Radio.EnableWindow(isClearType);

        if (isClearType)
        {
            int priority = atoi( priorityStr.str().c_str() );
            RATISCallEntry::EPriorityGroup pgroup = m_callEntry->getPriorityGroup(priority);
            m_emergencyRadio.SetCheck(pgroup == RATISCallEntry::EMERGENCY);
            m_normalRadio.SetCheck(pgroup == RATISCallEntry::NORMAL);
            m_priority6Radio.SetCheck(pgroup == RATISCallEntry::PRIORITY6);
        }
		// TD17572
    }


    void RatisApprovalDialog::OnApprove() 
    {
        // if it is a TTIS clear request, then get the priority from the radio dials
        if ((m_callEntry->getRATISType() == TA_Base_Core::RATIS_IN_CLEAR || m_callEntry->getRATISType() == TA_Base_Core::RATIS_OUT_CLEAR) && (m_callEntry->isTTIS()))
        {
            // priority 1 - 3 is emergency; 6 is priority 6; everything else is normal, so we'll 
            // just set the priority to 1 for emergency; 4 for normal; and 6 for priority 6.
            int priority = 4;
            if (m_emergencyRadio.GetCheck() == BST_CHECKED)
            {
                priority = 1;
            }
            else if (m_priority6Radio.GetCheck() == BST_CHECKED)
            {
                priority = 6;
            }

	        m_callEntry->submitRATISVettingResponse(true, priority);
            OnCancel();
            return;
        }

        // if it is not overridable, then just submit approval
        if (!m_callEntry->isOverridable())
        {
	        m_callEntry->submitRATISVettingResponse(true);
            OnCancel();
            return;
        }

        CString priorityStr, content;
        m_priorityCtrl.GetWindowText(priorityStr);
        int priority = atol((LPCTSTR)priorityStr);
        m_contentCtrl.GetWindowText(content);
	    m_callEntry->submitRATISVettingResponse(true, priority, content);
        OnCancel();
        return;
        OnCancel();
    }


    void RatisApprovalDialog::OnReject() 
    {
   	    m_callEntry->submitRATISVettingResponse(false);    
        OnCancel();
    }


    void RatisApprovalDialog::showErrorMessage(const std::string& message)
    {
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString reason = message.c_str();
		userMsg << reason;
		CString errMsg = userMsg.constructMessage(IDS_UE_020071);
        /*MessageBox(message.c_str(),
                   TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
                   MB_OK|MB_ICONSTOP);*/
		MessageBox(errMsg,
                   TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
                   MB_OK|MB_ICONSTOP);
		// ++ TD14164
    }

    void RatisApprovalDialog::OnUpdatePriority() 
    {
        CString priorityStr;
        m_priorityCtrl.GetWindowText(priorityStr);
        int priority = atol((LPCTSTR)priorityStr);
        if (priority < 1)
        {
            m_priorityCtrl.SetWindowText("1");
        }
        else if (priority > 8)
        {
            m_priorityCtrl.SetWindowText("8");
        }
    }
} // namespace TA_IRS_Bus


