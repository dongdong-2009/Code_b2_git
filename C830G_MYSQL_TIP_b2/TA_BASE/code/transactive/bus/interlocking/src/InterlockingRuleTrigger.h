/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/src/InterlockingRuleTrigger.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if !defined(InterlockingRuleTrigger_EE026C72_A8BF_4120_8A92_EF252641D08A__INCLUDED_)
#define InterlockingRuleTrigger_EE026C72_A8BF_4120_8A92_EF252641D08A__INCLUDED_

#include <string>
#include <vector>

namespace TA_Base_Bus
{
    /**
     * Holds those attributes of an InterlockingRule that are releated to the trigger
     * condition. Used to determine if a rule should be applied.
     */
    class InterlockingRuleTrigger
    {

    public:
	    InterlockingRuleTrigger();
	    virtual ~InterlockingRuleTrigger();

    public:
	    /**
	     * setEntityType
         *
         * @param entityType
	     * 
	     */
	    void setEntityType(const std::string& entityType);
	    /**
	     * setEntityKey
         *
         * @param entityKey
	     * 
	     */
	    void setEntityKey(const int entityKey);
	    /**
	     * setEntityAction
         * 
         * @param action
	     * 
	     */
	    void setEntityAction(const std::string& action);
	    /**
	     * addActionParameter
         *
         * @param parameter
	     * 
	     */
	    void addActionParameter(const std::string& parameter);
	    /**
	     * setActionParameters
         *
         * @param parameter
	     * 
	     */
        void setActionParameters(const std::vector<std::string>& parameter);
	    /**
	     * setPrecondition
         *
         * @param precondition
	     * 
	     */
	    void setPrecondition(const bool precondition);
	    /**
	     * setEnabled
         *
         * @param enabled
	     * 
	     */
	    void setEnabled(const bool enabled);
	    /**
	     * setEntityName
         *
         * @param name
	     * 
	     */
	    void setEntityName(const std::string& name);

	    std::string getEntityType();
	    int getEntityKey();
	    std::string getEntityAction();
        std::vector<std::string> getActionParameters();
	    bool getPreCondition();
	    bool getEnabled();
	    std::string getEntityName();

    private:
	    std::string m_entityName;
	    std::string m_entityType;
	    int m_entityKey;
	    std::string m_entityAction;
        std::vector<std::string> m_actionParameters;
	    /**
	     * true = pre
	     * false = post condition.
	     */
	    bool m_precondition;
	    bool m_enabled;

    };
} //namespace
#endif // !defined(InterlockingRuleTrigger_EE026C72_A8BF_4120_8A92_EF252641D08A__INCLUDED_)

