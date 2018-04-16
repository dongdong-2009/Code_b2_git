/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/incident_dai/src/CtaLocationAccessFactory.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * CtaLocationAccessFactory is a singleton that is used to retrieve Location objects from the
  * database. 
  */
#include <sstream>

#include "core/data_access_interface/incident_dai/src/CtaLocationAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/IncidentException.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#if _MSC_VER > 1000
    #pragma once
    #pragma warning( disable : 4786)
#endif

namespace TA_IRS_Core
{
	CtaLocationAccessFactory::CtaLocationAccessFactory()
		:m_bDataLoaded(false)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}
	
	
	CtaLocationAccessFactory::~CtaLocationAccessFactory()
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
	}
	
	T_LocDisplayMap CtaLocationAccessFactory::getAllLocations()
	{
		FUNCTION_ENTRY("getAllLocations");
		if (true == m_bDataLoaded)
		{
			return m_locationsDisp;
		}

		try
		{
			std::vector<TA_Base_Core::ILocation*> vecLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

			for (unsigned long i = 0; i < vecLocations.size(); i++ )
			{   
				std::auto_ptr<TA_Base_Core::ILocation> pTempLoc(vecLocations[i]);
				std::string location = pTempLoc->getName();
				std::string displayname = pTempLoc->getDisplayName();
				if (!location.empty())
				{            
					m_locationsDisp[location] = displayname;
				}
			}	

			vecLocations.clear();
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught while retrieving locations information from database.");
		}
		
		m_bDataLoaded = true;

		FUNCTION_EXIT;
		return m_locationsDisp;;
	}
} // end TA_IRS_Core
