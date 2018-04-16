#ifndef ACTIONATTRIBUTESELECTOR_H_INCLUDED
#define ACTIONATTRIBUTESELECTOR_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/ActionAttributeSelector.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
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
