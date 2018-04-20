/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/InterlockingRuleGuiManager.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
///////////////////////////////////////////////////////////
//  InterlockingRuleGuiManager.h
//  Implementation of the Class InterlockingRuleGuiManager
//  Created on:      27-Jan-2005 12:38:56 PM
///////////////////////////////////////////////////////////

#if !defined(InterlockingRuleGuiManager_9784DDE8_A1AA_4309_93DE_EDF814CC5D8A__INCLUDED_)
#define InterlockingRuleGuiManager_9784DDE8_A1AA_4309_93DE_EDF814CC5D8A__INCLUDED_

#include "bus\Interlocking\src\InterlockingRule.h"
#include "InterlockingRuleDialog.h"

namespace TA_Base_Core
{
    class ConfigUpdateMessageSender; 
    class AuditMessageSender;
}

namespace TA_Base_Bus
{
    /**
     * The main control center for the Interlocking Configurator
     *
     * Displays dialog as required and handles all message generation.
     */
    class InterlockingRuleGuiManager
    {

    public:
	    virtual ~InterlockingRuleGuiManager();

    public:
        /**
	     * This is the edit constructor. It takes a rule id.
	     * @param rule    The rule to edit.
	     * 
	     * 
	     */
        InterlockingRuleGuiManager( TA_Base_Bus::InterlockingRule* rule, const std::string& entityName );
	    
    
        /**
	     * Stores any changes to the db and sends all required messages.
	     * @param rule    The rule to be updated
	     * 
	     */
	    void updateRule();
	    
    
        /**
	     * Deletes the rule from the database and sends any required messages.
	     * @param rule    The rule to be deleted
	     * 
	     */
	    void deleteRule();
	    
    
        /**
	     * @param rule    The rule to be modified
	     * 
	     */
	    void editRule();
	    
    
        /**
	     * @param rule    The rule to display
	     * 
	     */
	    void viewRule();
	    
    
        /**
	     * setEntityName
         *
	     * @param name    The name of the entity this rule appies to
	     * 
	     */
        void setEntityName( const std::string& name);
	    
    private:

        // Assignment operator not used so it is made private
		InterlockingRuleGuiManager& operator=(const InterlockingRuleGuiManager &);
		
        // Copy constructor should not be used and has therefore been made private. 
        InterlockingRuleGuiManager( const InterlockingRuleGuiManager& );  

        /**
	     * The rule been created/modifed
	     */
        TA_Base_Bus::InterlockingRule* m_interlockingRule;

        /**
         * The modifed version of the rule
         */
        TA_Base_Bus::InterlockingRule m_localRule;

        /**
         * Entity name
         */
        std::string m_entityName;

        /**
         * the message senders
         */
        TA_Base_Core::ConfigUpdateMessageSender* m_commsSender;
        TA_Base_Core::AuditMessageSender* m_auditSender;
        void loadMessageSenders();


    };
} //namespace
#endif // !defined(InterlockingRuleGuiManager_9784DDE8_A1AA_4309_93DE_EDF814CC5D8A__INCLUDED_)