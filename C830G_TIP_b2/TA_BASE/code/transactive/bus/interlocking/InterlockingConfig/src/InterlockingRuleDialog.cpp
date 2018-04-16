/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/InterlockingRuleDialog.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
// InterlockingRuleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/interlocking_variable/src/IInterlockVariable.h"
#include "core/data_access_interface/interlocking_variable/src/InterlockVariableAccessFactory.h"
#include "bus/interlocking/src/InterlockingRule.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "IAddVariable.h"
#include "VariableTypeFactory.h"
#include "InterlockingRuleTriggerEditor.h"
#include "InterlockingRuleDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{

    const int InterlockingRuleDialog::ALIAS_COLUMN    = 0;
    const int InterlockingRuleDialog::VARIABLE_COLUMN = 1;
    const int InterlockingRuleDialog::VARIABLE_LIST_COLUMNS = 2;

    const CString InterlockingRuleDialog::VARIABLE_COLUMN_NAME = "Variable";
    const CString InterlockingRuleDialog::ALIAS_COLUMN_NAME    = "Name";

    /////////////////////////////////////////////////////////////////////////////
    // InterlockingRuleDialog dialog


    InterlockingRuleDialog::InterlockingRuleDialog(CWnd* pParent /*=NULL*/)
	    : CDialog(InterlockingRuleDialog::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(InterlockingRuleDialog)
	    m_name = _T("");
	    m_expression = _T("");
	    m_enabled = FALSE;
	    m_description = _T("");
	    m_precondition = -1;
		m_ivariable = NULL;
	//}}AFX_DATA_INIT

    }


    void InterlockingRuleDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(InterlockingRuleDialog)
	DDX_Control(pDX, IDC_Test, m_testButton);
	DDX_Control(pDX, IDC_Ok, m_okButton);
    	DDX_Control(pDX, IDC_RuleExpression, m_expressionEditor);
	    DDX_Control(pDX, IDC_VariableBtn8, m_variableBtn8);
	    DDX_Control(pDX, IDC_VariableBtn7, m_variableBtn7);
	    DDX_Control(pDX, IDC_VariableBtn6, m_variableBtn6);
	    DDX_Control(pDX, IDC_VariableBtn5, m_variableBtn5);
	    DDX_Control(pDX, IDC_VariableBtn4, m_variableBtn4);
	    DDX_Control(pDX, IDC_VariableBtn3, m_variableBtn3);
	    DDX_Control(pDX, IDC_VariableBtn2, m_variableBtn2);
	    DDX_Control(pDX, IDC_VariableBtn1, m_variableBtn1);
	    DDX_Control(pDX, IDC_VariableList, m_variableList);
	    DDX_Control(pDX, IDC_OperatorList, m_operatorList);
	    DDX_Control(pDX, IDC_RuleName, m_nameDisplay);
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


    BEGIN_MESSAGE_MAP(InterlockingRuleDialog, CDialog)
	    //{{AFX_MSG_MAP(InterlockingRuleDialog)
	    ON_BN_CLICKED(IDC_Ok, OnOk)
	    ON_BN_CLICKED(IDC_OpenTriggerEditor, OnOpenTriggerEditor)
	    ON_BN_CLICKED(IDC_Test, OnTest)
	    ON_LBN_DBLCLK(IDC_OperatorList, OnDblclkOperatorList)
	    ON_NOTIFY(NM_DBLCLK, IDC_VariableList, OnDblclkVariableList)
	    ON_BN_CLICKED(IDC_VariableBtn1, OnVariableBtn1)
	    ON_BN_CLICKED(IDC_VariableBtn2, OnVariableBtn2)
	    ON_BN_CLICKED(IDC_VariableBtn3, OnVariableBtn3)
	    ON_BN_CLICKED(IDC_VariableBtn4, OnVariableBtn4)
	    ON_BN_CLICKED(IDC_VariableBtn5, OnVariableBtn5)
	    ON_BN_CLICKED(IDC_VariableBtn6, OnVariableBtn6)
	    ON_BN_CLICKED(IDC_VariableBtn7, OnVariableBtn7)
	    ON_BN_CLICKED(IDC_VariableBtn8, OnVariableBtn8)
		ON_EN_KILLFOCUS(IDC_RuleExpression, OnKillfocusRuleExpression)
	    ON_BN_CLICKED(IDC_Cancel, OnCancel)
	ON_EN_CHANGE(IDC_RuleExpression, OnChangeRuleExpression)
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // InterlockingRuleDialog message handlers

    void InterlockingRuleDialog::OnCancel() 
    {
        destroyVariableDLL();
		CDialog::OnCancel();
    }

    void InterlockingRuleDialog::OnOk() 
    {
		if (isExpressionPresent() == true)
		{
			// run test. do not show current value. 
			if ( testExpression( false ) == true )
			{
				destroyVariableDLL();
				CDialog::OnOK();
			}
		}

    }
	
	void InterlockingRuleDialog::destroyVariableDLL()
	{
		if (m_ivariable != NULL)
		{
			m_ivariable->destroySelector();
            FreeLibrary( m_ivariable->libraryInstance );
			m_ivariable->libraryInstance=0;
			m_ivariable=NULL;
		}
	}

    void InterlockingRuleDialog::OnOpenTriggerEditor() 
    {
        // create main dialog screen
        InterlockingRuleTriggerEditor itriggerEditor;

        // copy rule information to trigger dialog
        itriggerEditor.setEntityName( std::string( m_entityName ));
        itriggerEditor.setEntityKey( m_entityKey );
        itriggerEditor.setEntityType( std::string( m_entityType ));
        itriggerEditor.setAction( std::string( m_entityAction ));

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
            m_nameDisplay.SetWindowText(m_name);
        }
    }

	void InterlockingRuleDialog::OnKillfocusRuleExpression() 
	{
		// Check that the expression text is not empty or a space
		// Enable the ok and test buttons if not TD 11399
		CString expression;
        m_expressionEditor.GetWindowText( expression );

		if ( expression.IsEmpty() == false)
		{
			m_okButton.EnableWindow(true);
			m_testButton.EnableWindow(true);
		}
		else
		{
			m_okButton.EnableWindow(false);
			m_testButton.EnableWindow(false);
		}
	}

	void InterlockingRuleDialog::OnChangeRuleExpression() 
	{		
		// Check for empty string and diable the ok & test buttons if so
		// TD 11399
		CString expression;
        m_expressionEditor.GetWindowText( expression );

		if ( expression.IsEmpty() == false)
		{
			m_okButton.EnableWindow(true);
			m_testButton.EnableWindow(true);
		}
		else
		{
			m_okButton.EnableWindow(false);
			m_testButton.EnableWindow(false);
		}		
	}

    bool InterlockingRuleDialog::isExpressionPresent( ) 
    {
		bool isOperatorPresent = false;
		CString expression;
        m_expressionEditor.GetWindowText( expression );

		if (expression.IsEmpty() == false)
		{
			// Parse the expression for valid operator(s)
			// If not the expressin is either not valid or just empty space
			for (int i = 0; i<m_operatorList.GetCount(); ++i)
			{
				CString opString;
				m_operatorList.GetText(i, opString);
				if (opString.IsEmpty() == false)
				{
					if (expression.Find(opString)>-1)
					{
						isOperatorPresent = true;
						break;
					}
				}
			}
		}

		if (isOperatorPresent == false)
		{
			// this is a case of empty expression or invalid expression
			// display message to request user to provide expression TD 11399
			CString erString("Invalid or empty expression input");
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << erString;
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_110017);

			// disable the ok and test buttons again as there is no expression
			m_okButton.EnableWindow(false);
			m_testButton.EnableWindow(false);
		}

		return isOperatorPresent;
	}

    bool InterlockingRuleDialog::testExpression( bool showResult) 
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

			// Disable the ok and test buttons as expression is not valid TD 11790
			m_okButton.EnableWindow(false);
			m_testButton.EnableWindow(false);
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

			// Disable the ok and test buttons as expression is not valid TD 11790
			m_okButton.EnableWindow(false);
			m_testButton.EnableWindow(false);
        }
