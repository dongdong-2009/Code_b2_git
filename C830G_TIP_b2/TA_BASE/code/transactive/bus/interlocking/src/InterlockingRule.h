#ifndef Bus_Layer_InterlockingRule_H
#define Bus_Layer_InterlockingRule_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/src/InterlockingRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#include <string>
#include <vector>
#include <map>
#include "InterlockingRuleTrigger.h"
#include "InterlockingRuleAlias.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"

namespace TA_Base_Bus
{

    class IBooleanContext;      // Forward declaration
    class IRealNumberContext;   // Forward declaration
    class IStringContext;       // Forward declaration
    
    /**
     * The Base class for an Interlocking rule.
     * 
     * Hold the rule itself as well as the trigger conditions and functions required
     * to evaluate the rule.
     */
    class InterlockingRule : public TA_Base_Bus::InterlockingRuleTrigger
    {
    public:

        /**
         *  Empty Constructor
         */
        InterlockingRule();

	    /**
	     * Trigger Constructor
	     * @param interlockingTrigger
	     * 
	     */
	    InterlockingRule(const InterlockingRuleTrigger& interlockingTrigger);

        /**
         * Full Constructor
         * @param interlockingTrigger
         * 
         */
        InterlockingRule::InterlockingRule(const TA_Base_Bus::InterlockingRuleTrigger& interlockingTrigger,
            int ruleId, const std::string& condition, const InterlockingRuleAliasVector& aliases);

        /**
          * Destructor
          */
        virtual ~InterlockingRule();

        /**
          * save
          *
          * writes this rule to the database
          */
        void save();

        /**
          * deleteMe
          *
          * removes this rule from the database
          */
        void deleteMe();

        /**
          * update
          *
          * updates this rule in the database
          */
        void update();

        /**
	     * setRuleId
         *
         * @param ruleId
	     */
	    void setRuleId(const unsigned int ruleId);

        /**
	     * getRuleId
         *
	     */
        unsigned int getRuleId() const;

       	/**
	     * getName
         *
         * @returns the rule name
	     */
	    std::string getName() const;

        /**
	     * setName
         *
         * @param name
	     */
	    void setName(const std::string& name);

       	/**
	     * getDescription
         *
         * return the rule description
         */
	    std::string getDescription() const;

        /**
	     * setDescription
         * @param description
	     * 
	     */
	    void setDescription(const std::string& description);

       	/**
	     * getCondition
         *
         * @returns the condition to be checked
	     */
        std::string getCondition() const;

       	/**
	     * setCondition
	     * @param p_condition
	     * 
	     */
        void setCondition(const std::string& p_condition);

        /**
         * get Aliases
         *
         * @returns the list alaises for this rule 
         */
        InterlockingRuleAliasVector getAliases();

        /**
         * set Aliases
         * 
         * @param aliasList the list of aliases for this rule
         */
        void setAliases(const InterlockingRuleAliasVector& aliasList);

        /**
         * add Alias
         * 
         * @param alias An alias to add to the list of aliases
         */
        void addAlias(const boost::shared_ptr<InterlockingRuleAlias>& alias);

       	/**
	     * evaluate
         *
         * @returns true if the rule condition evaluates to true
	     */
        bool evaluate();

        /**
          * loadContexts
          *
          * This will find and load all contexts that are avilable
          * Any missing contexts must be loaded by the client before evaluate
          * can be called.
          */
        void loadContexts();

        /**
          * loadContextFromDll
          *
          * This method will get any contexts from the named dll and
          * add them to the list.
          *
          * @param dllName The name of the dll to examine
          */
        void loadContextFromDll( const std::string& dllName );
 
        /**
          * addBooleanContext
          *
          * This method is used to add boolean contexts to the interlock rule.
          * These contexts will be passed on to the expression evaluator. The
		  * context objects need to exist for the life time of the RuleContainer.
          *
          * @param The context to add
          */
        void addBooleanContext( const IBooleanContext* p_booleanContext );

       	/**
          * addRealNumberContext
          *
          * This method is used to add real number contexts to the interlock rule.
          * These contexts will be passed on to the expression evaluator. The
		  * context objects need to exist for the life time of the RuleContainer.
          *
          * @param The context to add
	     */
        void addRealNumberContext( const IRealNumberContext* p_realNumContext );

     	/**
          * addStringContext
          *
          * This method is used to add string contexts to the interlock rule.
          * These contexts will be passed on to the expression evaluator. The
		  * context objects need to exist for the life time of the RuleContainer.
          *
          * @param The context to add
	     */
        void addStringContext( const IStringContext* p_stringContext );

    private:
        // Disable copy constructor and assignment operator
        InterlockingRule( const InterlockingRule& theInterlockingRule);
        InterlockingRule& operator=(const InterlockingRule &);

        // Internal unique id for this rule
	    unsigned int m_ruleId;

        // Name of this rule
        std::string m_name;

        // A long description for this rule
        std::string m_description;

        // the list of aliases
        InterlockingRuleAliasVector m_aliasList;

        // The condition that must evaluate to TRUE for the state transition to be permitted.
        std::string m_condition;

        // The expression evaluator.
        TA_Base_Bus::ExpressionEvaluator m_expressionEvaluator;

        // The list of variable types
        std::map<std::string, int> m_variableTypes;

    };
}
#endif // Bus_Layer_InterlockingRule_H
