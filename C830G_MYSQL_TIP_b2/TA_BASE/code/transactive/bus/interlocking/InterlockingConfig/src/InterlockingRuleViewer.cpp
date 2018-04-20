/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/InterlockingRuleViewer.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
// InterlockingRuleViewer.cpp : implementation file
//

#include "stdafx.h"
#include "InterlockingRuleTriggerEditor.h"
#include "InterlockingRuleViewer.h"
#include "bus/interlocking/src/InterlockingRule.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // InterlockingRuleViewer dialog


    InterlockingRuleViewer::InterlockingRuleViewer(CWnd* pParent /*=NULL*/)
	    : CDialog(InterlockingRuleViewer::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(InterlockingRuleViewer)
	    m_name = _T("");
	    m_expression = _T("");
	    m_enabled = FALSE;
	    m_description = _T("");
	    m_precondition = -1;
	    //}}AFX_DATA_INIT

        m_entityName = _T("");
        m_entityKey = 0;
        m_entityType = _T("");
        m_entityAction = _T("");
        m_actionParameters.clear();
        m_aliases.clear();
    }


    void InterlockingRuleViewer::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(InterlockingRuleViewer)
	DDX_Control(pDX, IDC_RuleExpression, m_expressionEditor);
	    DDX_Text(pDX, IDC_RuleName, m_name);
	    DDV_MaxChars(pDX, m_name, 100);
	    DDX_Text(pDX, IDC_RuleExpression, m_expression);
	    DDV_MaxChars(pDX, m_expression, 1000);
	    DDX_Check(pDX, IDC_RuleEnabled, m_enabled);
	    DDX_Text(pDX, IDC_RuleDescription, m_description);
	    DDV_MaxChars(pDX, m_description, 500);
	    DDX_Radio(pDX, IDC_PreTrigger, m_precondition);
	//}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(InterlockingRuleViewer, CDialog)
	    //{{AFX_MSG_MAP(InterlockingRuleViewer)
	    ON_BN_CLICKED(IDC_Cancel, OnCancel)
	    ON_BN_CLICKED(IDC_Ok, OnOk)
	    ON_BN_CLICKED(IDC_OpenRuleEditor, OnOpenRuleEditor)
	    ON_BN_CLICKED(IDC_OpenTriggerEditor, OnOpenTriggerEditor)
	    ON_BN_CLICKED(IDC_Test, OnTest)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // InterlockingRuleViewer message handlers

    void InterlockingRuleViewer::OnCancel() 
    {
        CDialog::OnCancel();
    }

    void InterlockingRuleViewer::OnOk() 
    {
        CDialog::OnOK();
    }

    void InterlockingRuleViewer::OnOpenRuleEditor() 
    {
        //required but not used	    
    }

    void InterlockingRuleViewer::OnOpenTriggerEditor() 
    {
        // create main dialog screen
        InterlockingRuleTriggerEditor itriggerEditor;

        // copy rule information to trigger dialog
        itriggerEditor.setEntityName( std::string(m_entityName) );
        itriggerEditor.setEntityKey( m_entityKey );
        itriggerEditor.setEntityType( std::string(m_entityType) );
        itriggerEditor.setAction( std::string(m_entityAction) );

        // currently only support one parameter
        if ( m_actionParameters.empty() )
        {
            itriggerEditor.setParameter( std::string("") );
        }
        else
        {
            itriggerEditor.setParameter( m_actionParameters[0] );
        }


        // display trigger editor dialog (finally!)
        int ret = itriggerEditor.DoModal();
    
        // do we save or abandon changes
        if (ret == IDOK)
        {
            // user is happy with the changes - save them
            m_entityAction = itriggerEditor.getAction().c_str();

            // currently only support one parameter
            m_actionParameters.clear();
            m_actionParameters.push_back( itriggerEditor.getParameter() );

            //change rule name (FUTURE: unless set by user)
            m_name = (m_entityName + "." + m_entityAction + "." + m_actionParameters[0].c_str());
    //        m_nameDisplay.SetWindowText(m_name);
        }
	    
    }

    bool InterlockingRuleViewer::testExpression( bool showResult) 
    {
        InterlockingRule rule;
        rule.setAliases( m_aliases );
        try
        {
            CString expression;
            m_expressionEditor.GetWindowText( expression );
            // FUTURE: parse expression and replace aliases with variables
            rule.setCondition( std::string(expression) );
        }
        catch(...)
        {
            // Expression failed to parse. Please check for correct formatting.
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_110014);
            return false;
        }
        rule.loadContexts();
        try
        {
            bool result = rule.evaluate();
            if ( showResult == true )
            {
                if ( result == true )
                {
                    // Expression currently evaulates to true
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_110015);
                }
                else
                {
                    // Expression currently evaulates to false
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_110016);
                }
            }
            return true;
        }
        catch( TA_Base_Core::MathematicalEvaluationException e )
        {
            // Expression failed to evaulate. Error returned is:??
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_110017);
        }
        catch(...)
        {
            // Expression failed to evaulate
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_110018);
        }
        return false;
    }

    void InterlockingRuleViewer::OnTest() 
    {
        // test current expression and show results
        testExpression( true );
    }
}//namespace