/*        catch(...)
        {
            // Expression failed to evaulate
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_110018);
        }
*/        return false;
    }

    void InterlockingRuleDialog::OnTest() 
    {
        // test current expression and show results
		if ( isExpressionPresent() == true )
		{
			testExpression( true );
		}
    }

    void InterlockingRuleDialog::OnDblclkOperatorList() 
    {
	    int selection = m_operatorList.GetCurSel();
        CString opr;
        // longest operater is 'FALSE' so double it
        m_operatorList.GetText( selection, opr.GetBuffer( 10 ) );
        opr.ReleaseBuffer();
        
        addOperatorToExpression( std::string( opr ) );
    }


    BOOL InterlockingRuleDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
        // Fill the operator list picker
        m_operatorList.AddString("(");
        m_operatorList.AddString(")");
        m_operatorList.AddString("-");
        m_operatorList.AddString("+");
        m_operatorList.AddString("*");
        m_operatorList.AddString("/");
        m_operatorList.AddString("^");
        m_operatorList.AddString("<");
        m_operatorList.AddString(">");
        m_operatorList.AddString("=");
        m_operatorList.AddString("<=");
        m_operatorList.AddString(">=");
        m_operatorList.AddString("!=");
        m_operatorList.AddString("OR");
        m_operatorList.AddString("AND");
        m_operatorList.AddString("XOR");
        m_operatorList.AddString("NOT");
        m_operatorList.AddString("TRUE");
        m_operatorList.AddString("FALSE");
        m_operatorList.AddString("SQRT");

        // format and fill the variable list picker
        m_variableList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_SINGLESEL );
        RECT variableListSize;
        m_variableList.GetClientRect( &variableListSize );
        m_variableList.InsertColumn(0, ALIAS_COLUMN_NAME, LVCFMT_LEFT, variableListSize.right / VARIABLE_LIST_COLUMNS );	
        m_variableList.InsertColumn(0, VARIABLE_COLUMN_NAME, LVCFMT_LEFT, variableListSize.right / VARIABLE_LIST_COLUMNS );	

        // add variables to list view
        InterlockingRuleAliasVector::iterator ap = m_aliases.begin();
        for ( int i=0; ap < m_aliases.end(); ap++, i++ )
        {
            m_variableList.InsertItem(i, (*ap)->getAliasName().c_str() );
            m_variableList.SetItemText(i, VARIABLE_COLUMN, (*ap)->getVariableName().c_str() );
        }

        // load variable button vector for easy access to buttons
        m_variableButtons.clear();
        m_variableButtons.push_back(&m_variableBtn1);
        m_variableButtons.push_back(&m_variableBtn2);
        m_variableButtons.push_back(&m_variableBtn3);
        m_variableButtons.push_back(&m_variableBtn4);
        m_variableButtons.push_back(&m_variableBtn5);
        m_variableButtons.push_back(&m_variableBtn6);
        m_variableButtons.push_back(&m_variableBtn7);
        m_variableButtons.push_back(&m_variableBtn8);

        // get dll info from db
        m_variables = TA_Base_Core::InterlockVariableAccessFactory::getInstance().getInterlockVariables();

        // set and enable variable selector buttons as required
        std::vector<TA_Base_Core::IInterlockVariable*>::iterator vi = m_variables.begin();
        for ( i=0; vi != m_variables.end(); vi++)
        {
            if ( !(*vi)->getButtonText().empty() )
            {
                m_variableButtons[i]->SetWindowText((*vi)->getButtonText().c_str());
                m_variableButtons[i]->EnableWindow(true);
                m_variableButtons[i]->ShowWindow(true);
                i++;
            }
        }
		
		if (m_expression.IsEmpty() == false)
		{
			// Enable the ok and test button on start if expression is keyed TD 11790
			m_okButton.EnableWindow(true);
			m_testButton.EnableWindow(true);
		}
		else
		{
			// Disable the ok and test button on start and enable when expression is keyed TD 11399
			m_okButton.EnableWindow(false);
			m_testButton.EnableWindow(false);
		}

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    void InterlockingRuleDialog::OnDblclkVariableList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        POSITION pos = m_variableList.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			unsigned int i = m_variableList.GetNextSelectedItem(pos);
			// check for empty string
			CString chkString = m_variableList.GetItemText(i,0);
			if ( chkString.IsEmpty() == false )
			{
				addVariableToExpression( std::string( chkString ) );
			}
		}

	    *pResult = 0;
    }



    void InterlockingRuleDialog::OnVariableBtn1() 
    {
	    openVariableSelector(1);
    }

    void InterlockingRuleDialog::OnVariableBtn2() 
    {
	    openVariableSelector(2);
    }

    void InterlockingRuleDialog::OnVariableBtn3() 
    {
	    openVariableSelector(3);	    
    }

    void InterlockingRuleDialog::OnVariableBtn4() 
    {
	    openVariableSelector(4);	    
    }

    void InterlockingRuleDialog::OnVariableBtn5() 
    {
	    openVariableSelector(5);	    
    }

    void InterlockingRuleDialog::OnVariableBtn6() 
    {
	    openVariableSelector(6);	    
    }

    void InterlockingRuleDialog::OnVariableBtn7() 
    {
	    openVariableSelector(7);	    
    }

    void InterlockingRuleDialog::OnVariableBtn8() 
    {
	    openVariableSelector(8);	    
    }

    void InterlockingRuleDialog::openVariableSelector( int selector )
    {
        BeginWaitCursor();

        // convert selector to base 0
        selector--;
        std::string dllName = m_variables[selector]->getDllName();
        TA_ASSERT(!dllName.empty(), "Entity Type not configured in DB");

        // open dll
        VariableTypeFactory variableFactory;
        m_ivariable = variableFactory.createVariableSelector(dllName, this);
        TA_ASSERT(m_ivariable != NULL, "Variable factory broken!!");

        // show selector
        m_ivariable->showSelector();

        EndWaitCursor();
    }

    void InterlockingRuleDialog::addVariable(const std::string& variableName, const std::string& variableAlias, const std::string& variableType)
    {
        // check if variable already exists
        LVFINDINFO info;

        info.flags = LVFI_STRING;
        info.psz = variableName.c_str();
        
        if ( m_variableList.FindItem( &info ) != -1 )
        {
            return;
        }

        // add variable to list picker
        int place = m_variableList.GetItemCount();
        m_variableList.InsertItem(place, variableAlias.c_str() );
        m_variableList.SetItemText(place, VARIABLE_COLUMN, variableName.c_str() );

        // add variable to aliasList
        boost::shared_ptr<InterlockingRuleAlias> alias( new InterlockingRuleAlias() );
        alias->setAliasName( variableAlias );
        alias->setVariableName( variableName );
        alias->setVariableType( variableType );
        m_aliases.push_back( alias );

    }

    void InterlockingRuleDialog::addOperatorToExpression( const std::string& opr )
    {
        m_expressionEditor.ReplaceSel(" ");
        m_expressionEditor.ReplaceSel(opr.c_str());
        m_expressionEditor.ReplaceSel(" ");
        m_expressionEditor.SetFocus();
    }

    void InterlockingRuleDialog::addVariableToExpression( const std::string& var )
    {
        m_expressionEditor.ReplaceSel(" {");
        m_expressionEditor.ReplaceSel(var.c_str());
        m_expressionEditor.ReplaceSel("} ");
        m_expressionEditor.SetFocus();
    }

}//namespace

