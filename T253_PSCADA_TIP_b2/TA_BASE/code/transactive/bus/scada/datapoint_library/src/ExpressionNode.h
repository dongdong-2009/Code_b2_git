/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/datapoint_library/src/ExpressionNode.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2017/10/31 16:21:54 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */

#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include <string>

#include "app/scada/common/src/CommonDefs.h"
#include "app/scada/common/src/IEntityUpdateEventProcessor.h"

class InputAssociation;
class IExpressionNodeNotification;
class DataPointValue;
class DataPointState;

class ExpressionNode : public IEntityUpdateEventProcessor
{
public:

	ExpressionNode ( IExpressionNodeNotification * user );
	virtual ~ExpressionNode();

	//
	// operations specific to IEntityUpdateEventProcessor
	//


	virtual void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType);


	//
	// operations specific to ExpressionNode
	//


	const DataPointState & getCurrentState() const;
	void setNumberString ( const std::string & numberString );
	void setDataPointNameString ( const std::string & dataPointName );

	unsigned char getOperator() const;
	void setOperator ( unsigned char expressionOperator );

	ExpressionNode * getLeftExpression() const;
	void setLeftExpressionNode ( ExpressionNode * left );

	ExpressionNode * getRightExpression() const;
	void setRightExpressionNode ( ExpressionNode * right );

private:

	void ExpressionNode::updateCurrentState ( const DataPointValue & dpv,
											  unsigned long tmstamp,
											  DataPointStatus status );

	std::string m_valueString;
	unsigned char m_expressionOperator;
	DataPointState * m_currentState;
	ExpressionNode * m_left;
	ExpressionNode * m_right;
	InputAssociation * m_inputAssociation;
	IExpressionNodeNotification * m_user;
	
	ExpressionNode() {};
	ExpressionNode ( const ExpressionNode & theInstance ) {};
	ExpressionNode & operator= ( const ExpressionNode & rhs ) {};
};

#endif