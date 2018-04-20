#ifndef Bus_Layer_Interlocking_RuleContainer_H
#define Bus_Layer_Interlocking_RuleContainer_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/src/RuleContainer.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#include <map>
#include <vector>
#include "boost/shared_ptr.hpp"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"
#include "core/synchronisation/src/WriteReadThreadLockable.h"

namespace TA_Base_Bus
{
    class InterlockingRule;
    class IBooleanContext;
    class IStringContext;
    class IRealNumberContext;

    /**
     * Holds all rules of "intrest".
     * Currently your options are *all* rules or all rules for an entity.
     */
    class RuleContainer : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::ConfigUpdateMessageCorbaDef>
                        , public TA_Base_Core::WriteReadThreadLockable
    {
    public:

        /**
          * Constructor
          */
        RuleContainer();

        /**
          * Constructor for a specific entity
          */
        RuleContainer(const unsigned long p_entityKey, const std::string& p_entityType);

        /**
          * Destructor
          */
        virtual ~RuleContainer();

        /**
          * getRulesForEntityAndDesiredState
          *
          * @param The entity key of the device whos state we want to change
          * @param The state we want to change to (this can be the string representation of
          *        a number, a boolean constant (true or false) or a variable name that can be
          *        looked up within the supplied context).
          * @param If this is a launch/precondition check(true) or a return/postcondition check(false).
          *
          * @return A vector of rules that apply to the entity when being put into the
          *         desired state.
          */
        std::vector< boost::shared_ptr<TA_Base_Bus::InterlockingRule> > getRulesForEntity(unsigned long p_entityKey,
                                                                 const std::string& p_desiredState,
                                                                 const bool preCondition = true ) const;

        /**
          * getRulesForEntityAndDesiredState
          *
          * @param The entity key of the device whos state we want to change
          * @param The action we want to preform on this entity
          * @param The action parameter.
          *        eg1 if entity is a sign the action could be "set" and the parameter could be "scene 3"
          *        eg2 if entity is a setpoint the action could be "set" and the parameter could be "50%"
          * @param If this is a launch/precondition check(true) or a return/postcondition check(false).
          *
          * @return A vector of rules that apply to the entity when being put into the
          *         desired state.
          */
        std::vector< boost::shared_ptr<TA_Base_Bus::InterlockingRule> > getRulesForEntity(unsigned long p_entityKey,
                                                                 const std::string& p_action,
                                                                 const std::string& p_parameter,
                                                                 const bool preCondition = true ) const;

        /**
          * enableOnlineConfig
          *
          */
        void enableOnlineConfig();

        /**
         * receiveSpecialisedMessage
         *
         * to receive update message if online config is enabled
         */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::ConfigUpdateMessageCorbaDef& message); 

        /**
          * addRule
          *
          * @param The interlocking rule
          */
        void addRule( boost::shared_ptr<TA_Base_Bus::InterlockingRule>& p_rule );

        /**
          * deleteRule
          *
          * deletes the rule from the container NOT the database
          *
          * @param p_entityKey the entity key this rule releates to
          * @param p_ruleId the id of the rule to be deleted
          */
        void deleteRule( unsigned int p_entityKey, unsigned long p_ruleId );

        /**
          * addBooleanContext
          *
          * This method is used to add boolean contexts to each of the interlocking rules.
          * These contexts will be used to retrieve the values for any boolean variables
          * included in supplied expressions.
          *
          * NOTE - The rule container does not take ownership of these context objects. That
          *        is, it will NOT delete them. However, they do need to exist for the lifetime
          *        of the rule container. You should delete them after deleting the rule
          *        container object.
          *
          * @param The context to add
          */
        void addBooleanContext( const IBooleanContext* p_booleanContext );

        /**
          * addRealNumberContext
          *
          * This method is used to add real number contexts to each of the interlocking rules.
          * These contexts will be used to retrieve the values for any real number variables
          * included in supplied expressions.
          *
          * NOTE - The rule container does not take ownership of these context objects. That
          *        is, it will NOT delete them. However, they do need to exist for the lifetime
          *        of the rule container. You should delete them after deleting the rule
          *        container object.
          *
          * @param The context to add
          */
        void addRealNumberContext( const IRealNumberContext* p_realNumContext );

        /**
          * addStringContext
          *
          * This method is used to add stringcontexts to each of the interlocking rules.
          * These contexts will be used to retrieve the values for any string variables
          * included in supplied expressions.
          *
          * NOTE - The rule container does not take ownership of these context objects. That
          *        is, it will NOT delete them. However, they do need to exist for the lifetime
          *        of the rule container. You should delete them after deleting the rule
          *        container object.
          *
          * @param The context to add
          */
        void addStringContext( const IStringContext* p_stringContext );

        /**
          * operator []
          *
          * This is used to access the rules stored in this container.
          * I could have created an iterator but it seemed like too much
          * work for the time allowed
          */
        TA_Base_Bus::InterlockingRule* operator[](unsigned int i);

        /**
          * size
          *
          * returns the number of rules stored in the container.
          *
          */
        unsigned int size(){ return m_rules.size(); }

        /**
          * loadRule
          */
        void loadRule(unsigned long ruleId);

        /**
          * reloadRule
          */
        void reloadRule( unsigned long p_entityKey, unsigned long ruleId );

    private:

        // Disable copy constructor and assignment operator
        RuleContainer( const RuleContainer& theRuleContainer);
        RuleContainer& operator=(const RuleContainer &);

        /**
          * loadRules
          */
        void loadRules();

        unsigned long m_entityKey;
        std::string m_entityType;

        // Maps entity keys to rules.
        std::multimap<unsigned long, unsigned long> m_entityKeyToRuleMap;
        std::vector<boost::shared_ptr<TA_Base_Bus::InterlockingRule> > m_rules;

        // The lock used to control access to this containter
        mutable TA_Base_Core::WriteReadThreadLockable m_lock;

    };
}
#endif // Bus_Layer_Interlocking_RuleContainer_H
