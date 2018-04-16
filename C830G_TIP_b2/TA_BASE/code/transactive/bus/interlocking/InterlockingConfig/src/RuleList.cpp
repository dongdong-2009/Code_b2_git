/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/RuleList.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
// RuleList.cpp : implementation file
//

#include "stdafx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/interlocking/src/IInterlockRule.h"
#include "bus/interlocking/src/RuleContainer.h"
#include "bus/interlocking/src/InterlockingRule.h"
#include "RuleList.h"
#include "InterlockingRuleGuiManager.h"
#include "InterlockingRuleTriggerEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//define columes for the list view
static const int RULE_COLUMN    (0);
static const int ACTION_COLUMN  (1);
static const int ENABLED_COLUMN (2);
static const int PRE_COLUMN     (3);
static const int DEFAULT_RULE_WIDTH (200);
static const CString ENABLED ("Y");
static const CString DISABLED ("");
static const CString PRECONDITION ("Y");
static const CString POSTCONDITION ("");
/////////////////////////////////////////////////////////////////////////////
// RuleList
namespace TA_Base_Bus
{
    RuleList::RuleList()
        :m_rules(NULL)
    {
    }

    RuleList::~RuleList()
    {
        if (m_rules != NULL )
        {
            delete(m_rules);
            m_rules = NULL;
        }
    }


    BEGIN_MESSAGE_MAP(RuleList, CListCtrl)
	    //{{AFX_MSG_MAP(RuleList)
	    ON_WM_CREATE()
	    ON_WM_ACTIVATE()
	    ON_WM_CHILDACTIVATE()
	    ON_WM_ENABLE()
	    ON_WM_SHOWWINDOW()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // RuleList message handlers


    /////////////////////////////////////////////////////////////////////////////
    // RuleList methods

    bool RuleList::init()
    {
        FUNCTION_ENTRY("init");

        const LPCTSTR lpszHeading0("Rule");
        const LPCTSTR lpszHeading1("Action");
        const LPCTSTR lpszHeading2("Enabled");
        const LPCTSTR lpszHeading3("Pre Check");

        // create some reasonable default column widths
        const int actionColumnWidth( CListCtrl::GetStringWidth(lpszHeading1) + 50 );
        const int enabledColumnWidth( CListCtrl::GetStringWidth(lpszHeading2) + 14 );
        const int preColumnWidth( CListCtrl::GetStringWidth(lpszHeading3) + 14 );

        // give everything else to the rule column - but ensure a good minimun
        RECT listSize;
        CListCtrl::GetClientRect( &listSize );
        int ruleColumnWidth( listSize.right - actionColumnWidth - enabledColumnWidth - preColumnWidth );
        if ( ruleColumnWidth < DEFAULT_RULE_WIDTH )
        {
            ruleColumnWidth = DEFAULT_RULE_WIDTH;
        }

        // now create the columns
        CListCtrl::ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS );
        CListCtrl::InsertColumn(RULE_COLUMN,    lpszHeading0, LVCFMT_LEFT, ruleColumnWidth);
        CListCtrl::InsertColumn(ACTION_COLUMN,  lpszHeading1, LVCFMT_LEFT, actionColumnWidth );
        CListCtrl::InsertColumn(ENABLED_COLUMN, lpszHeading2, LVCFMT_CENTER, enabledColumnWidth );
        CListCtrl::InsertColumn(PRE_COLUMN,     lpszHeading3, LVCFMT_CENTER, preColumnWidth );


