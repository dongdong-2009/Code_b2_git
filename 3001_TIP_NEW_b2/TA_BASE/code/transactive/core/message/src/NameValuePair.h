#if !defined(NameValuePair_486B4473_8472_4781_8430_650410E73806__INCLUDED_)
#define NameValuePair_486B4473_8472_4781_8430_650410E73806__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/message/src/NameValuePair.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/12/17 14:00:24 $
  * Last modified by:  $Author: shurui.liu $
  * 
  * This is used for subscription information. Each Struct represents a filterable
  * name, and the value that it must be.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <vector>

namespace TA_Base_Core
{

	struct NameValuePair
	{
		// What type of evaluation
		enum EvaluationType
		{
			EQUALS = 0,
			NOT_EQUALS
		};

		std::string name;
		std::string value;
		EvaluationType evalType;

		NameValuePair( const std::string& filterName, const std::string& filterValue )
			: name(filterName), value(filterValue), evalType( EQUALS )
		{
		}

		NameValuePair( const std::string& filterName, EvaluationType eval, const std::string& filterValue )
			: name(filterName), value(filterValue), evalType( eval )
		{
		}

		~NameValuePair() {}
	}; // NameValuePair

	typedef std::vector<NameValuePair*> FilterData;

	typedef std::vector<NameValuePair*> DescriptionParameters;
	typedef std::vector<NameValuePair*>::iterator DescriptionParametersIterator;
	typedef std::vector<NameValuePair*>::const_iterator ConstDescriptionParametersIterator;


}; // TA_Base_Core

#endif // !defined(NameValuePair_486B4473_8472_4781_8430_650410E73806__INCLUDED_)
