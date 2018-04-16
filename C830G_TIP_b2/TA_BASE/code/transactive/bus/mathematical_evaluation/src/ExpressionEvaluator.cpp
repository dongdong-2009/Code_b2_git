/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/ExpressionEvaluator.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "bus/mathematical_evaluation/src/BooleanConstant.h"
#include "bus/mathematical_evaluation/src/BooleanVariable.h"
#include "bus/mathematical_evaluation/src/AndExpression.h"
#include "bus/mathematical_evaluation/src/OrExpression.h"
#include "bus/mathematical_evaluation/src/XorExpression.h"
#include "bus/mathematical_evaluation/src/NotExpression.h"
#include "bus/mathematical_evaluation/src/RealNumberConstant.h"
#include "bus/mathematical_evaluation/src/RealNumberAddExpression.h"
#include "bus/mathematical_evaluation/src/RealNumberDivideExpression.h"
#include "bus/mathematical_evaluation/src/RealNumberGreaterThanComparison.h"
#include "bus/mathematical_evaluation/src/RealNumberLessThanComparison.h"
#include "bus/mathematical_evaluation/src/RealNumberGreaterThanOrEqualComparison.h"
#include "bus/mathematical_evaluation/src/RealNumberLessThanOrEqualComparison.h"
#include "bus/mathematical_evaluation/src/EqualityComparison.h"
#include "bus/mathematical_evaluation/src/InequalityComparison.h"
#include "bus/mathematical_evaluation/src/RealNumberMultiplyExpression.h"
#include "bus/mathematical_evaluation/src/RealNumberPowerOfExpression.h"
#include "bus/mathematical_evaluation/src/RealNumberSquareRootExpression.h"
#include "bus/mathematical_evaluation/src/RealNumberAbsoluteExpression.h"
#include "bus/mathematical_evaluation/src/RealNumberSubtractExpression.h"
#include "bus/mathematical_evaluation/src/RealNumberVariable.h"
#include "bus/mathematical_evaluation/src/StringLiteral.h"
#include "bus/mathematical_evaluation/src/StringVariable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace
{
    const std::string LESSTHAN           = "<";
    const std::string GREATERTHAN        = ">";
    const std::string GREATERTHANOREQUAL = ">=";
    const std::string LESSTHANOREQUAL    = "<=";
    const std::string EQUAL              = "=";
    const std::string NOTEQUAL           = "!=";
    const std::string AND                = "AND";
    const std::string OR                 = "OR";
    const std::string XOR                = "XOR";
    const std::string NOT                = "NOT";
    const std::string MULTIPLY           = "*";
    const std::string DIVIDE             = "/";
    const std::string ADDITION           = "+";
    const std::string SUBTRACTION        = "-";
    const std::string POWER              = "^";
    const std::string SQRT               = "SQRT";
	const std::string FABS				 = "FABS";

    class OperatorPrecedence
    {
        public:
            OperatorPrecedence()
            {
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(POWER             ,0)); // Highest precedence
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(SQRT              ,1));
				m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(FABS              ,2)); 
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(NOT               ,3));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(MULTIPLY          ,4));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(DIVIDE            ,5));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(ADDITION          ,6));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(SUBTRACTION       ,7));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(GREATERTHAN       ,8));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(LESSTHAN          ,9));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(GREATERTHANOREQUAL,10));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(LESSTHANOREQUAL   ,11));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(EQUAL             ,12));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(NOTEQUAL          ,13));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(AND               ,14));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(OR                ,15));
                m_operatorPrecedenceLookup.insert(std::pair<std::string, unsigned long>(XOR               ,16)); // Lowest precedence
            }

            bool isOperatorOfLowerPrecedence( const std::string& p_operator,
                                              const std::string& p_operatorToCompareTo ) const
            {
                std::map<std::string, unsigned long>::const_iterator p1 = m_operatorPrecedenceLookup.find(p_operator);
                std::map<std::string, unsigned long>::const_iterator p2 = m_operatorPrecedenceLookup.find(p_operatorToCompareTo);

                if( (p1 == m_operatorPrecedenceLookup.end() ) || ( p2 == m_operatorPrecedenceLookup.end() ) )
                {
                    TA_THROW(TA_Base_Core::MathematicalEvaluationException("Operators could not be compared for precedence - operator was invalid"));
                }

                if( p1->second > p2->second )
                {
                    return true;
                }

                return false;
            }

        private:
            std::map<std::string, unsigned long> m_operatorPrecedenceLookup;
    };


}

