/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/EqualityComparison.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * EqualityComparison represent the real number equality operation.
  */
#include <memory>

#include "bus/mathematical_evaluation/src/EqualityComparison.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{
    EqualityComparison::EqualityComparison(std::auto_ptr< IRealNumberExpression > leftHandSide,
                                           std::auto_ptr< IRealNumberExpression > rightHandSide)
    :m_realNumberRighthandSide(rightHandSide), m_realNumberLefthandSide(leftHandSide),
     m_booleanLefthandSide(NULL), m_booleanRighthandSide(NULL),
     m_stringLefthandSide(NULL), m_stringRighthandSide(NULL)
    {
        TA_ASSERT(m_realNumberLefthandSide.get()  != NULL, "leftHandSide is null!");
        TA_ASSERT(m_realNumberRighthandSide.get() != NULL, "rightHandSide is null!");
    }


    EqualityComparison::EqualityComparison(std::auto_ptr< IBooleanExpression > leftHandSide,
                                           std::auto_ptr< IBooleanExpression > rightHandSide)
    :m_realNumberRighthandSide(NULL), m_realNumberLefthandSide(NULL),
     m_booleanLefthandSide(leftHandSide), m_booleanRighthandSide(rightHandSide),
     m_stringLefthandSide(NULL), m_stringRighthandSide(NULL)
    {
        TA_ASSERT(m_booleanLefthandSide.get()  != NULL, "LeftHandSide is null!");
        TA_ASSERT(m_booleanRighthandSide.get() != NULL, "rightHandSide is null!");
    }


    EqualityComparison::EqualityComparison(std::auto_ptr< IStringExpression > leftHandSide,
                                           std::auto_ptr< IStringExpression > rightHandSide)
    :m_realNumberRighthandSide(NULL), m_realNumberLefthandSide(NULL),
     m_booleanLefthandSide(NULL), m_booleanRighthandSide(NULL),
     m_stringLefthandSide(leftHandSide), m_stringRighthandSide(rightHandSide)
    {
        TA_ASSERT(m_stringLefthandSide.get()  != NULL, "leftHandSide is null!");
        TA_ASSERT(m_stringRighthandSide.get() != NULL, "rightHandSide is null!");
    }


    EqualityComparison::~EqualityComparison()
    {
        m_realNumberRighthandSide = std::auto_ptr< IRealNumberExpression >(static_cast< IRealNumberExpression* >(NULL));
        m_realNumberLefthandSide  = std::auto_ptr< IRealNumberExpression >(static_cast< IRealNumberExpression* >(NULL));
        m_booleanRighthandSide    = std::auto_ptr< IBooleanExpression >(static_cast< IBooleanExpression* >(NULL));
        m_booleanLefthandSide     = std::auto_ptr< IBooleanExpression >(static_cast< IBooleanExpression* >(NULL));
        m_stringRighthandSide     = std::auto_ptr< IStringExpression >(static_cast< IStringExpression* >(NULL));
        m_stringLefthandSide      = std::auto_ptr< IStringExpression >(static_cast< IStringExpression* >(NULL));
    }


    bool EqualityComparison::evaluate() const
    {
        if( (m_realNumberRighthandSide.get() != NULL) && (m_realNumberLefthandSide.get() != NULL) )
        {
            return m_realNumberRighthandSide->evaluate() == m_realNumberLefthandSide->evaluate();
        }

        if( (m_booleanRighthandSide.get() != NULL) && (m_booleanLefthandSide.get() != NULL) )
        {
            return m_booleanRighthandSide->evaluate() == m_booleanLefthandSide->evaluate();
        }

        if( (m_stringRighthandSide.get() != NULL) && (m_stringLefthandSide.get() != NULL) )
        {
            if( 0 == m_stringRighthandSide->evaluate().compare(m_stringLefthandSide->evaluate()) )
            {
                return true; // strings are equal
            }

            return false;
        }

        return false; // This is never executed, keeps compiler happy
    }
}
