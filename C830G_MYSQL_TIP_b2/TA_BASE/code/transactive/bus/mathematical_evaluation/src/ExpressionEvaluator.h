#ifndef M_Eval_ExpressionEvaluator_H
#define M_Eval_ExpressionEvaluator_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/ExpressionEvaluator.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * 
  */
#include <vector>
#include <string>
#include <map>

#include "bus/mathematical_evaluation/src/IBooleanExpression.h"
#include "bus/mathematical_evaluation/src/ContextContainer.h"
#include "bus/mathematical_evaluation/src/IRealNumberExpression.h"
#include "bus/mathematical_evaluation/src/IStringExpression.h"
#include "bus/mathematical_evaluation/src/Tokeniser.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{

    class ExpressionEvaluator
    {
    friend class ReadLockHelper;

    public:

        /**
          * Constructor
          */
        ExpressionEvaluator();

        /**
          * Constructor
          *
          * This constructor instantiates a Tokeniser object and parses
          * the supplied expression (by calling setNewExpression)
          *
          * @param The expression
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be parsed.
          */
        ExpressionEvaluator( const std::string& p_expression );

        /**
          * Destructor
          */
        virtual ~ExpressionEvaluator();

        /**
          * setNewExpression
          *
          * This method instantiates the m_tokeniser object (if necessary)
          * and parses/tokenises the expression.
          *
          * @param The expression
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be parsed.
          */
        void setNewExpression(const std::string& p_expression);

        /**
          * evaluate
          *
          * This method evaluates a boolean expression.
          *
          * @return The result of the expression after evaluation
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be evaluated.
          */
        bool evaluateBooleanExpression();

        /**
          * evaluate
          *
          * This method evaluates a real number expression.
          *
          * @return The result of the expression after evaluation
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be evaluated.
          */
        double evaluateRealNumberExpression();

        /**
          * addBooleanContext
          *
          * This method is used to add boolean contexts to the expression evaluator.
          * These contexts will be used to retrieve the values for any boolean variables
          * included in supplied expressions.
          *
          * NOTE: Contexts need to exist for at least as long as the ExpressionEvaluator object since no ownership is
          *       transferred to it.
          *
          * @param The context to add
          */
        void addBooleanContext( const IBooleanContext* p_booleanContext );

        /**
          * addRealNumberContext
          *
          * This method is used to add real number contexts to the expression evaluator.
          * These contexts will be used to retrieve the values for any real number variables
          * included in supplied expressions.
          *
          * NOTE: Contexts need to exist for at least as long as the ExpressionEvaluator object since no ownership is
          *       transferred to it.
          *
          * @param The context to add
          */
        void addRealNumberContext( const IRealNumberContext* p_realNumContext );

        /**
          * addStringContext
          *
          * This method is used to add string contexts to the expression evaluator.
          * These contexts will be used to retrieve the values for any string variables
          * included in supplied expressions.
          *
          * NOTE: Contexts need to exist for at least as long as the ExpressionEvaluator object since no ownership is
          *       transferred to it.
          *
          * @param The context to add
          */
        void addStringContext( const IStringContext* p_stringContext );

    private:

        // Disable copy constructor and assignment operator.
        ExpressionEvaluator( const ExpressionEvaluator& theExpressionEvaluator);
        ExpressionEvaluator& operator=(const ExpressionEvaluator&);

        /**
          * aquireReadLock
          *
          * This method performs the necessary operations when aquiring the read lock
          *
          */
        void aquireReadLock();

        /**
          * releaseReadLock
          *
          * This method performs the necessary operations when release the read lock
          *
          */
        void releaseReadLock();

        /**
          * findLowestPrecedenceOperatorAtSameLevel
          *
          * This method takes a start point and length, and returns the operator of the
          * lowest precedence at the highest level (ie operators nested within parenthesis are
          * ignored).
          *
          * @param Start point (in expression)
          * @param Length (of expression)
          *
          * @return The index of the lowest precedence operator
          */
        int findLowestPrecedenceOperatorAtSameLevel( unsigned int start, unsigned int length );

        /**
          * generateBooleanExpression
          *
          * This method recursively generates a boolean expression tree for evaluation.
          *
          * NOTE: This method may throw an unhandled exception.
          *
          */
        std::auto_ptr< IBooleanExpression > generateBooleanExpression(unsigned int start, unsigned int length );        

        /**
          * generateRealNumberExpression
          *
          * This method recursively generates a real number expression tree for evaluation.
          *
          * NOTE: This method may throw an unhandled exception.
          *
          */
        std::auto_ptr< IRealNumberExpression > generateRealNumberExpression(unsigned int start, unsigned int length );        

        /**
          * generateStringExpression
          *
          * This method recursively generates a string expression tree for evaluation.
          *
          * NOTE: This method may throw an unhandled exception.
          *
          */
        std::auto_ptr< IStringExpression > generateStringExpression(unsigned int start, unsigned int length );        

        //////////////////////
        // Member variables //
        //////////////////////

        // Obtains a lock for writing.
        TA_Base_Core::ReEntrantThreadLockable m_writeLock;
        // Obtains a lock for reading.
        TA_Base_Core::ReEntrantThreadLockable m_readLock;
        // Number of items read.
        int m_readCount;
		// Initial length (after parsing) of the expression
		int m_initialLen;

        // Used to determine the context of the character returned.
        ContextContainer m_contexts;
        // Separates the input string.
        Tokeniser m_tokeniser;
        // Contains the various tokens obtained from the input string.
        typedef boost::shared_ptr< TA_Base_Bus::Token > TokenPtr;
        std::vector< TokenPtr > m_tokens;
    };
}
#endif // M_Eval_ExpressionEvaluator_H