namespace TA_Base_Bus
{

    class ReadLockHelper
    {
        public:

        ReadLockHelper( TA_Base_Bus::ExpressionEvaluator& expr) : m_objectToLock(expr)
        {
            m_objectToLock.aquireReadLock();
        }

        ~ReadLockHelper()
        {
            m_objectToLock.releaseReadLock();
        }

        private:

        // Disable copy constructor and assignment operator.
        ReadLockHelper( const ReadLockHelper& theReadLockHelper);
        ReadLockHelper& operator=(const ReadLockHelper&);

        TA_Base_Bus::ExpressionEvaluator& m_objectToLock;
    };

    //
    // Constructor
    //
    ExpressionEvaluator::ExpressionEvaluator()
    : m_contexts(), m_tokeniser(), m_readCount(0), m_initialLen(0)
    {
    }


    //
    // Constructor
    //
    ExpressionEvaluator::ExpressionEvaluator( const std::string& p_expression )
    : m_contexts(), m_tokeniser(), m_readCount(0), m_initialLen(0)
    {
        try
        {
            m_tokeniser.setExpression(p_expression);
        }
        catch( TA_Base_Core::MathematicalEvaluationException )
        {
            throw;
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception raised.", "Supplied expression could not be parsed." );
            TA_THROW(TA_Base_Core::MathematicalEvaluationException("Unknown exception raised - supplied expression could not be parsed"));
        }
    }


    //
    // Destructor
    //
    ExpressionEvaluator::~ExpressionEvaluator()
    {
    }


    //
    // setNewExpression
    //
    void ExpressionEvaluator::setNewExpression(const std::string& p_expression)
    {
        TA_Base_Core::ThreadGuard guard( m_writeLock );

        try
        {
            m_tokeniser.setExpression(p_expression);
        }
        catch( TA_Base_Core::MathematicalEvaluationException )
        {
            throw;
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception raised.", "Supplied expression could not be parsed." );
            TA_THROW(TA_Base_Core::MathematicalEvaluationException("Unknown exception raised - supplied expression could not be parsed"));
        }
    }


    //
    // findLowestPrecedenceOperatorAtSameLevel
    //
    int ExpressionEvaluator::findLowestPrecedenceOperatorAtSameLevel( unsigned int start, unsigned int length )
    {
        unsigned int currentNestingLevel = 0;
        int indexOfLowestSoFar = -1;
        OperatorPrecedence operatorPrecedence;

        TA_ASSERT( !(m_tokens.size() < (start + length)), "Invalid size.");

        for( unsigned long i = start; i < (start + length); i++ )
        {
            TA_Base_Bus::E_ExpressionEvaluationTokenType type = (m_tokens[i])->getType();

            switch( type )
            {
                case( EETT_RealNumOperator ) :
                {
                    if( 0 == currentNestingLevel )
                    {
                        if( -1 == indexOfLowestSoFar )
                        {
                            indexOfLowestSoFar = i;
                        }
                        else
                        {
                            if( operatorPrecedence.isOperatorOfLowerPrecedence(
                                (m_tokens[i])->getRealNumOperator(), (m_tokens[indexOfLowestSoFar])->getRealNumOperator()) )
                            {
                                indexOfLowestSoFar = i;
                            }
                        }
                    }
                    break;
                };
                case( EETT_BooleanOperator ) :
                {
                    if( 0 == currentNestingLevel )
                    {
                        if( -1 == indexOfLowestSoFar )
                        {
                            indexOfLowestSoFar = i;
                        }
                        else
                        {
                            if( operatorPrecedence.isOperatorOfLowerPrecedence(
                                (m_tokens[i])->getBooleanOperator(), (m_tokens[indexOfLowestSoFar])->getBooleanOperator()) )
                            {
                                indexOfLowestSoFar = i;
                            }
                        }
                    }
                    break;
                };
                case( EETT_OpenParenthesis ) :
                {
                    ++currentNestingLevel;
                    break;
                };
                case( EETT_CloseParenthesis ) :
                {
                    if( 0 == currentNestingLevel )
                    {
                        return indexOfLowestSoFar;
                    }
                    else
                    {
                        --currentNestingLevel;
                    }
                    break;
                };
                default :
                {
                    break;
                };
            }
        }
        
        return indexOfLowestSoFar;
    }


