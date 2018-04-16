/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/ExpressionEvaluator.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2012/02/06 16:15:14 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *
  */

#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include <string>

class ExpressionNode;
class IExpressionNodeNotification;

class ExpressionEvaluator
{
public:

	ExpressionEvaluator ( IExpressionNodeNotification * user );
	virtual ~ExpressionEvaluator();

	bool getIsValidExpression() const;
	bool getResult();
	void updateExpression ( const std::string & expression );

private:

	static const char EQUALITY_CHARACTER;
	static const char WHITE_SPACE_CHARACTER;
	static const char END_OF_STRING_CHARACTER;
	static const char OPEN_BRACKET_CHARACTER;
	static const char CLOSE_BRACKET_CHARACTER;
	static const char DOT_CHARACTER;

	void parseExpression ( const std::string & expression );
	void incrementStrPointer();
	void SkipSpaces();
	ExpressionNode * findEquality();
	ExpressionNode * findOpenCloseBracket();
	ExpressionNode * findIdentifier();
	double evaluate ( const ExpressionNode * currentNode );

	bool m_evaluationError;
	bool m_validExpression;
	int m_currentPosition;
	std::string m_expressionString;
	ExpressionNode * m_currentNode;
	IExpressionNodeNotification * m_user;

	ExpressionEvaluator() {};
	ExpressionEvaluator ( const ExpressionEvaluator & theInstance ) {};
	ExpressionEvaluator & operator= ( const ExpressionEvaluator & rhs ) {};
};

#endif