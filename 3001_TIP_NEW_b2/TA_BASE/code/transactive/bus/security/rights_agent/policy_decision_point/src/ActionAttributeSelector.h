#ifndef ACTIONATTRIBUTESELECTOR_H_INCLUDED
#define ACTIONATTRIBUTESELECTOR_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/ActionAttributeSelector.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  */

namespace TA_Base_Bus
{
	//Class declaration
	class ActionAttributeSelector : public Function
	{
	public:

		friend class Context;

		ActionAttributeSelector( const std::string& attribName );

		virtual ValueSet getValues( const Context& context ) const;

	private:

		ActionAttributeSelector( const ActionAttributeSelector &x );
		ActionAttributeSelector& operator=( const ActionAttributeSelector &x );

		std::string m_attribName;

	}; // ActionAttributeSelector
}; // TA_Base_Bus

#endif // ACTIONATTRIBUTESELECTOR_H_INCLUDED
