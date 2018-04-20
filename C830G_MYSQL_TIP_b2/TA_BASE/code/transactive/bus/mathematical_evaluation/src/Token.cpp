/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/Token.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * 
  */
#include "bus/mathematical_evaluation/src/Token.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"

const std::string TRUESTR = "TRUE";

namespace TA_Base_Bus
{

    //
    // setValue
    //
    void Token::setValue( const std::string& value )
    {
        m_value = value;
    }


    //
    // setType
    //
    void Token::setType( const E_ExpressionEvaluationTokenType type )
    {
        m_type = type;
    }


    //
    // getType
    //
    const E_ExpressionEvaluationTokenType Token::getType()
    {
        return m_type;
    }


    //
    // getBooleanOperator
    //
    const std::string Token::getBooleanOperator()
    {
        return m_value;
    }


    //
    // getRealNumOperator
    //
    const std::string Token::getRealNumOperator()
    {
        return m_value;
    }


    //
    // getRealValue
    //
    const double Token::getRealValue()
    {
        return atof(m_value.c_str());
    }


    //
    // getIntegerValue
    //
    const int Token::getIntegerValue()
    {
        return atoi(m_value.c_str());
    }


    //
    // getBooleanValue
    //
    const bool Token::getBooleanValue()
    {
        if( 0 == m_value.compare(TRUESTR) )
        {
            return true;
        }
        return false;
    }


    //
    // getStringLiteral
    //
    const std::string Token::getStringLiteral()
    {
        return m_value;
    }


    //
    // getVariable
    //
    const std::string Token::getVariable()
    {
        return m_value;
    }
}
