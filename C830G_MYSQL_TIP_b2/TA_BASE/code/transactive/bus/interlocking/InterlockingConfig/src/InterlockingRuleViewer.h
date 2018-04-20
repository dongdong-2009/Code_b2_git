/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/InterlockingRuleViewer.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
#if !defined(AFX_INTERLOCKINGRULEVIEWER_H_)
#define AFX_INTERLOCKINGRULEVIEWER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <vector>
#include <string>
#include "bus/interlocking/src/InterlockingRuleAlias.h"
// InterlockingRuleViewer.h : header file
//

namespace TA_Base_Bus
{
    /**
     * Allows a user to view and test a rule.
     *
     * No changes are currently permitted. 
     * Minor changes (enable/disable) may be implemented in the future
     */
    class InterlockingRuleViewer : public CDialog
    {
    // Construction
    public:
	    InterlockingRuleViewer(CWnd* pParent = NULL);   // standard constructor

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(InterlockingRuleViewer)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
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
	    //{{AFX_MSG(InterlockingRuleViewer)
	    afx_msg void OnCancel();
	    afx_msg void OnOk();
	    afx_msg void OnOpenRuleEditor();
	    afx_msg void OnOpenTriggerEditor();
	    afx_msg void OnTest();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
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

        // Dialog Data
	    //{{AFX_DATA(InterlockingRuleViewer)
	    enum { IDD = IDD_InterlockingRuleViewer };
	    CEdit	m_expressionEditor;
	    CString	m_name;
	    CString	m_expression;
	    BOOL	m_enabled;
	    CString	m_description;
	    int		m_precondition;
	//}}AFX_DATA

        // these variables are used by child dialogs
        CString m_entityName;
        unsigned long m_entityKey;
        CString m_entityType;
        CString m_entityAction;
        std::vector<std::string> m_actionParameters;
        InterlockingRuleAliasVector m_aliases;

    
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}//namespace
#endif // !defined(AFX_INTERLOCKINGRULEVIEWER_H_)
