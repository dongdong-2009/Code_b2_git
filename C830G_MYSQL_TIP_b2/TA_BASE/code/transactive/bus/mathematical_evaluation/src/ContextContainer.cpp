/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/ContextContainer.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#include "bus/mathematical_evaluation/src/ContextContainer.h"
#include "bus/mathematical_evaluation/src/IBooleanContext.h"
#include "bus/mathematical_evaluation/src/IRealNumberContext.h"
#include "bus/mathematical_evaluation/src/IStringContext.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    //
    // Constructor
    //
    ContextContainer::ContextContainer()
    {}


    //
    // Destructor
    //
    ContextContainer::~ContextContainer()
    {}


    //
    // addBooleanContext
    //
    void ContextContainer::addBooleanContext( const TA_Base_Bus::IBooleanContext* p_context )
    {
        m_booleanContexts.push_back(p_context);
    }


    //
    // addRealNumberContext
    //
    void ContextContainer::addRealNumberContext( const TA_Base_Bus::IRealNumberContext* p_context )
    {
        m_realNumberContexts.push_back(p_context);
    }


    //
    // addStringContext
    //
    void ContextContainer::addStringContext( const TA_Base_Bus::IStringContext* p_context )
    {
        m_stringContexts.push_back(p_context);
    }


    //
    // getBooleanValue
    //
    bool ContextContainer::getBooleanValue(const std::string& name) const
    {
        std::vector<const TA_Base_Bus::IBooleanContext*>::const_iterator p = m_booleanContexts.begin();
        std::vector<const TA_Base_Bus::IBooleanContext*>::const_iterator end = m_booleanContexts.end();

        while( p != end )
        {
            try
            {
                bool value = (*p)->getValue(name);
                return value;
            }
            catch( TA_Base_Core::MathematicalEvaluationException )
            {
                ++p;
            }
        }

        std::string msg = name + " could not be found";

        TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));

        return false; // this line will never execute, it just keeps the compiler happy
    }


    //
    // getRealNumberValue
    //
    double ContextContainer::getRealNumberValue(const std::string& name) const
    {
        std::vector<const TA_Base_Bus::IRealNumberContext*>::const_iterator p = m_realNumberContexts.begin();
        std::vector<const TA_Base_Bus::IRealNumberContext*>::const_iterator end = m_realNumberContexts.end();

        while( p != end )
        {
            try
            {
                double value = (*p)->getValue(name);
                return value;
            }
            catch( TA_Base_Core::MathematicalEvaluationException )
            {
                ++p;
            }
        }

        std::string msg = name + " could not be found";

        TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));

        return 0.0; // this line will never execute, it just keeps the compiler happy
    }


    //
    // getStringValue
    //
    std::string ContextContainer::getStringValue(const std::string& name) const
    {
        std::vector<const TA_Base_Bus::IStringContext*>::const_iterator p = m_stringContexts.begin();
        std::vector<const TA_Base_Bus::IStringContext*>::const_iterator end = m_stringContexts.end();

        while( p != end )
        {
            try
            {
                std::string value = (*p)->getValue(name);
                return value;
            }
            catch( TA_Base_Core::MathematicalEvaluationException )
            {
                ++p;
            }
        }

        std::string msg = name + " could not be found";

        TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));

        return ""; // this line will never execute, it just keeps the compiler happy
    }


    //
    // isBooleanVariable
    //
    bool ContextContainer::isBooleanVariable(const std::string& name) const
    {
        try
        {
            bool tmp = getBooleanValue(name);
            return true;
        }
        catch( TA_Base_Core::MathematicalEvaluationException )
        {
            return false;
        }
    }


    //
    // isRealNumberVariable
    //
    bool ContextContainer::isRealNumberVariable(const std::string& name) const
    {
        try
        {
            double tmp = getRealNumberValue(name);
            return true;
        }
        catch( TA_Base_Core::MathematicalEvaluationException )
        {
            return false;
        }
    }


    //
    // isStringVariable
    //
    bool ContextContainer::isStringVariable(const std::string& name) const
    {
        try
        {
            std::string tmp = getStringValue(name);
            return true;
        }
        catch( TA_Base_Core::MathematicalEvaluationException )
        {
            return false;
        }
    }
}
