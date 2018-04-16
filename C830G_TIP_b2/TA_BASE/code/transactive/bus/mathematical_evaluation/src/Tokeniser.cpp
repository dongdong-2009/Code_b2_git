/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/Tokeniser.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/mathematical_evaluation/src/Tokeniser.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/utilities/src/DebugUtil.h"

#include <cctype>

namespace
{
    const std::string INTEGER_NUM       = "0123456789";

    //
    // isValidRealNumber
    //
    bool isValidRealNumber(const std::string& p_expression)
    {
        if( p_expression.find_first_of(INTEGER_NUM) != 0 )
        {
            return false;
        }

        std::string::size_type decimal = p_expression.find_first_not_of(INTEGER_NUM);

        if( decimal == std::string::npos )
        {
            return false;
        }

        if( p_expression[decimal] != '.' )
        {
            return false;
        }

        if( p_expression.find_first_of(INTEGER_NUM, decimal) != (decimal + 1) )
        {
            return false;
        }

        return true;
    }

    //
    // stringsAreEqual
    //
    bool stringsAreEqual(const std::string& string1, const std::string& string2)
    {
        if( string1.length() != string2.length() )
        {
            return false;
        }

        std::string::const_iterator it1=string1.begin();
        std::string::const_iterator it2=string2.begin();

        //stop when either string's end has been reached
        while ( (it1!=string1.end()) && (it2!=string2.end()) ) 
        { 
            if(::toupper(*it1) != ::toupper(*it2)) //letters differ?
            {
                return false;
            }

            ++it1;
            ++it2;
        }

        return true;
    }
};

namespace TA_Base_Bus
{
    const std::string INTEGER_NUM       = "0123456789";
    const std::string REAL_NUM          = "0123456789.";
    const std::string OPERATOR          = "*/+-^<>=";
    const std::string TRUE_CONSTANT     = "TRUE";
    const std::string FALSE_CONSTANT    = "FALSE";
    const std::string AND_CONSTANT      = "AND";
    const std::string OR_CONSTANT       = "OR";
    const std::string XOR_CONSTANT      = "XOR";
    const std::string NOT_CONSTANT      = "NOT";
    const std::string OPEN_PARENTHESIS  = "(";
    const std::string CLOSE_PARENTHESIS = ")";
    const std::string NOT_EQUAL_TO      = "!=";
    const std::string GREATER_OR_EQUAL  = ">=";
    const std::string LESS_OR_EQUAL     = "<=";
    const std::string SQRT_CONSTANT     = "SQRT";
	const std::string FABS_CONSTANT     = "FABS";

    //  
    // Constructor
    //
    Tokeniser::Tokeniser()
    {
    }


    //
    // Constructor
    //
    Tokeniser::Tokeniser(const std::string& p_expression)
    {
        parse(p_expression);
    }


    //
    // Destructor
    //
    Tokeniser::~Tokeniser()
    {
        clearTokens();
    }


    //
    // clearTokens
    //
    void Tokeniser::clearTokens()
    {
        for( unsigned int i = 0; i < m_tokens.size(); i++ )
        {
            if( m_tokens[i].get() != NULL )
            {
            }
        }
        m_tokens.clear();
    }


    //
    // getTokens
    //
    std::vector< boost::shared_ptr< TA_Base_Bus::Token > > Tokeniser::getTokens() const
    {
        return m_tokens;
    }


    //
    // setExpression
    //
    void Tokeniser::setExpression(const std::string& p_expression)
    {
        parse(p_expression);
    }