    //
    // generateBooleanExpression
    //
    std::auto_ptr< IBooleanExpression > ExpressionEvaluator::generateBooleanExpression(unsigned int start, unsigned int length)
    {
        TA_ASSERT( !(m_tokens.size() < (start + length)), "Invalid size.");

        if( length > 1 )
        {
			if (m_initialLen == 0) m_initialLen = length; // initialise the initial length when this function is first called;

            int nextOperator = findLowestPrecedenceOperatorAtSameLevel(start, length);

            if( -1 == nextOperator )
            {
                if( (m_tokens[start])->getType() == EETT_OpenParenthesis )
                {
                    return generateBooleanExpression( (start + 1), (length - 2) );
                }
                else
                {
                    TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression - missing operator or open bracket"));
                }
            }

            std::string operatorType = (m_tokens[nextOperator])->getBooleanOperator();

			// TD 11934 The length has to be incremented, as the start is always index 0
			// This is a case of an operator being defined at the end of expression ie. invalid
			if ( m_initialLen == (nextOperator + 1) && operatorType.empty()==false )
			{
				// This is an invalid expression
                TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression - missing operator or open bracket"));
			}

            if( 0 == operatorType.compare(LESSTHAN) )
            {
                return std::auto_ptr< IBooleanExpression >( new RealNumberLessThanComparison( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }                 

            if( 0 == operatorType.compare(GREATERTHAN) )
            {
                return std::auto_ptr< IBooleanExpression >( new RealNumberGreaterThanComparison( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }           

            if( 0 == operatorType.compare(GREATERTHANOREQUAL) )
            {
                return std::auto_ptr< IBooleanExpression >( new RealNumberGreaterThanOrEqualComparison( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }           

            if( 0 == operatorType.compare(LESSTHANOREQUAL) )
            {
                return std::auto_ptr< IBooleanExpression >( new RealNumberLessThanOrEqualComparison( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }           

            if( 0 == operatorType.compare(EQUAL) )
            {
                try
                {
                    return std::auto_ptr< IBooleanExpression >( new EqualityComparison( generateBooleanExpression(start, (nextOperator - start)),generateBooleanExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
                }
                catch( const TA_Base_Core::MathematicalEvaluationException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "MathematicalEvaluationException", e.what() );
                    // There is no specific ordering for whether the real number or string expression is constructed first.
                    try
                    {
                        return std::auto_ptr< IBooleanExpression >( new EqualityComparison( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
                    }
                    catch( const TA_Base_Core::MathematicalEvaluationException& )
                    {
                        return std::auto_ptr< IBooleanExpression >( new EqualityComparison( generateStringExpression(start, (nextOperator - start)),generateStringExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
                    }
                }
            }           

            if( 0 == operatorType.compare(NOTEQUAL) )
            {
                try
                {
	                return std::auto_ptr< IBooleanExpression > (new InequalityComparison( generateBooleanExpression(start, (nextOperator - start)),generateBooleanExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
                }
                catch( const TA_Base_Core::MathematicalEvaluationException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "MathematicalEvaluationException", e.what() );
                    // There is no specific ordering for whether the real number or string expression is constructed first.
                    try
                    {
						return std::auto_ptr< IBooleanExpression >( new InequalityComparison( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));

					}
                    catch( const TA_Base_Core::MathematicalEvaluationException& )
                    {
                        return std::auto_ptr< IBooleanExpression >( new InequalityComparison( generateStringExpression(start, (nextOperator - start)),generateStringExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
                    }
                }
            }           

            if( 0 == operatorType.compare(AND) )
            {
				return std::auto_ptr< IBooleanExpression >( new AndExpression( generateBooleanExpression(start, (nextOperator - start)),generateBooleanExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }           
            
			if( 0 == operatorType.compare(OR) )
            {
				return std::auto_ptr< IBooleanExpression >( new OrExpression( generateBooleanExpression(start, (nextOperator - start)),generateBooleanExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }           
            
			if( 0 == operatorType.compare(XOR) )
            {
				return std::auto_ptr< IBooleanExpression >( new XorExpression( generateBooleanExpression(start, (nextOperator - start)),generateBooleanExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
			}
			
			if( 0 == operatorType.compare(NOT) )
            {
				return std::auto_ptr< IBooleanExpression >( new NotExpression( generateBooleanExpression((nextOperator + 1), ((length + start) - (nextOperator + 1)) )));
            }
			
			// All other cases are unknown or invalid expression like TD 11934 there is not "type" after the operator
			std::string msg = "Unknown operator found near or after " + (m_tokens[start])->getVariable();

            TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
        }
        else
        {
			if( (m_tokens[start])->getType() == EETT_Variable )
			{
				if( m_contexts.isBooleanVariable( (m_tokens[start])->getVariable()) )
				{
					return std::auto_ptr< IBooleanExpression >( new BooleanVariable( (m_tokens[start])->getVariable(), m_contexts));
				}
				std::string msg = "Boolean variable " + (m_tokens[start])->getVariable() + " could not be resolved";

				TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
			}
			if( (m_tokens[start])->getType() == EETT_BooleanValue )
			{
				return std::auto_ptr< IBooleanExpression >( new BooleanConstant( (m_tokens[start])->getBooleanValue()));
			}
			std::string msg = "Unknown expression: " + (m_tokens[start])->getVariable();

			TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
        }

        TA_ASSERT(false, "Execution of code should not reach this point.");
    }


    //
    // generateRealNumberExpression
    //
    std::auto_ptr< IRealNumberExpression > ExpressionEvaluator::generateRealNumberExpression(unsigned int start, unsigned int length)
    {
        TA_ASSERT( !(m_tokens.size() < (start + length)), "Invalid size.");

        if( length > 1 )
        {
			if (m_initialLen == 0) m_initialLen = length; // initialise the initial length when this function is first called;
            
			int nextOperator = findLowestPrecedenceOperatorAtSameLevel(start, length);

            if( -1 == nextOperator )
            {
                if( (m_tokens[start])->getType() == EETT_OpenParenthesis )
                {
                    return generateRealNumberExpression( (start + 1), (length - 2) );
                }
                else
                {
                    TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression - missing operator or open bracket"));
                }
            }

            std::string operatorType = (m_tokens[nextOperator])->getRealNumOperator();

			// TD 11934 The length has to be incremented, as the start is always index 0
			// This is a case of an operator being defined at the end of expression ie. invalid
			if ( m_initialLen == (nextOperator + 1) && operatorType.empty()==false )
			{
				// This is an invalid expression
                TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression - missing operator or open bracket"));
			}

            if( 0 == operatorType.compare(MULTIPLY) )
            {
                return std::auto_ptr< IRealNumberExpression >( new RealNumberMultiplyExpression( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }
            if( 0 == operatorType.compare(DIVIDE) )
            {
                return std::auto_ptr< IRealNumberExpression >( new RealNumberDivideExpression( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }
            if( 0 == operatorType.compare(ADDITION) )
            {
                return std::auto_ptr< IRealNumberExpression >( new RealNumberAddExpression( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }
            if( 0 == operatorType.compare(SUBTRACTION) )
            {
                return std::auto_ptr< IRealNumberExpression >( new RealNumberSubtractExpression( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }
            if( 0 == operatorType.compare(POWER) )
            {
                return std::auto_ptr< IRealNumberExpression >( new RealNumberPowerOfExpression( generateRealNumberExpression(start, (nextOperator - start)),generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }
            if( 0 == operatorType.compare(SQRT) )
            {
                return std::auto_ptr< IRealNumberExpression >( new RealNumberSquareRootExpression( generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }
			if( 0 == operatorType.compare(FABS) )
            {
                return std::auto_ptr< IRealNumberExpression >( new RealNumberAbsoluteExpression( generateRealNumberExpression((nextOperator + 1), ((length + start) - (nextOperator + 1))) ));
            }

            std::string msg = "Unknown operator found near or after " + (m_tokens[start])->getVariable();

            TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
        }
        else
        {
            if( (m_tokens[start])->getType() == EETT_Variable )
            {
                if( m_contexts.isRealNumberVariable( (m_tokens[start])->getVariable()) )
                {
                    return std::auto_ptr< IRealNumberExpression >( new RealNumberVariable( (m_tokens[start])->getVariable(), m_contexts));
                }
                std::string msg = "Real Number variable " + (m_tokens[start])->getVariable() + " could not be resolved";

                TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
            }
            if( (m_tokens[start])->getType() == EETT_RealValue )
            {
                return std::auto_ptr< IRealNumberExpression >( new RealNumberConstant( (m_tokens[start])->getRealValue()));
            }
            if( (m_tokens[start])->getType() == EETT_IntegerValue )
            {
                return std::auto_ptr< IRealNumberExpression >( new RealNumberConstant( static_cast<double>(m_tokens[start]->getIntegerValue())));
            }
            std::string msg = "Unknown expression: " + (m_tokens[start])->getVariable();

            TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
        }
        TA_ASSERT(false, "Execution of code should not reach this point.");
    }


    //
    // generateStringExpression
    //
    std::auto_ptr< IStringExpression > ExpressionEvaluator::generateStringExpression(unsigned int start, unsigned int length)
    {
        if( length > 1 )
        {
            int nextOperator = findLowestPrecedenceOperatorAtSameLevel(start, length);

            if( -1 == nextOperator )
            {
                if( (m_tokens[start])->getType() == EETT_OpenParenthesis )
                {
                    return generateStringExpression( (start + 1), (length - 2) );
                }
                else
                {
                    TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression - missing operator or open bracket"));
                }
            }

            std::string msg = "Unknown operator found near or after " + (m_tokens[start])->getVariable();

            TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
        }
        else
        {
            if( (m_tokens[start])->getType() == EETT_Variable )
            {
                if( m_contexts.isStringVariable( (m_tokens[start])->getVariable()) )
                {
                    return std::auto_ptr< IStringExpression >( new StringVariable( (m_tokens[start])->getVariable(), m_contexts));
                }
                std::string msg = "String variable " + (m_tokens[start])->getVariable() + " could not be resolved";

                TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
            }
            if( (m_tokens[start])->getType() == EETT_StringLiteral )
            {
                return std::auto_ptr< IStringExpression >( new StringLiteral( (m_tokens[start])->getStringLiteral()));
            }
            std::string msg = "Unknown expression: " + (m_tokens[start])->getVariable();

            TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
        }
        TA_ASSERT(false, "Execution of code should not reach this point.");
    }

    //
    // addBooleanContext
    //
    void ExpressionEvaluator::addBooleanContext( const IBooleanContext* p_booleanContext )
    {
        TA_Base_Core::ThreadGuard guard( m_writeLock );

        m_contexts.addBooleanContext(p_booleanContext);
    }


    //
    // addRealNumberContext
    //
    void ExpressionEvaluator::addRealNumberContext( const IRealNumberContext* p_realNumContext )
    {
        TA_Base_Core::ThreadGuard guard( m_writeLock );

        m_contexts.addRealNumberContext(p_realNumContext);
    }


    //
    // addStringContext
    //
    void ExpressionEvaluator::addStringContext( const IStringContext* p_stringContext )
    {
        TA_Base_Core::ThreadGuard guard( m_writeLock );

        m_contexts.addStringContext(p_stringContext);
    }


    //
    // evaluate
    //
    double ExpressionEvaluator::evaluateRealNumberExpression()
    {
        TA_Base_Bus::ReadLockHelper g(*this);      
        m_tokens = m_tokeniser.getTokens();
        std::auto_ptr< IRealNumberExpression > exp = std::auto_ptr< IRealNumberExpression >(generateRealNumberExpression(0, m_tokens.size()));
        double result = exp.get()->evaluate();

        return result;
    }


    //
    // evaluate
    //
    bool ExpressionEvaluator::evaluateBooleanExpression()
    {
        TA_Base_Bus::ReadLockHelper g(*this);
        m_tokens = m_tokeniser.getTokens();

		// Check that m_tokens is not null or empty as it should not. Fix for TD 11399
		if ( !(m_tokens.size() > 0) )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "evaluateBooleanExpression called. Tokeniser returned an empty or null token vector, m_tokens." );
			TA_ASSERT(m_tokens.size() > 0, "m_tokens or rules are empty or null which should not be.");
		}

        try
        {
            std::auto_ptr< IBooleanExpression > exp = std::auto_ptr< IBooleanExpression >(generateBooleanExpression(0, m_tokens.size()));
            return exp.get()->evaluate();
        }
        // add misc error handlers here
        // convert all to TA_Base_Core::MathematicalEvaluationExceptions
        catch( TA_Base_Core::ObjectResolutionException& e )
        {
            std::string msg = e.what();
            throw TA_Base_Core::MathematicalEvaluationException(msg);
        }

    }


    //
    // aquireReadLock
    //
    void ExpressionEvaluator::aquireReadLock()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "aquireReadLock" );

        TA_Base_Core::ThreadGuard guard(m_readLock);

        ++m_readCount;
        if( 1 == m_readCount )
        {
            m_writeLock.acquire();
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "aquireReadLock" );
    }


    //
    // releaseReadLock
    //
    void ExpressionEvaluator::releaseReadLock()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "releaseReadLock" );

        TA_Base_Core::ThreadGuard guard(m_readLock);

        --m_readCount;
        if( 0 == m_readCount )
        {
            m_writeLock.release();
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "releaseReadLock" );
    }
}
