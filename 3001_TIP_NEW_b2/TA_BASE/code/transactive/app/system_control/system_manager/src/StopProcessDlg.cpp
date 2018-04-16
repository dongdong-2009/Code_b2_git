/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_manager/src/StopProcessDlg.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * StopProcessDlg is associated with the dialog box allowing 
  * operators to stop a running process.
  *
  */

#pragma warning(disable:4786)

#include <string>

#include "StdAfx.h"
#include "StopProcessDlg.h"
#include "SystemManagerDlg.h"
#include "AdminManager.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/OperatorAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/exceptions/src/SystemControllerException.h"


namespace TA_Base_App
{

    StopProcessDlg::StopProcessDlg(const std::vector<std::string>& processes, CWnd* parent)
        : CDialog(StopProcessDlg::IDD, parent), m_processes(processes)
    {
        //{{AFX_DATA_INIT(StopProcessDlg)
            // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
    }


    void StopProcessDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(StopProcessDlg)
		DDX_Control(pDX, IDC_STOP_PROCESS_LIST, m_processList);
        DDX_Control(pDX, IDOK, m_stopProcessButton);
		//}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(StopProcessDlg, CDialog)
        //{{AFX_MSG_MAP(StopProcessDlg)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL StopProcessDlg::OnInitDialog()
    {
        CDialog::OnInitDialog();

        // Set the label containing the processes to stop.
        std::string processList;
        for(std::vector<std::string>::iterator iter = m_processes.begin(); 
            iter != m_processes.end(); iter++)
        {
			m_processList.AddString((*iter).c_str());
        }

        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }

    void StopProcessDlg::OnOK()
    { 
        const unsigned long entityKey = AdminManager::getInstance().getEntityKey();
        std::string sessionId = AdminManager::getInstance().getSessionId();
        std::string currentUser = AdminManager::getInstance().getOperatorName();

        std::string process;
        for(std::vector<std::string>::iterator iter = m_processes.begin(); 
            iter != m_processes.end(); iter++)
        {
            process = *iter;

            //
            // Log an event.
            //

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Operator %s requested that process ''%s'' be stopped.",
                currentUser.c_str(), process.c_str());
           
            
            // Only send an audit message if in a valid session.
            if (AdminManager::getInstance().validSession())
            {
	            TA_Base_Core::AuditMessageSender* auditSender;
	            auditSender = 
	                TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::OperatorAudit::Context);
	
	            TA_Base_Core::DescriptionParameters desc;
	            TA_Base_Core::NameValuePair param1("ProcessId", process.c_str());
	            desc.push_back(&param1);
	
	            auditSender->sendAuditMessage(
	                TA_Base_Core::OperatorAudit::ApplicationStopRequest, // Message Type
	                entityKey,   // Entity key
	                desc,        // Description
	                "",          // Additional details
	                sessionId,   // SessionID if applicable
	                "",          // AlarmID if an alarm associated with event
	                "",          // IncidentKey if incident associated with event
	                "");         // EventID of a parent event, used to link events
	
				delete auditSender;
            }

            //
            // Stop the process.
            //

            try
            {
                AdminManager::getInstance().stopProcess(process);
            }
            catch(TA_Base_Core::SystemControllerException&)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << process;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_020021);
            }
        } 
  
        CDialog::OnOK(); 
    }

} // namespace TA_Base_App

