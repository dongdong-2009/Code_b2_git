/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/InterlockingRuleDialog.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
#if !defined(AFX_INTERLOCKINGRULEDIALOG_H__90EB2B8E_40F5_4CCC_A92D_C9E9B788B74E__INCLUDED_)
#define AFX_INTERLOCKINGRULEDIALOG_H__90EB2B8E_40F5_4CCC_A92D_C9E9B788B74E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <vector>
#include <string>
#include <list>
#include "bus/interlocking/src/InterlockingRuleAlias.h"
#include "IAddVariable.h"
#include "IVariable.h"

// InterlockingRuleDialog.h : header file
//
namespace TA_Base_Core
{
    class IInterlockVariable;
}
namespace TA_Base_Bus
{
    /**
     * The main place to edit/create an InterlockingRule
     *
     * Displays the user interaction dialog
     */
    class InterlockingRuleDialog : public CDialog, public IAddVariable
    {
    // Construction
    public:
	    InterlockingRuleDialog(CWnd* pParent = NULL);   // standard constructor

    // Implementation
    public:
        /**
         *  addVariable
         * 
         *  call back function used by the variable selectors to add a variable to the list
         *  of variables the user can pick from.
         *
         * @param variableName  The name of the entity that can be used as a variable
         * @param variableAlias The user defined alias for this variable
         * @param variableType  The internal type used to class this variable. Used to get the contexts.
         */
        void addVariable(const std::string& variableName, const std::string& variableAlias, const std::string& variableType);

    public:
        /**
         * setName
         *
         * sets the name to the passed in value
         *
         * @param in The value to set it to
         */
        void setName( const std::string& in)
        {
            m_name = in.c_str();
        }

        /** 
         *  getName
         *
         *  Returns the current value of m_name
         *
         */
        std::string	getName()
        {
            return std::string(m_name);
        }

        /**
         * setExpression
         *
         * sets the expression to the passed in value
         *
         * @param in The value to set it to
         */
        void setExpression( const std::string& in)
        {
            // FUTURE: parse condition and replace variables with aliases
            m_expression = in.c_str();
        }

        /** 
         *  getExpression
         *
         *  Returns the current value of m_expression
         *
         */
        std::string	getExpression()
        {
            // FUTURE: parse condition and replace aliases with variables
            return std::string(m_expression);
        }

        /**
         * setEnabled
         *
         * sets the enabled to the passed in value
         *
         * @param in The value to set it to
         */
        void setEnabled( const bool in)
        {
            m_enabled = in;
        }

        /** 
         *  getEnabled
         *
         *  Returns the current value of m_enabled
         *
         */
        bool getEnabled()
        {
            return (m_enabled == 0)? false:true;
        }


        /**
         * setDescription
         *
         * sets the description to the passed in value
         *
         * @param in The value to set it to
         */
        void setDescription( const std::string& in)
        {
            m_description = in.c_str();
        }

        /** 
         *  getDescription
         *
         *  Returns the current value of m_description
         *
         */
        std::string	getDescription()
        {
            return std::string(m_description);
        }

        /**
         * setPrecondition
         *
         * sets the precondition to the passed in value
         *
         * @param in The value to set it to
         */
        void setPrecondition( const bool in)
        {
            m_precondition = in? 0:1;
        }

        /** 
         *  getPrecondition
         *
         *  Returns the current value of m_precondition
         *
         */
        bool getPrecondition()
        {
            return (m_precondition == 0)? true:false;
        }

        /**
         * setEntityName
         *
         * sets the entityName to the passed in value
         *
         * @param in The value to set it to
         */
        void setEntityName( const std::string& in )
        {
            m_entityName = in.c_str();
        }

        /** 
         *  getEntityName
         *
         *  Returns the current value of m_entityName
         *
         */
        std::string getEntityName()
        {
            return std::string(m_entityName);
        }

        /**
         * setEntityKey
         *
         * sets the entityKey to the passed in value
         *
         * @param in The value to set it to
         */
        void setEntityKey( const unsigned long in )
        {
            m_entityKey = in;
        }

        /** 
         *  getEntityKey
         *
         *  Returns the current value of m_entityKey
         *
         */
        unsigned long getEntityKey()
        {
            return m_entityKey;
        }

        /**
         * setEntityType
         *
         * sets the entityType to the passed in value
         *
         * @param in The value to set it to
         */
        void setEntityType( const std::string& in )
        {
            m_entityType = in.c_str();
        }

        /** 
         *  getEntityType
         *
         *  Returns the current value of m_entityType
         *
         */
        std::string getEntityType()
        {
            return std::string(m_entityType);
        }

        /**
         * setEntityAction
         *
         * sets the entityAction to the passed in value
         *
         * @param in The value to set it to
         */
        void setEntityAction( const std::string& in)
        {
            m_entityAction = in.c_str();
        }

