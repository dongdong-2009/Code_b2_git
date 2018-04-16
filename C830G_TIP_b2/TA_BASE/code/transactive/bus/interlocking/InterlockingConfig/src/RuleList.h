/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/RuleList.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
#if !defined(AFX_RULELIST_H__DBF3470B_988C_4C6F_86C1_E923714FD27B__INCLUDED_)
#define AFX_RULELIST_H__DBF3470B_988C_4C6F_86C1_E923714FD27B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RuleList.h : header file
//
#include <vector>
#include "bus/interlocking/src/RuleContainer.h"

#ifdef BUILDING_INTERLOCKINGCONFIG_DLL
#define EXPORT_INTDLL __declspec(dllexport) 
#else
#define EXPORT_INTDLL __declspec(dllimport) 
#endif

/////////////////////////////////////////////////////////////////////////////
// RuleList window
namespace TA_Base_Bus
{

    /**
     * This class must be used on every screen where the user
     * is allowed to view/edit interlocking rules
     *
     * No other classes should be required.
     *
     * It is the public interface for the InterlockingConfig package
     */
    class EXPORT_INTDLL RuleList : public CListCtrl
    {
    public:
	    RuleList();

    public:
	    virtual ~RuleList();

    public:
        /**
         *  init
         *
         * Creates and lays out the control
         *
         */
        bool init();

        /**
         *  loadRules
         *
         *  Loads all rules for the given entity.
         *
         * @param entityId The id for this entity. It must be unique for each entity type.
         * @param entityType The type of entity. It must be a type used in the LK_ENTITY_TYPES table.
         * @param entityName The name the entity is know by, by the users of the system.
         */
        void loadRules(const unsigned long entityId, const std::string& entityType, const std::string& entityName);

        /**
         *  editRule
         * 
         *  Opens the dialog editor for the selected rule. If no rule is selected it does nothing.
         */
        void editRule();

        /**
         *  deleteRule
         *
         *  Deletes the selected rule. If no rule is selected it does nothing.
         */
        void deleteRule();

        /**
         *  newRule
         *
         *  Opens the trigger editor. If the user does not cancel the trigger editor,
         *  it will then open the edit dialog so the user can fill in the rest of the rule details
         */
        void newRule();

        /**
         *  viewRule
         *
         *  Opens the rule viewer
         */
        void viewRule();

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(RuleList)
	    //}}AFX_VIRTUAL


	    // Generated message map functions
    protected:
	    //{{AFX_MSG(RuleList)
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()

    private:
        /**
         *  loadRules
         *
         *  Loads all rules for the known entity.
         */
        void loadRules();

        // Assignment operator not used so it is made private
	    RuleList& operator=(const RuleList &);
	    
        // Copy constructor should not be used and has therefore been made private. 
        RuleList( const RuleList& );  

    // Attributes
    private:
        TA_Base_Bus::RuleContainer* m_rules;
        std::string m_entityName;
        std::string m_entityType;
        unsigned long m_entityId;

    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
} // end namespace
#endif // !defined(AFX_RULELIST_H__DBF3470B_988C_4C6F_86C1_E923714FD27B__INCLUDED_)
