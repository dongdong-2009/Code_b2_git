#ifndef M_Eval_Token_H
#define M_Eval_Token_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/Token.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * 
  */
#include <string>
#include <map>

namespace TA_Base_Bus
{

    enum E_ExpressionEvaluationTokenType
    {
        EETT_BooleanOperator     = 0,
        EETT_BooleanUnaryOperator = 1,
        EETT_RealNumOperator     = 2,
        EETT_RealValue           = 3,
        EETT_IntegerValue        = 4,
        EETT_BooleanValue        = 5,
        EETT_StringLiteral       = 6,
        EETT_Variable            = 7,
        EETT_OpenParenthesis     = 8,
        EETT_CloseParenthesis    = 9
    };

    class Token
    {
    public:

        /**
          * Constructor
          */
        Token(){}

        /**
          * Destructor
          */
        virtual ~Token(){}

        void setValue( const std::string& value );

        void setType( const E_ExpressionEvaluationTokenType type );

        const E_ExpressionEvaluationTokenType getType();

        const std::string getRealNumOperator();

        const std::string getBooleanOperator();

        const double getRealValue();

        const int getIntegerValue();

        const bool getBooleanValue();

        const std::string getStringLiteral();

        const std::string getVariable();

        // Allow Token to be copyable.
        Token( const Token& theToken);

    private:

        Token& operator=(const Token&);

        std::string m_value;
        E_ExpressionEvaluationTokenType m_type;
    };
}
#endif // M_Eval_Token_H
