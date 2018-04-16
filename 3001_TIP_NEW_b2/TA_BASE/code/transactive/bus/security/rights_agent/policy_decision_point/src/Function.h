#ifndef FUNCTION_H_INCLUDED
#define FUNCTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/Function.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/06/07 14:28:46 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  See http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  *
  *  The root of a GoF's Interpreter pattern hierarchy for expression evaluation.
  */

namespace TA_Base_Bus
{
	//Class declaration
	class Function
	{
	public:

		Function()
		{
		}

		virtual ~Function()
		{
		}

		virtual PdpConst::EConditionResult evaluate( const Context& context ) const;

		virtual ValueSet getValues( const Context& context ) const;

	private:

		Function( const Function &x );
		Function& operator=( const Function &x );

	}; // Function
}; // TA_Base_Bus

#endif // FUNCTION_H_INCLUDED
