/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/InterlockingRuleGuiManager.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
///////////////////////////////////////////////////////////
//  InterlockingRuleGuiManager.cpp
//  Implementation of the Class InterlockingRuleGuiManager
//  Created on:      27-Jan-2005 12:38:56 PM
///////////////////////////////////////////////////////////

#include "stdafx.h"

#include <vector>
#include <sstream>
#include "core/utilities/src/TAAssert.h"
#include "resource.h"
#include "InterlockingRuleGuiManager.h"
#include "InterlockingRuleViewer.h"

#include "core\utilities\src\RunParams.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"
#include "core/message/types/InterlockingConfigAudit_MessageTypes.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/src/AuditMessageSender.h"

extern AFX_EXTENSION_MODULE TA_InterlockingConfigDLL;
namespace TA_Base_Bus
{

    InterlockingRuleGuiManager::~InterlockingRuleGuiManager()
    {
    }


    /**
     * This is the edit constructor. It takes a rule id.
     * @param rule    The rule to edit.
     * 
     * 
     */
    InterlockingRuleGuiManager::InterlockingRuleGuiManager( TA_Base_Bus::InterlockingRule* rule, const std::string& entityName)
        :m_interlockingRule(rule),
         m_entityName(entityName),
         m_commsSender(NULL),
         m_auditSender(NULL)
    {
        TA_ASSERT(rule != NULL, "null pointer passed");
        m_localRule.setEntityKey(m_interlockingRule->getEntityKey());
        m_localRule.setEntityType(m_interlockingRule->getEntityType());
        m_localRule.setEntityAction(m_interlockingRule->getEntityAction());
        m_localRule.setActionParameters(m_interlockingRule->getActionParameters());
        m_localRule.setEnabled(m_interlockingRule->getEnabled());
        m_localRule.setPrecondition(m_interlockingRule->getPreCondition());
        m_localRule.setName(m_interlockingRule->getName());
        m_localRule.setDescription(m_interlockingRule->getDescription());
        m_localRule.setCondition(m_interlockingRule->getCondition());
        m_localRule.setAliases(m_interlockingRule->getAliases());

        loadMessageSenders();
    }



