/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/common/src/Serialization.cpp $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/21 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Serialization commonly used by active and editor steps.
*/

#include "Serialization.h"

namespace TA_Base_Bus
{


    Serialization::Serialization():m_bInverse(true)
    {
        m_tmpResult.clear();
    }
 
    Serialization::Serialization( const Serialization::StringVector& strSource ):m_bInverse(false),m_tmpResult(strSource)
    {
    }

    Serialization::StringVector Serialization::stringVector()
    {
        return m_tmpResult;
    }

	bool Serialization::isSerialize()
	{
		return m_bInverse;
	}

	ptree& Serialization::getPtree()
	{
		if (isPtreeEmpty())
		{
			if (!m_tmpResult.empty())
			{
				Serialization::StringVector::iterator firstItr = m_tmpResult.begin();
				std::istringstream is(*firstItr);
				boost::property_tree::json_parser::read_json(is, m_ptResult);
			}
		}

		return m_ptResult;
	}

	bool Serialization::isPtreeEmpty()
	{
		return m_ptResult.empty();
	}

} // TA_Base_App
