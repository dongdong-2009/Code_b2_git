 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/IndexedAtsDssPlanMaps.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * AtsDssPlanMap is an implementation of IAtsDssPlanMap. It holds the data specific to an AtsDssPlanMap entry
  * in the database, and allows read-only access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/ats/src/IndexedAtsDssPlanMaps.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
    IndexedAtsDssPlanMaps::IndexedAtsDssPlanMaps(IAtsDssPlanMaps dssPlanMaps)
    {
		IAtsDssPlanMapsIt it;

		for (it = dssPlanMaps.begin(); it != dssPlanMaps.end(); it++)
		{
			if (*it != NULL)
			{
				unsigned long index = (*it)->getDssEvent();

				std::pair<IndexedDssPlanMapsIt, bool> result = 
					m_atsDssPlanMaps.insert(IndexedDssPlanMaps::value_type(index, (*it)));


				// Insert should succeed
				TA_ASSERT(result.second == true, "IndexedAtsDssPlanMaps() - insertion of IAtsDssPlanMaps failed");
			}
		}
    }

    IndexedAtsDssPlanMaps::~IndexedAtsDssPlanMaps() 
	{
		IndexedDssPlanMapsIt it;

		for (it = m_atsDssPlanMaps.begin(); it != m_atsDssPlanMaps.end(); it++)
		{
			if ((*it).second != NULL)
			{
				delete (*it).second;
				(*it).second = NULL;
			}
		}

		m_atsDssPlanMaps.clear();
    }

	std::string IndexedAtsDssPlanMaps::getPlanPath(long dssEvent)
    {
		unsigned long index = dssEvent;

		IndexedDssPlanMapsIt it = m_atsDssPlanMaps.find(index);

		if (it != m_atsDssPlanMaps.end() && (*it).second != NULL)
		{
			return (*it).second->getPlanPath();
		}

		// Not found - throw exception
		TA_THROW(TA_Base_Core::DataException("DSS Event/Input pair not found", TA_Base_Core::DataException::NO_VALUE, ""));
    }

} // closes TA_Core
