#ifndef CONSTATTRIBUTESELECTOR_H_INCLUDED
#define CONSTATTRIBUTESELECTOR_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/ConstAttributeSelector.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  */

namespace TA_Base_Bus
{
	//Class declaration
	class ConstAttributeSelector : public Function
	{
	public:

		ConstAttributeSelector( const std::string& attribValue );

		virtual ValueSet getValues( const Context& context ) const;

	private:

		ConstAttributeSelector( const ConstAttributeSelector &x );
		ConstAttributeSelector& operator=( const ConstAttributeSelector &x );

		ValueSet m_valueSet;

	}; // ConstAttributeSelector
}; // TA_Base_Bus

#endif // CONSTATTRIBUTESELECTOR_H_INCLUDED
