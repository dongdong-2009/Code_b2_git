#ifndef RESOURCEATTRIBUTESELECTOR_H_INCLUDED
#define RESOURCEATTRIBUTESELECTOR_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/ResourceAttributeSelector.h $
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
	class ResourceAttributeSelector : public Function
	{
	public:

		friend class Context;

		ResourceAttributeSelector( const std::string& attribName );

		virtual ValueSet getValues( const Context& context ) const;

	private:

		ResourceAttributeSelector( const ResourceAttributeSelector &x );
		ResourceAttributeSelector& operator=( const ResourceAttributeSelector &x );

		std::string m_attribName;

	}; // ResourceAttributeSelector
}; // TA_Base_Bus

#endif // RESOURCEATTRIBUTESELECTOR_H_INCLUDED
