#ifndef SUBJECTATTRIBUTESELECTOR_H_INCLUDED
#define SUBJECTATTRIBUTESELECTOR_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/SubjectAttributeSelector.h $
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
	class SubjectAttributeSelector : public Function
	{
	public:

		friend class Context;

		SubjectAttributeSelector( const std::string& attribName, const std::string& category = "" );

		virtual ValueSet getValues( const Context& context ) const;

	private:

		SubjectAttributeSelector( const SubjectAttributeSelector &x );
		SubjectAttributeSelector& operator=( const SubjectAttributeSelector &x );

		std::string m_attribName;
		std::string m_category;
		
	}; // SubjectAttributeSelector
}; // TA_Base_Bus

#endif // SUBJECTATTRIBUTESELECTOR_H_INCLUDED