        /** 
         *  getEntityAction
         *
         *  Returns the current value of m_entityAction
         *
         */
        std::string getEntityAction()
        {
            return std::string(m_entityAction);
        }

        /**
         * setActionParameters
         *
         * sets the actionParameters to the passed in value
         *
         * @param in The value to set it to
         */
        void setActionParameters( const std::vector<std::string>& in)
        {
            m_actionParameters = in;
        }

        /** 
         *  getActionParameters
         *
         *  Returns the current value of m_actionParameters
         *
         */
        std::vector<std::string> getActionParameters()
        {
            return m_actionParameters;
        }

        /**
         * setAliases
         *
         * sets the aliases to the passed in value
         *
         * @param in The value to set it to
         */
        void setAliases( const InterlockingRuleAliasVector& in)
        {
            m_aliases = in;
        }

        /** 
         *  getAliases
         *
         *  Returns the current value of m_aliases
         *
         */
        InterlockingRuleAliasVector getAliases()
        {
            return m_aliases;
        }


    protected:
	    // Generated message map functions
	    //{{AFX_MSG(InterlockingRuleDialog)
	    afx_msg void OnOk();
	    afx_msg void OnOpenTriggerEditor();
	    afx_msg void OnTest();
	    afx_msg void OnDblclkOperatorList();
	    afx_msg void OnCancel();
	    virtual BOOL OnInitDialog();
	    afx_msg void OnDblclkVariableList(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnVariableBtn1();
	    afx_msg void OnVariableBtn2();
	    afx_msg void OnVariableBtn3();
	    afx_msg void OnVariableBtn4();
	    afx_msg void OnVariableBtn5();
	    afx_msg void OnVariableBtn6();
	    afx_msg void OnVariableBtn7();
	    afx_msg void OnVariableBtn8();
		afx_msg void OnKillfocusRuleExpression();
	afx_msg void OnChangeRuleExpression();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(InterlockingRuleDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    private:
        /**
         *  isExpressionPresent
         *
         *  Ensure that some rules or inputs are keyed into the dialog before testing the expression
		 *  Fix for TD 11399
         *
         * @returns true if there are some expression or rules keyed
         */
        bool isExpressionPresent( );

        /**
         *  testExpression
         * 
         *  Tests the current expression to ensure it's valid and optionaly displays 
         *  what it currently evalutes to.
         *
         * @param showResult If true, the current result is displayed
         * @returns true if the expression can be evaluted without error
         */
        bool testExpression( bool showResult );

        /**
         *  openVariableSelector
         *
         *  Creates/displays a variable selector for the user
         *
         * @param selector An index into the list of selectors
         */
        void openVariableSelector( int selector );

        /**
         *  addOperatorToExpression
         *
         *  Adds the operator to the expression at the current cursor location
         *
         * @param opr The operator to add
         */
        void addOperatorToExpression( const std::string& opr );

        /**
         *  addVariableToExpression
         *
         *  Adds the variable to the expression at the current cursor location
         *
         * @param var The variable to add
         */
        void addVariableToExpression( const std::string& var );

        /**
         *  destroyVariableDLL
         *
         *  Destroy the created variable DLL object if it is not NULL
		 *  upon closing the window
         *
         */
		void destroyVariableDLL();

        // Dialog Data
    private:
        CString m_entityName;
        unsigned long m_entityKey;
        CString m_entityType;
        CString m_entityAction;
        std::vector<std::string> m_actionParameters;
        InterlockingRuleAliasVector m_aliases;
		IVariable* m_ivariable;

        std::vector<CButton*> m_variableButtons;
        std::vector<TA_Base_Core::IInterlockVariable*> m_variables;
    
    private:
        //{{AFX_DATA(InterlockingRuleDialog)
	enum { IDD = IDD_InterlockingRuleDialog };
	CButton	m_testButton;
	CButton	m_okButton;
	    CEdit	m_expressionEditor;
	    CButton	m_variableBtn8;
	    CButton	m_variableBtn7;
	    CButton	m_variableBtn6;
	    CButton	m_variableBtn5;
	    CButton	m_variableBtn4;
	    CButton	m_variableBtn3;
	    CButton	m_variableBtn2;
	    CButton	m_variableBtn1;
	    CListCtrl	m_variableList;
	    CListBox	m_operatorList;
	    CEdit	m_nameDisplay;
	    CString	m_name;
	    CString	m_expression;
	    BOOL	m_enabled;
	    CString	m_description;
	    int		m_precondition;
	//}}AFX_DATA

        // following used to define the columns in the variable selector
        static const int VARIABLE_COLUMN;
        static const int ALIAS_COLUMN;
        static const CString VARIABLE_COLUMN_NAME;
        static const CString ALIAS_COLUMN_NAME;
        static const int VARIABLE_LIST_COLUMNS;

    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}//namespace
#endif // !defined(AFX_INTERLOCKINGRULEDIALOG_H__90EB2B8E_40F5_4CCC_A92D_C9E9B788B74E__INCLUDED_)
