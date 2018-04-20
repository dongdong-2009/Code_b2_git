/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/src/InterlockingRuleTrigger.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#include "InterlockingRuleTrigger.h"

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif /* defined( WIN32 ) */

namespace TA_Base_Bus
{
    InterlockingRuleTrigger::InterlockingRuleTrigger()
        : m_entityName(""),
    	  m_entityType(""),
	      m_entityKey(0),
	      m_entityAction(""),
	      m_precondition(true),
	      m_enabled(true)
    {
	    m_actionParameters.clear();
    }



    InterlockingRuleTrigger::~InterlockingRuleTrigger()
    {
    }



    /**
     * @param entityType
     * 
     */
    void InterlockingRuleTrigger::setEntityType(const std::string& entityType)
    {
        m_entityType = entityType;
    }


    /**
     * @param entityKey
     * 
     */
    void InterlockingRuleTrigger::setEntityKey(const int entityKey)
    {
        m_entityKey = entityKey;
    }


    /**
     * @param action
     * 
     */
    void InterlockingRuleTrigger::setEntityAction(const std::string& action)
    {
        m_entityAction = action;
    }


    /**
     * @param parameter
     * 
     */
    void InterlockingRuleTrigger::setActionParameters(const std::vector<std::string>& parameters)
    {
        m_actionParameters = parameters;
    }


    /**
     * @param parameter
     * 
     */
    void InterlockingRuleTrigger::addActionParameter(const std::string& parameter)
    {
        m_actionParameters.push_back(parameter);
    }


    /**
     * @param precondition
     * 
     */
    void InterlockingRuleTrigger::setPrecondition(const bool precondition)
    {
        m_precondition = precondition;
    }


    /**
     * @param enabled
     * 
     */
    void InterlockingRuleTrigger::setEnabled(const bool enabled)
    {
        m_enabled = enabled;
    }


    /**
     * @param name
     * 
     */
    void InterlockingRuleTrigger::setEntityName(const std::string& name)
    {
        m_entityName = name;
    }


    std::string InterlockingRuleTrigger::getEntityType()
    {
        return m_entityType;
    }


    int InterlockingRuleTrigger::getEntityKey()
    {
        return m_entityKey;
    }


    std::string InterlockingRuleTrigger::getEntityAction()
    {
        return m_entityAction;
    }


    std::vector<std::string> InterlockingRuleTrigger::getActionParameters()
    {
        return m_actionParameters;
    }


    bool InterlockingRuleTrigger::getPreCondition()
    {
        return m_precondition;    
    }


    bool InterlockingRuleTrigger::getEnabled()
    {
        return m_enabled;
    }


    std::string InterlockingRuleTrigger::getEntityName()
    {
        return m_entityName;
    }


} //namespace

