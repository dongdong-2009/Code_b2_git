/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/src/Interlocker.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */
#include <vector>

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif /* defined( WIN32 ) */

#include "bus/interlocking/src/Interlocker.h"
#include "bus/interlocking/src/InterlockingRule.h"
#include "bus/interlocking/src/RuleContainer.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/exceptions/src/InterlockingException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{

    //
    // Constructor
    //
    Interlocker::Interlocker()
    : m_ruleContainer(static_cast<RuleContainer*>(NULL))
    {
        try
        {
            m_ruleContainer = std::auto_ptr<RuleContainer>(new RuleContainer());
        }
        catch( const TA_Base_Core::InterlockingException& ie )
        {
            // If InterlockingException caught, we don't want to change
            // the message - just rethrow.

			LOG_EXCEPTION_CATCH(SourceInfo, "InterlockingException", ie.what());
            
            throw;
        }
        catch( ... )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught in Constructor");
			
			TA_THROW(TA_Base_Core::InterlockingException("Unable to construct Interlocker"));
        }
    }

    //
    // Constructor
    //
    Interlocker::Interlocker( const unsigned long p_entityKey, const std::string& p_entityType )
    : m_ruleContainer(static_cast<RuleContainer*>(NULL))
    {
        try
        {
            m_ruleContainer = std::auto_ptr<RuleContainer>(new RuleContainer( p_entityKey, p_entityType ));
        }
        catch( const TA_Base_Core::InterlockingException& ie )
        {
            // If InterlockingException caught, we don't want to change
            // the message - just rethrow.

			LOG_EXCEPTION_CATCH(SourceInfo, "InterlockingException", ie.what());
            
            throw;
        }
        catch( ... )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught in Constructor");
			
			TA_THROW(TA_Base_Core::InterlockingException("Unable to construct Interlocker"));
        }
    }


    //
    // Destructor
    //
    Interlocker::~Interlocker()
    {}


    //
    // addBooleanContext
    //
    void Interlocker::addBooleanContext( const IBooleanContext* p_booleanContext )
    {
        if (p_booleanContext != NULL )
        {
            m_ruleContainer->addBooleanContext(p_booleanContext);
        }
    }


    //
    // addRealNumberContext
    //
    void Interlocker::addRealNumberContext( const IRealNumberContext* p_realNumContext )
    {
        if ( p_realNumContext != NULL)
        {
            m_ruleContainer->addRealNumberContext(p_realNumContext);
        }
    }


    //
    // addStringContext
    //
    void Interlocker::addStringContext( const IStringContext* p_stringContext )
    {
        if ( p_stringContext != NULL )
        {
            m_ruleContainer->addStringContext(p_stringContext);
        }
    }


    //
    // permitStateChange
    //
    bool Interlocker::permitStateChange( unsigned long p_entityKey, const std::string& p_desiredState, const bool preCondition )
    {
        std::vector< boost::shared_ptr<TA_Base_Bus::InterlockingRule> > applicableRules = m_ruleContainer->getRulesForEntity(p_entityKey, p_desiredState, preCondition);

        for( std::vector< boost::shared_ptr<TA_Base_Bus::InterlockingRule> >::iterator p = applicableRules.begin();
             p != applicableRules.end();
             p++ )
        {
            if( (*p)->evaluate() == false )
            {
                return false;
            }
        }
        return true;
    }

    //
    // permitStateChange
    //
    bool Interlocker::permitStateChange( unsigned long p_entityKey, const std::string& p_action, const std::string& p_parameter, const bool preCondition )
    {
        std::vector< boost::shared_ptr<TA_Base_Bus::InterlockingRule> > applicableRules = m_ruleContainer->getRulesForEntity(p_entityKey, p_action, p_parameter, preCondition);

        for( std::vector< boost::shared_ptr<TA_Base_Bus::InterlockingRule> >::iterator p = applicableRules.begin();
             p != applicableRules.end();
             p++ )
        {
            if( (*p)->evaluate() == false )
            {
                return false;
            }
        }
        return true;
    }
}