    //
    // extractNextOperator
    //
    unsigned int Tokeniser::extractNextOperator(const std::string& p_expression)
    {
        if( 0 == p_expression.find_first_of(OPERATOR, 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            std::string val = p_expression.substr(0, 1);
            
            // If the operator is a '-' and the previous token is an operator or open bracket,
            // it indicates a negative number rather than a subtract operator 
            if( (0 == val.compare("-")) && 
                    ( ( 0 == m_tokens.size() ) ||
                      ( (m_tokens[m_tokens.size()-1])->getType() == TA_Base_Bus::EETT_RealNumOperator) || 
                      ( (m_tokens[m_tokens.size()-1])->getType() == TA_Base_Bus::EETT_OpenParenthesis) ) )
            {
                if( isValidRealNumber(p_expression.substr(1, (p_expression.find_first_not_of(REAL_NUM, 1)))) )
                {
                    std::string num = p_expression.substr(0, (p_expression.find_first_not_of(REAL_NUM, 1)));
                    token->setValue(num);
                    token->setType(TA_Base_Bus::EETT_RealValue);
                    m_tokens.push_back(token);
                    return num.length();
                }
                else if( 1 == p_expression.find_first_of(INTEGER_NUM, 1) )
                {
                    std::string num = p_expression.substr(0, (p_expression.find_first_not_of(INTEGER_NUM, 1)));
                    token->setValue(num);
                    token->setType(TA_Base_Bus::EETT_IntegerValue);
                    m_tokens.push_back(token);
                    return num.length();
                }
                else
                {
                    TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression. Illegal positioning of '-' character"));
                }
            }

            // Count the number of operators to ensure there is no more than 20 in an expression.
            ++m_operatorCount;
			//TD 16521
			//zhou yuan++
			//remove the limitation of the expression count. 
//            if(m_operatorCount > 20)
//            {
//                TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid Expression. Expression contains more than 20 operators."));
//            }
			//++zhou yuan

            if( 0 == val.compare("=") )
            {
                token->setValue(val);
                token->setType(TA_Base_Bus::EETT_BooleanOperator);
                m_tokens.push_back(token);  
                return 1;
            }

            token->setValue(val);
            token->setType(TA_Base_Bus::EETT_RealNumOperator);
            m_tokens.push_back(token);
            return val.length();
        }

        return 0;
    }


    //
    // extractNextConstant
    //
    unsigned int Tokeniser::extractNextConstant(const std::string& p_expression)
    {
        if( (p_expression.length() >= NOT_EQUAL_TO.length()) &&
            stringsAreEqual(p_expression.substr(0, NOT_EQUAL_TO.length()), NOT_EQUAL_TO) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(NOT_EQUAL_TO));
            token->setType(TA_Base_Bus::EETT_BooleanOperator);
            m_tokens.push_back(token);
            return NOT_EQUAL_TO.length();
        }
        if( (p_expression.length() >= GREATER_OR_EQUAL.length()) &&
            stringsAreEqual(p_expression.substr(0, GREATER_OR_EQUAL.length()), GREATER_OR_EQUAL) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(GREATER_OR_EQUAL));
            token->setType(TA_Base_Bus::EETT_RealNumOperator);
            m_tokens.push_back(token);
            return GREATER_OR_EQUAL.length();
        }
        if( (p_expression.length() >= LESS_OR_EQUAL.length()) &&
            stringsAreEqual(p_expression.substr(0, LESS_OR_EQUAL.length()), LESS_OR_EQUAL) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(LESS_OR_EQUAL));
            token->setType(TA_Base_Bus::EETT_RealNumOperator);
            m_tokens.push_back(token);
            return LESS_OR_EQUAL.length();
        }
        if( (p_expression.length() >= SQRT_CONSTANT.length()) &&
            stringsAreEqual(p_expression.substr(0, SQRT_CONSTANT.length()), SQRT_CONSTANT) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(SQRT_CONSTANT));
            token->setType(TA_Base_Bus::EETT_RealNumOperator);
            m_tokens.push_back(token);
            return SQRT_CONSTANT.length();
        }
		if( (p_expression.length() >= FABS_CONSTANT.length()) &&
            stringsAreEqual(p_expression.substr(0, FABS_CONSTANT.length()), FABS_CONSTANT) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(FABS_CONSTANT));
            token->setType(TA_Base_Bus::EETT_RealNumOperator);
            m_tokens.push_back(token);
            return FABS_CONSTANT.length();
        }
        if( (p_expression.length() >= AND_CONSTANT.length()) &&
            stringsAreEqual(p_expression.substr(0, AND_CONSTANT.length()), AND_CONSTANT) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(AND_CONSTANT));
            token->setType(TA_Base_Bus::EETT_BooleanOperator);
            m_tokens.push_back(token);
            return AND_CONSTANT.length();
        }
        if( (p_expression.length() >= OR_CONSTANT.length()) &&
            stringsAreEqual(p_expression.substr(0, OR_CONSTANT.length()), OR_CONSTANT) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(OR_CONSTANT));
            token->setType(TA_Base_Bus::EETT_BooleanOperator);
            m_tokens.push_back(token);
            return OR_CONSTANT.length();
        }
        if( (p_expression.length() >= XOR_CONSTANT.length()) &&
            stringsAreEqual(p_expression.substr(0, XOR_CONSTANT.length()), XOR_CONSTANT) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(XOR_CONSTANT));
            token->setType(TA_Base_Bus::EETT_BooleanOperator);
            m_tokens.push_back(token);
            return XOR_CONSTANT.length();
        }
        if( (p_expression.length() >= NOT_CONSTANT.length()) &&
            stringsAreEqual(p_expression.substr(0, NOT_CONSTANT.length()), NOT_CONSTANT) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(NOT_CONSTANT));
            token->setType(TA_Base_Bus::EETT_BooleanOperator);
            m_tokens.push_back(token);
            return NOT_CONSTANT.length();
        }
        if( (p_expression.length() >= FALSE_CONSTANT.length()) &&
            stringsAreEqual(p_expression.substr(0, FALSE_CONSTANT.length()), FALSE_CONSTANT) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(FALSE_CONSTANT));
            token->setType(TA_Base_Bus::EETT_BooleanValue);
            m_tokens.push_back(token);
            return FALSE_CONSTANT.length();
        }
        if( (p_expression.length() >= TRUE_CONSTANT.length()) &&
            stringsAreEqual(p_expression.substr(0, TRUE_CONSTANT.length()), TRUE_CONSTANT) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(TRUE_CONSTANT));
            token->setType(TA_Base_Bus::EETT_BooleanValue);
            m_tokens.push_back(token);
            return TRUE_CONSTANT.length();
        }

        return 0;
    }


    //
    // extractNextNumber
    //
    unsigned int Tokeniser::extractNextNumber(const std::string& p_expression)
    {
        if( isValidRealNumber(p_expression.substr(0, (p_expression.find_first_not_of(REAL_NUM)))) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            std::string num = p_expression.substr(0, (p_expression.find_first_not_of(REAL_NUM)));
            token->setValue(num);
            token->setType(TA_Base_Bus::EETT_RealValue);
            m_tokens.push_back(token);
            return num.length();
        }
        if( 0 == p_expression.find_first_of(INTEGER_NUM, 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            std::string num = p_expression.substr(0, (p_expression.find_first_not_of(INTEGER_NUM)));
            token->setValue(num);
            token->setType(TA_Base_Bus::EETT_IntegerValue);
            m_tokens.push_back(token);
            return num.length();
        }

        return 0;
    }


    //
    // extractNextVariable
    //
    unsigned int Tokeniser::extractNextVariable(const std::string& p_expression)
    {
        if( 0 == p_expression.find_first_of('{', 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            unsigned long len = p_expression.find_first_of('}', 1);

            if( len == std::string::npos )
            {
                TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression, missing \' character"));
            }

            std::string val = p_expression.substr(1, len - 1);
            token->setValue(val);
            token->setType(TA_Base_Bus::EETT_Variable);
            m_tokens.push_back(token);
            return val.length() + 2;
        }

        return 0;
    }


    //
    // extractNextStringLiteral
    //
    unsigned int Tokeniser::extractNextStringLiteral(const std::string& p_expression)
    {
        if( 0 == p_expression.find_first_of('\'', 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            unsigned long len = p_expression.find_first_of('\'', 1);

            if( len == std::string::npos )
            {
                TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression, missing \' character"));
            }

            std::string val = p_expression.substr(1, len - 1);
            token->setValue(val);
            token->setType(TA_Base_Bus::EETT_StringLiteral);
            m_tokens.push_back(token);
            return val.length() + 2;
        }

        return 0;
    }


    //
    // extractNextParenthesis
    //
    unsigned int Tokeniser::extractNextParenthesis(const std::string& p_expression)
    {
        if( 0 == p_expression.find_first_of(OPEN_PARENTHESIS, 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(OPEN_PARENTHESIS));
            token->setType(TA_Base_Bus::EETT_OpenParenthesis);
            m_tokens.push_back(token);
            return 1;
        }
        if( 0 == p_expression.find_first_of(CLOSE_PARENTHESIS, 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(CLOSE_PARENTHESIS));
            token->setType(TA_Base_Bus::EETT_CloseParenthesis);
            m_tokens.push_back(token);
            return 1;
        }

        return 0;
    }


    //
    // parse
    //
    void Tokeniser::parse( const std::string& p_expression)
    {
        m_operatorCount = 0;

        clearTokens();

        unsigned long i = 0;

        while( i < p_expression.length() )
        {
            if( p_expression.find(" ", i) == i )
            {
                ++i;
            }
            else
            {
                unsigned long j = i;

                i += extractNextVariable(p_expression.substr(i));
                i += extractNextStringLiteral(p_expression.substr(i));
                i += extractNextParenthesis(p_expression.substr(i));
                i += extractNextConstant(p_expression.substr(i));
                i += extractNextOperator(p_expression.substr(i));
                i += extractNextNumber(p_expression.substr(i));
                if( i == j )
                {
                    TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression."));
                }
            }
        }
    }
}