    /**
     * loadMessageSender
     *
     * If the users fails to set all the required run parameters,
     * we will assume that this is a simple test setup and keep
     * things simple. ie: no messaging
     */
    void InterlockingRuleGuiManager::loadMessageSenders()
    {
        FUNCTION_ENTRY("loadMessageSender");

        if (!TA_Base_Core::RunParams::getInstance().isSet( RPARAM_NOTIFYHOSTS ) )
        {
            return;
        }
        if (!TA_Base_Core::RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ) )
        {
            return;
        }
        m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().getConfigUpdateMessageSender( TA_Base_Core::ConfigUpdate::Context );
        m_auditSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::InterlockingConfigAudit::Context);
    }

    /**
     * Stores any changes to the db and sends all required messages.
     * @param rule    The rule to be updated
     * 
     */
    void InterlockingRuleGuiManager::updateRule()
    {
        FUNCTION_ENTRY("updateRule");

        // have to do this test befor calling update() cause update will create a rule id if required
        TA_Base_Core::EModificationType action = (m_interlockingRule->getRuleId() == 0) ?
            TA_Base_Core::Create : TA_Base_Core::Update;

        bool update = (m_interlockingRule->getRuleId() == 0) ? false : true;
        
        m_interlockingRule->update();
                
        if (m_commsSender != NULL)
        {
            std::stringstream ruleId;
            ruleId << m_interlockingRule->getRuleId();
            std::vector<std::string> data;
            data.push_back( ruleId.str() );

            m_commsSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigInterlocking, // message type
                                                   m_interlockingRule->getEntityKey(),             // entity key
                                                   action,                                         // modification type 
                                                   data);                                          // subsystem key

        }

        if (m_auditSender != NULL)
        {

            TA_Base_Core::DescriptionParameters dp;
            TA_Base_Core::NameValuePair param1("InterlockingRuleName", m_interlockingRule->getName());
        
            dp.push_back(&param1);
        
            if (update)
            {
                m_auditSender->sendAuditMessage(
                    TA_Base_Core::InterlockingConfigAudit::InterlockingRuleUpdated,// Message Type
                    0,                                                       // Entity key
                    dp,                                                      // Description
                    "",                                                      // Additional details
                    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID), // SessionID
                    "",                                                      // AlarmID if an alarm associated with event
                    "",                                                      // IncidentKey if incident associated with event
                    "");                                                     // EventID of a parent event, used to link events
            }
            else // create
            {
                m_auditSender->sendAuditMessage(
                    TA_Base_Core::InterlockingConfigAudit::InterlockingRuleCreated,// Message Type
                    0,                                                       // Entity key
                    dp,                                                      // Description
                    "",                                                      // Additional details
                    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID), // SessionID
                    "",                                                      // AlarmID if an alarm associated with event
                    "",                                                      // IncidentKey if incident associated with event
                    "");                                                     // EventID of a parent event, used to link events
            }

        }
    }

    /**
     * Deletes the rule from the database and sends any required messages.
     * @param rule    The rule to be deleted
     * 
     */
    void InterlockingRuleGuiManager::deleteRule()
    {
        FUNCTION_ENTRY("deleteRule");

        m_interlockingRule->deleteMe();

        if (m_commsSender != NULL)
        {
            std::stringstream ruleId;
            ruleId << m_interlockingRule->getRuleId();
            std::vector<std::string> data;
            data.push_back( ruleId.str() );
            m_commsSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigInterlocking, // message type
                                                   m_interlockingRule->getEntityKey(),             // entity key
                                                   TA_Base_Core::Delete,                           // modification type 
                                                   data);                                          // subsystem key
        }

        if (m_auditSender != NULL)
        {

            TA_Base_Core::DescriptionParameters dp;
            TA_Base_Core::NameValuePair param1("InterlockingRuleName", m_interlockingRule->getName());
        
            dp.push_back(&param1);
        
            m_auditSender->sendAuditMessage(
                TA_Base_Core::InterlockingConfigAudit::InterlockingRuleDeleted, // Message Type
                0,                                                       // Entity key
                dp,                                                      // Description
                "",                                                      // Additional details
                TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID), // SessionID
                "",                                                      // AlarmID if an alarm associated with event
                "",                                                      // IncidentKey if incident associated with event
                "");                                                     // EventID of a parent event, used to link events
        }
    }


    /**
     * @param rule    The rule to be modified
     * 
     */
    void InterlockingRuleGuiManager::editRule()
    {
        FUNCTION_ENTRY("editRule");

        // create main dialog screen
        InterlockingRuleDialog iruleDialog;

        // copy rule information to main dialog
        iruleDialog.setName( m_localRule.getName() );
        iruleDialog.setDescription( m_localRule.getDescription() );
        iruleDialog.setExpression( m_localRule.getCondition() );
        iruleDialog.setPrecondition( m_localRule.getPreCondition() );
        iruleDialog.setEnabled( m_localRule.getEnabled() );
        iruleDialog.setEntityName( m_entityName );
        iruleDialog.setEntityKey( m_interlockingRule->getEntityKey() );
        iruleDialog.setEntityType( m_interlockingRule->getEntityType() );
        iruleDialog.setEntityAction( m_interlockingRule->getEntityAction() );
        iruleDialog.setActionParameters( m_interlockingRule->getActionParameters() );
        iruleDialog.setAliases( m_localRule.getAliases() );

    	// Use our resources only
	    HINSTANCE hInstOld = AfxGetResourceHandle();
	    AfxSetResourceHandle( TA_InterlockingConfigDLL.hModule );
		
        // display main dialog - blocking call
        int ret = iruleDialog.DoModal();
        
    	 // restore the old resource chain
        AfxSetResourceHandle(hInstOld);

        // do we save or abandon changes
        if (ret == IDOK)
        {
            // user is happy with the changes - save them
            m_interlockingRule->setEntityKey( iruleDialog.getEntityKey() );
            m_interlockingRule->setEntityType( iruleDialog.getEntityType() );
            m_interlockingRule->setEntityAction( iruleDialog.getEntityAction() );
            m_interlockingRule->setActionParameters( iruleDialog.getActionParameters() );
            m_interlockingRule->setEnabled( iruleDialog.getEnabled() ); 
            m_interlockingRule->setPrecondition( iruleDialog.getPrecondition() );
            m_interlockingRule->setName( iruleDialog.getName() );
            m_interlockingRule->setDescription( iruleDialog.getDescription() );
            try
            {
                m_interlockingRule->setCondition( std::string( iruleDialog.getExpression() ));
            }
            catch(...)
            {
                // This should be caught by the test function before it gets here
                TA_ASSERT( false, "Invalid Expression");
            }
            m_interlockingRule->setAliases( iruleDialog.getAliases() );
            updateRule();

            // return rule id
            m_localRule.setRuleId( m_interlockingRule->getRuleId() );
        }
    }

    /**
     * @param rule    The rule to display
     * 
     */
    void InterlockingRuleGuiManager::viewRule()
    {
        FUNCTION_ENTRY("viewRule");

        // create main dialog screen
        InterlockingRuleViewer iruleDialog;

        // copy rule information to main dialog
        iruleDialog.setName( m_localRule.getName() );
        iruleDialog.setDescription( m_localRule.getDescription() );
        iruleDialog.setExpression( m_localRule.getCondition() );
        iruleDialog.setPrecondition( m_localRule.getPreCondition() );
        iruleDialog.setEnabled( m_localRule.getEnabled() );
        iruleDialog.setEntityName( m_entityName );
        iruleDialog.setEntityKey( m_interlockingRule->getEntityKey() );
        iruleDialog.setEntityType( m_interlockingRule->getEntityType() );
        iruleDialog.setEntityAction( m_interlockingRule->getEntityAction() );
        iruleDialog.setActionParameters( m_interlockingRule->getActionParameters() );
        iruleDialog.setAliases( m_localRule.getAliases() );

    	// Use our resources only
	    HINSTANCE hInstOld = AfxGetResourceHandle();

	    AfxSetResourceHandle( TA_InterlockingConfigDLL.hModule);
		
        // display main dialog - blocking call
        int ret = iruleDialog.DoModal();
        
    	 // restore the old resource chain
        AfxSetResourceHandle(hInstOld);

        // do we save or abandon changes
        if (ret == IDOK)
        {
            // FUTURE we may allow minor changes on this screen
            // eg: enable/disable
            // for now do nothing

        }

    }

 } //namespace
