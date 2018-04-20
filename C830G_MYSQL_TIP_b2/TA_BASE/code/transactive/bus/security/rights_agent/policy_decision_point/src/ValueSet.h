#ifndef VALUESET_H_INCLUDED
#define VALUESET_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/ValueSet.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Supporting class for Context. This implements all the stl::map logic to make
  *  context attributes behave as defined in:
  *
  *  http://www.oasis-open.org/committees/xacml/repository/oasis-xacml-1.0.pdf
  */

namespace TA_Base_Bus
{
	//Class declaration
	class ValueSet
	{
	public:

		ValueSet();
		ValueSet( const ValueSet &x );

		ValueSet( const std::set<std::string>& values );

		// Warning, this constructor will reject duplicate values
		ValueSet( const std::vector<std::string>& values );

		bool insert( const std::string& newValue );

		std::set<std::string>::size_type size() const;

		bool operator==( const ValueSet& rhs ) const;
		bool operator<=( const ValueSet& rhs ) const;
		bool operator!=( const ValueSet& rhs ) const;
		
		std::string dump() const;

	private:
		
		
		ValueSet& operator=( const ValueSet &x );

		bool containedBy( const ValueSet& rhs ) const;

		typedef std::set<std::string> ValueSetType;

		ValueSetType m_valueSet;

	}; // ValueSet
}; // TA_Base_Bus

#endif // VALUESET_H_INCLUDED