        return true;
    }

    void RuleList::loadRules(const unsigned long entityId, const std::string& entityType, const std::string& entityName)
    {
        FUNCTION_ENTRY("loadRules");

        // save const info for later
        m_entityName = entityName;
        m_entityType = entityType;
        m_entityId = entityId;

        loadRules();

    }

    void RuleList::loadRules()
    {
        FUNCTION_ENTRY("loadRules()");

        try
        {
            if (m_rules != NULL)
            {
                delete(m_rules);
                m_rules = NULL;
            }
            m_rules = new TA_Base_Bus::RuleContainer( m_entityId, m_entityType );
        }
        catch(...)
        {
            TA_ASSERT(false, "Problem loading rules");
        }

        // remove any old rules
        CListCtrl::DeleteAllItems();

        // add rules to list view
        if (m_rules->size() > 0 )
        {
            for ( unsigned int i = 0; i < m_rules->size(); i++ )
            {
                TA_ASSERT((*m_rules)[i] != NULL, "null pointer");
                CListCtrl::InsertItem(i, (*m_rules)[i]->getName().c_str() );
                CListCtrl::SetItemText(i, ACTION_COLUMN,   (*m_rules)[i]->getEntityAction().c_str() );
                CListCtrl::SetItemText(i, ENABLED_COLUMN, ((*m_rules)[i]->getEnabled())?ENABLED:DISABLED );
                CListCtrl::SetItemText(i, PRE_COLUMN,     ((*m_rules)[i]->getPreCondition())?PRECONDITION:POSTCONDITION );

            }
        }
    }

    void RuleList::editRule()
    {
        FUNCTION_ENTRY("editRule");

        POSITION pos = CListCtrl::GetFirstSelectedItemPosition();

        if (pos == NULL)
        {
            //Future: provide a friendly reminder to select something
            return;
        }
    
        // This is a single select list so just use the position returned
        // For now the pos in the list is the same as the pos in m_rules
        // If we allow sorting this will change and the following logic
        // will become more complex :(
        unsigned int i = CListCtrl::GetNextSelectedItem(pos);
        TA_ASSERT((*m_rules)[i] != NULL, "null pointer");

        TA_Base_Bus::InterlockingRuleGuiManager guiManager( (*m_rules)[i], m_entityName );

        guiManager.editRule();

        // update form with any visable changes
        CListCtrl::SetItemText(i, RULE_COLUMN,     (*m_rules)[i]->getName().c_str() );
        CListCtrl::SetItemText(i, ACTION_COLUMN,   (*m_rules)[i]->getEntityAction().c_str() );
        CListCtrl::SetItemText(i, ENABLED_COLUMN, ((*m_rules)[i]->getEnabled())?ENABLED:DISABLED );
        CListCtrl::SetItemText(i, PRE_COLUMN,     ((*m_rules)[i]->getPreCondition())?PRECONDITION:POSTCONDITION );


    }

    void RuleList::viewRule()
    {
        FUNCTION_ENTRY("viewRule");

        POSITION pos = CListCtrl::GetFirstSelectedItemPosition();

        if (pos == NULL)
        {
            //Future: provide a friendly reminder to select something
            return;
        }
    
        // This is a single select list so just use the position returned
        // For now the pos in the list is the same as the pos in m_rules
        // If we allow sorting this will change and the following logic
        // will become more complex :(
        unsigned int i = CListCtrl::GetNextSelectedItem(pos);
        TA_ASSERT((*m_rules)[i] != NULL, "null pointer");

        TA_Base_Bus::InterlockingRuleGuiManager guiManager( (*m_rules)[i], m_entityName );

        guiManager.viewRule();

        // update form with any visable changes
        // Leave this in in case minor changes are allowed in future eg: enable/disable
        CListCtrl::SetItemText(i, RULE_COLUMN,     (*m_rules)[i]->getName().c_str() );
        CListCtrl::SetItemText(i, ACTION_COLUMN,   (*m_rules)[i]->getEntityAction().c_str() );
        CListCtrl::SetItemText(i, ENABLED_COLUMN, ((*m_rules)[i]->getEnabled())?ENABLED:DISABLED );
        CListCtrl::SetItemText(i, PRE_COLUMN,     ((*m_rules)[i]->getPreCondition())?PRECONDITION:POSTCONDITION );


    }

    void RuleList::deleteRule()
    {
        FUNCTION_ENTRY("deleteRule");

        POSITION pos = CListCtrl::GetFirstSelectedItemPosition();

        if (pos == NULL)
        {
            //Future: provide a friendly reminder to select something
            return;
        }
    
        // This is a single select list so just use the position returned
        // For now the pos in the list is the same as the pos in m_rules
        // If we allow sorting this will change and the following logic
        // will become more complex :(
        unsigned int i = CListCtrl::GetNextSelectedItem(pos);
        TA_ASSERT((*m_rules)[i] != NULL, "null pointer");

        TA_Base_Bus::InterlockingRuleGuiManager guiManager( (*m_rules)[i], m_entityName );

        guiManager.deleteRule();

        // remove rule from list
        m_rules->deleteRule( (*m_rules)[i]->getEntityKey(), (*m_rules)[i]->getRuleId() );
        CListCtrl::DeleteItem(i);

    }

    void RuleList::newRule()
    {
        FUNCTION_ENTRY("newRule");

        // Start by creating the trigger
    
        // create trigger dialog screen
        TA_Base_Bus::InterlockingRuleTriggerEditor itriggerEditor;

        // copy rule information to trigger dialog
        itriggerEditor.setEntityName( m_entityName );
        itriggerEditor.setEntityKey( m_entityId );
        itriggerEditor.setEntityType( m_entityType );
        itriggerEditor.setAction( std::string("") );
        itriggerEditor.setParameter( std::string("") );

        // display trigger editor dialog 
        int ret = itriggerEditor.DoModal();
    
        // do we create a new rule?
        if ( ret == IDOK )
        {
            // user is happy with the changes - create new rule
            TA_Base_Bus::InterlockingRule rule;

            // fill in known details (trigger)
            rule.setEntityKey( m_entityId );
            rule.setEntityType( m_entityType );
            rule.setEntityAction( itriggerEditor.getAction() );
            rule.addActionParameter( itriggerEditor.getParameter() );
            std::string name( m_entityName + "." + itriggerEditor.getAction() + "." + itriggerEditor.getParameter() );
            rule.setName( name );

            // now get user to fill in the rest
            TA_Base_Bus::InterlockingRuleGuiManager guiManager( &rule, m_entityName );

            guiManager.editRule();

            if ( rule.getRuleId() == 0 )
            {
                // user cancled the add
                return;
            }

            // add the new rule to the list
            // if we change the list to be sorted this should also change to maintain the sort order
            m_rules->loadRule( rule.getRuleId() );
            int count = CListCtrl::GetItemCount();
            CListCtrl::InsertItem(count, rule.getName().c_str() );
            CListCtrl::SetItemText(count, ACTION_COLUMN,   rule.getEntityAction().c_str() );
            CListCtrl::SetItemText(count, ENABLED_COLUMN, (rule.getEnabled())?"Y":"" );
            CListCtrl::SetItemText(count, PRE_COLUMN,     (rule.getPreCondition())?"Y":"" );

            // select the new rule in the list
            CListCtrl::SetSelectionMark(count);
        }
    }

} //namespace