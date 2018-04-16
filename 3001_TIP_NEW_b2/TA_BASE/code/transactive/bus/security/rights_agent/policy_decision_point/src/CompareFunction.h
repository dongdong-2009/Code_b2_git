#ifndef COMPAREFUNCTION_H_INCLUDED
#define COMPAREFUNCTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/CompareFunction.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  *
  *  This is an extremely cut down version of conditions as presented in the above
  *  document. It only supports one test and only supports != and == test types.
  */

namespace TA_Base_Bus
{
	//Class declaration
	class CompareFunction : public Function
	{
	public:

		virtual ~CompareFunction(){}

		CompareFunction( const Function* lhsOp, const Function* rhsOp,
			PdpConst::EComparisonType compareType );

		virtual PdpConst::EConditionResult evaluate( const Context& context ) const;

	private:

		CompareFunction( const CompareFunction &x );
		CompareFunction& operator=( const CompareFunction &x );

		std::auto_ptr<const Function> m_lhsOp;
		std::auto_ptr<const Function> m_rhsOp;

		PdpConst::EComparisonType m_compareType;

	}; // IsEqualFunction
}; // TA_Base_Bus

#endif // COMPAREFUNCTION_H_INCLUDED
