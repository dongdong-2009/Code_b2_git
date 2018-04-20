/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/InterlockingRuleTriggerEditor.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
// InterlockingRuleTriggerEditor.cpp : implementation file
//

#include "stdafx.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/interlocking_entity/src/IInterlockEntity.h"
#include "core/data_access_interface/interlocking_entity/src/InterlockEntityAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataBaseException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "IEntityAction.h"
#include "EntityActionFactory.h"
#include "InterlockingRuleTriggerEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // InterlockingRuleTriggerEditor dialog


    InterlockingRuleTriggerEditor::InterlockingRuleTriggerEditor(CWnd* pParent /*=NULL*/)
	    : CDialog(InterlockingRuleTriggerEditor::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(InterlockingRuleTriggerEditor)
	    m_action = _T("");
	    m_entityName = _T("");
	    m_parameter = _T("");
        m_entityType = _T("");
        m_entityKey = 0;
        m_ientityAction = NULL;
	    //}}AFX_DATA_INIT

        m_entityType = _T("");
        m_entityKey = 0;
        m_ientityAction = NULL;
    }


    void InterlockingRuleTriggerEditor::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY("DoDataExchange");

	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(InterlockingRuleTriggerEditor)
	    DDX_Text(pDX, IDC_EntityName, m_entityName);
	    DDX_Control(pDX, IDC_Action, m_actions);
	    DDX_CBString(pDX, IDC_Action, m_action);
	    DDV_MaxChars(pDX, m_action, 80);
    	DDX_Control(pDX, IDC_Parameters, m_parameters);
	    DDX_CBString(pDX, IDC_Parameters, m_parameter);
	    DDV_MaxChars(pDX, m_parameter, 80);
	//}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(InterlockingRuleTriggerEditor, CDialog)
	    //{{AFX_MSG_MAP(InterlockingRuleTriggerEditor)
	    ON_CBN_SELCHANGE(IDC_Action, OnSelchangeAction)
	    ON_CBN_SELCHANGE(IDC_Parameters, OnSelchangeParameters)
	    ON_BN_CLICKED(IDC_Cancel, OnCancel)
	    ON_BN_CLICKED(IDC_Ok, OnOk)
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // InterlockingRuleTriggerEditor message handlers

    void InterlockingRuleTriggerEditor::OnSelchangeAction() 
    {
        FUNCTION_ENTRY("OnSelchangeAction");

        // get new selection
        int selIndex = m_actions.GetCurSel();
        if (selIndex == LB_ERR)
        {
            return;
        }
        m_actions.GetLBText(selIndex, m_action);

        // get parameters for this action and store in combobox
        std::vector<std::string> parameters = m_ientityAction->getParameters(std::string(m_action));
        loadParameterList(parameters);

        // select current parameter
        m_parameters.SelectString(0, m_parameter);

		FUNCTION_EXIT;
    }

    void InterlockingRuleTriggerEditor::OnSelchangeParameters() 
    {
        FUNCTION_ENTRY("OnSelchangeAction");

		FUNCTION_EXIT;
	}

    void InterlockingRuleTriggerEditor::loadActionList (const std::vector<std::string>& actions)
    {
        FUNCTION_ENTRY("loadActionList");

        // remove any items in the list
        if (m_actions.GetCount() > 0 )
        {
            while (m_actions.DeleteString(0));
        }

        // add new items
        for (std::vector<std::string>::const_iterator it = actions.begin(); it != actions.end(); it++)
        {
            m_actions.AddString((*it).c_str());
        }
        
		FUNCTION_EXIT;
    }

    void InterlockingRuleTriggerEditor::loadParameterList (const std::vector<std::string>& parameters)
    {
        FUNCTION_ENTRY("loadParameterList");

        // remove any items in the list
        m_parameters.ResetContent();

        // add new items
        for (std::vector<std::string>::const_iterator it = parameters.begin(); it != parameters.end(); it++)
        {
            m_parameters.AddString((*it).c_str());
        }
		
		FUNCTION_EXIT;
    }

    BOOL InterlockingRuleTriggerEditor::OnInitDialog() 
    {
        FUNCTION_ENTRY("OnInitDialog");

	    CDialog::OnInitDialog();
	    
        // get dll info from db
        TA_Base_Core::IInterlockEntity* entity = NULL;
        try
        {
            entity = TA_Base_Core::InterlockEntityAccessFactory::getInstance().getInterlockEntity(std::string(m_entityType));
        }
        catch(TA_Base_Core::DataException& e)
        {
            if (e.getFailType() != TA_Base_Core::DataException::NO_VALUE)
            {
                std::string msg = "No records found for entity type" + std::string(m_entityType);
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", msg.c_str() );
                throw e;
            }
            std::string msg = e.what();
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", msg.c_str() );
            return -1; // don't know what else to do here??
        }
        catch(TA_Base_Core::DatabaseException& e)
        {
            std::string msg = e.what();
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataBaseException", msg.c_str() );
            return -1;
        }

        TA_ASSERT(entity != NULL, "Entity access factory broken!!");

        std::string dllName = entity->getDllName();
        delete(entity);

        TA_ASSERT(!dllName.empty(), "Entity Type not configured in DB");

        // open dll
        EntityActionFactory actionFactory;
        m_ientityAction = actionFactory.createEntityAction(dllName);
        TA_ASSERT(m_ientityAction != NULL, "Action factory broken!!");

        // get action info from dll and store in combobox
        std::vector<std::string> actions = m_ientityAction->getActions(m_entityKey);
        TA_ASSERT(!actions.empty(), "Action list can not be empty!");
        loadActionList(actions);

        // select current action
        m_actions.SelectString(0, m_action);

        // get possible parameters for this action and store in combobox
        std::vector<std::string> parameters = m_ientityAction->getParameters(std::string(m_action));
        loadParameterList(parameters);
	    
        // select current parameter
        m_parameters.SelectString(0, m_parameter);

		FUNCTION_EXIT;
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    void InterlockingRuleTriggerEditor::OnCancel() 
    {
	    // TODO: Add your control notification handler code here
        CDialog::OnCancel();
    }

    void InterlockingRuleTriggerEditor::OnOk() 
    {
	    // TODO: Add your control notification handler code here
		this->UpdateData(true);

		if (m_action.IsEmpty() || m_parameter.IsEmpty() )
		{
			// display a message to ask operator to provide action and parameter input
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_610005);

			return;
		}
		
		CDialog::OnOK();
    }

}// Namespace
