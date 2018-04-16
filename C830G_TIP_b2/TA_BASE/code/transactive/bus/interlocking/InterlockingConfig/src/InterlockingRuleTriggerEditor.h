/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/InterlockingRuleTriggerEditor.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
#if !defined(AFX_INTERLOCKINGRULETRIGGEREDITOR_H__DEEA693D_B543_44D9_882D_4EFBC4EA4941__INCLUDED_)
#define AFX_INTERLOCKINGRULETRIGGEREDITOR_H__DEEA693D_B543_44D9_882D_4EFBC4EA4941__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "resource.h"
// InterlockingRuleTriggerEditor.h : header file
//

namespace TA_Base_Bus
{
    class IEntityAction;
    
    /**
     * The place to create a new rule or change the trigger conditions for a current rule
     */
    class InterlockingRuleTriggerEditor : public CDialog
    {
        // Construction
    public:
	    InterlockingRuleTriggerEditor(CWnd* pParent = NULL);   // standard constructor

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(InterlockingRuleTriggerEditor)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

        // Implementation
    public:
   	    /**
         * setAction
         *
         * sets the action to the passed in value
         *
         * @param in The value to set it to
         */
        void setAction( const std::string& in)
        {
            m_action  = in.c_str();
        }

        /** 
         *  getAction
         *
         *  Returns the current value of m_action
         *
         */
        std::string	getAction()
        {
            return std::string(m_action);
        }

        /**
         * setEntityName
         *
         * sets the entityName to the passed in value
         *
         * @param in The value to set it to
         */
        void setEntityName( const std::string& in)
        {
            m_entityName  = in.c_str();
        }

        /** 
         *  getEntityName
         *
         *  Returns the current value of m_entityName
         *
         */
        std::string	getEntityName()
        {
            return std::string(m_entityName);
        }

        /**
         * setParameter
         *
         * sets the parameter to the passed in value
         *
         * @param in The value to set it to
         */
        void setParameter( const std::string& in)
        {
            m_parameter  = in.c_str();
        }

        /** 
         *  getParameter
         *
         *  Returns the current value of m_parameter
         *
         */
        std::string	getParameter()
        {
            return std::string(m_parameter);
        }

        /**
         * setEntityType
         *
         * sets the entityType to the passed in value
         *
         * @param in The value to set it to
         */
        void setEntityType( const std::string& in)
        {
            m_entityType  = in.c_str();
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
         * setEntityKey
         *
         * sets the entityKey to the passed in value
         *
         * @param in The value to set it to
         */
        void setEntityKey( const unsigned long in)
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


    protected:

	    // Generated message map functions
	    //{{AFX_MSG(InterlockingRuleTriggerEditor)
	    afx_msg void OnSelchangeAction();
	    afx_msg void OnSelchangeParameters();
	    virtual BOOL OnInitDialog();
	    afx_msg void OnCancel();
	    afx_msg void OnOk();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        /**
         *  loadActionList
         *
         *  Populates the action dropdown list with the supplied items.
         *  It replaces anything currently in the list
         *
         * @param actions The list of valid actions
         */
        void loadActionList(const std::vector<std::string>& actions);

        /**
         *  loadParameterList
         *
         *  Populates the action parameter dropdown list with the supplied items.
         *  It replaces anything currently in the list
         *
         * @param parameters The list of valid parameters
         */
        void loadParameterList(const std::vector<std::string>& parameters);

        // Assignment operator not used so it is made private
		InterlockingRuleTriggerEditor& operator=(const InterlockingRuleTriggerEditor &);
		
        // Copy constructor should not be used and has therefore been made private. 
        InterlockingRuleTriggerEditor( const InterlockingRuleTriggerEditor& );  

        // Dialog Data
	    //{{AFX_DATA(InterlockingRuleTriggerEditor)
	    enum { IDD = IDD_InterlockingRuleTriggerEditor };
	    CComboBox	m_parameters;
	    CComboBox	m_actions;
	    CString	m_action;
	    CString	m_entityName;
	    CString	m_parameter;
    	//}}AFX_DATA

        CString m_entityType;
        unsigned long m_entityKey;
        IEntityAction* m_ientityAction;

     };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}// namespace
#endif // !defined(AFX_INTERLOCKINGRULETRIGGEREDITOR_H__DEEA693D_B543_44D9_882D_4EFBC4EA4941__INCLUDED_)
