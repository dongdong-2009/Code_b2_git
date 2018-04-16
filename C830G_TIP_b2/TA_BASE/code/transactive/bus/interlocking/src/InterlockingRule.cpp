/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/src/InterlockingRule.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif /* defined( WIN32 ) */

#include "ace/DLL.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/interlocking/src/InterlockingRule.h"
#include "core/data_access_interface/interlocking/src/ConfigInterlockRule.h"
#include "core/data_access_interface/interlocking/src/InterlockAlias.h"
#include "core/data_access_interface/interlocking/src/InterlockRuleAccessFactory.h"
#include "core/data_access_interface/interlocking_variable/src/IInterlockVariable.h"
#include "core/data_access_interface/interlocking_variable/src/InterlockVariableAccessFactory.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{


    //
    // Empty Constructor
    //
    InterlockingRule::InterlockingRule()
        : TA_Base_Bus::InterlockingRuleTrigger(),
    	  m_ruleId(0),
          m_name(""),
          m_description(""),
          m_aliasList(0),
          m_condition(""),
          m_expressionEvaluator()
    {
        m_variableTypes.clear();
    }


    InterlockingRule::InterlockingRule(const TA_Base_Bus::InterlockingRuleTrigger& interlockingTrigger)    
        : TA_Base_Bus::InterlockingRuleTrigger(interlockingTrigger),
    	  m_ruleId(0),
          m_name(""),
          m_description(""),
          m_aliasList(0),
          m_condition("")
    {
        m_variableTypes.clear();
        m_expressionEvaluator.setNewExpression(m_condition);
    }

    InterlockingRule::InterlockingRule(const TA_Base_Bus::InterlockingRuleTrigger& interlockingTrigger,
        int ruleId, const std::string& condition, const InterlockingRuleAliasVector& aliases)
        : TA_Base_Bus::InterlockingRuleTrigger(interlockingTrigger),
    	  m_ruleId(ruleId),
          m_name(""),
          m_description(""),
          m_aliasList(0),
          m_condition(condition)
    {
        m_variableTypes.clear();
        m_expressionEvaluator.setNewExpression(m_condition);
    }

    //
    // Destructor
    //
    InterlockingRule::~InterlockingRule()
    {}


    void InterlockingRule::update()
    {
        TA_Base_Core::ConfigInterlockRule* rulePointer = NULL;
        if ( m_ruleId == 0 )
        {
            rulePointer = dynamic_cast<TA_Base_Core::ConfigInterlockRule*>(TA_Base_Core::InterlockRuleAccessFactory::getInstance().createInterlockRule());
        }
        else
        {
            rulePointer = dynamic_cast<TA_Base_Core::ConfigInterlockRule*>(TA_Base_Core::InterlockRuleAccessFactory::getInstance().getInterlockRule(m_ruleId, true));
        }
        TA_ASSERT(rulePointer != NULL, "Cast failed");

        rulePointer->setEntityKey(this->getEntityKey());
        rulePointer->setEntityType(this->getEntityType());
        rulePointer->setDesiredState(this->getEntityAction());
        rulePointer->setParameters(this->getActionParameters());
        rulePointer->setEnabled(this->getEnabled());
        rulePointer->setPreCondition(this->getPreCondition());
        rulePointer->setName(m_name);
        rulePointer->setDescription(m_description);
        rulePointer->setCondition(m_condition);

        // copy alias list
        std::vector<boost::shared_ptr<TA_Base_Core::IInterlockAlias> > aliases;

        for (InterlockingRuleAliasVector::iterator ai = m_aliasList.begin(); ai != m_aliasList.end(); ai++)
        {
            boost::shared_ptr<TA_Base_Core::IInterlockAlias> alias( new TA_Base_Core::InterlockAlias( 0 ));
//            TA_ASSERT(alias != NULL, "Failed to allocate pointer");

            if ( m_ruleId != 0 )
            {
                alias->setRuleId( rulePointer->getKey() );
            }
            alias->setAlias((*ai)->getAliasName());
            alias->setVariableName((*ai)->getVariableName());
            alias->setVariableType((*ai)->getVariableType());
            aliases.push_back(alias);
        }
        rulePointer->setAliases( aliases );

        rulePointer->save();
        
        if ( m_ruleId == 0 )
        {
            m_ruleId = rulePointer->getKey();
        }
    }

    std::string InterlockingRule::getName() const
    {
        return m_name;
    }


    void InterlockingRule::setName(const std::string& p_name)
    {
        m_name = p_name;
    }


    std::string InterlockingRule::getDescription() const
    {
        return m_description;
    }


    void InterlockingRule::setDescription(const std::string& p_description)
    {
        m_description = p_description;
    }


    std::string InterlockingRule::getCondition() const
    {
        return m_condition;
    }


    void InterlockingRule::setCondition(const std::string& p_condition)
    {
        m_condition = p_condition;
        m_expressionEvaluator.setNewExpression(m_condition);
    }


    InterlockingRuleAliasVector InterlockingRule::getAliases()
    {
        return m_aliasList;
    }

    void InterlockingRule::setAliases(const InterlockingRuleAliasVector& aliasList)
    {
        m_aliasList.clear();
        m_aliasList = aliasList;
    }

    void InterlockingRule::addAlias(const boost::shared_ptr<InterlockingRuleAlias>& alias)
    {
        m_aliasList.push_back(alias);
    }

    void InterlockingRule::loadContexts()
    {
        // spin through all known variables and load context if not already loaded
        for ( InterlockingRuleAliasVector::iterator ialias = m_aliasList.begin(); ialias != m_aliasList.end(); ialias++)
        {
            std::map<std::string, int>::iterator itype;

            // do we know about this one yet?
            std::string type( (*ialias)->getVariableType() );
            itype = m_variableTypes.find( type );
            if ( itype == m_variableTypes.end() )
            {
                // a new type! load and add context
                TA_Base_Core::IInterlockVariable* ivariable = NULL;
                try
                {
                    ivariable = TA_Base_Core::InterlockVariableAccessFactory::getInstance().getInterlockVariable( type );
                }
                catch( ... )
                {
                    // don't really care about errors here
                    // if there is a problem we just won't load anything
                    // this also provides backward compatibility where the client
                    // is expected to load the contexts
                }
                if ( ivariable == NULL )
                {
                    continue;
                }
                
                std::string dllName = ivariable->getDllName();
                delete(ivariable);
                ivariable = NULL;

                loadContextFromDll( dllName );
                m_variableTypes.insert( std::pair<std::string,int>(dllName, 1) );
            }
        }

    }

    void InterlockingRule::loadContextFromDll( const std::string& dllName )
    {
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Loading dynamic link library %s", dllName.c_str() );

        // Load the DLL
        ACE_DLL libraryInstance = ACE_DLL ();
        if ( libraryInstance.open( dllName.c_str(), ACE_DEFAULT_SHLIB_MODE, 0 ) == -1 )
        {
            // error opening dll
            return;
        }

        // Load the context functions.
        typedef IStringContext* (*pfnStringContext)();
        typedef IBooleanContext* (*pfnBooleanContext)();
        typedef IRealNumberContext* (*pfnRealNumberContext)();

        pfnStringContext DllStringContext = NULL;
        pfnBooleanContext DllBooleanContext = NULL;
        pfnRealNumberContext DllRealNumberContext = NULL;

        // The '1' as the second parameter means ingore any errors. See ACE docs for more infomation.
        DllStringContext = (pfnStringContext) libraryInstance.symbol( "createStringContext", 1 );
        DllBooleanContext = (pfnBooleanContext) libraryInstance.symbol( "createBooleanContext", 1 );
        DllRealNumberContext = (pfnRealNumberContext) libraryInstance.symbol( "createRealNumberContext", 1 );

        // Store all provided contexts
        // NOTE: It's OK if a context is NULL. It just means that there is no context of that type for this variable type
        if ( DllStringContext != NULL )
        {
            addStringContext( DllStringContext() );
        }
        if ( DllBooleanContext != NULL )
        {
            addBooleanContext( DllBooleanContext() );
        }
        if ( DllRealNumberContext != NULL )
        {
            addRealNumberContext( DllRealNumberContext() );
        }
    }

    void InterlockingRule::addBooleanContext( const IBooleanContext* p_booleanContext )
    {
        m_expressionEvaluator.addBooleanContext(p_booleanContext);
    }


    void InterlockingRule::addRealNumberContext( const IRealNumberContext* p_realNumContext )
    {
        m_expressionEvaluator.addRealNumberContext(p_realNumContext);
    }

    void InterlockingRule::addStringContext( const IStringContext* p_stringContext )
    {
        m_expressionEvaluator.addStringContext(p_stringContext);
    }

    bool InterlockingRule::evaluate()
    {
        return m_expressionEvaluator.evaluateBooleanExpression();
    }


    void InterlockingRule::setRuleId(const unsigned int ruleId)
    {
        m_ruleId = ruleId;
    }


    unsigned int InterlockingRule::getRuleId() const
    {
        return m_ruleId;
    }

    void InterlockingRule::deleteMe()
    {
        TA_Base_Core::ConfigInterlockRule* rulePointer = NULL;
        rulePointer = dynamic_cast<TA_Base_Core::ConfigInterlockRule*>(TA_Base_Core::InterlockRuleAccessFactory::getInstance().getInterlockRule(m_ruleId, true));

        TA_ASSERT(rulePointer != NULL, "Cast failed");

        try
        {
            rulePointer->deleteThisObject();
        }
        catch(...)
        {
            delete(rulePointer);
            rulePointer = NULL;
            TA_ASSERT(false, "Failed to delete rule");
        }
        delete(rulePointer);
        rulePointer = NULL;
    }
} // namespace
