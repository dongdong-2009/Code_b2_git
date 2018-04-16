/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_TIP/TA_BASE/transactive/bus/response_plans/PlanDataAccess/src/PlanDataBaseHelper.cpp $
  *  @author:  Jian Huang
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2011/01/25 10:51:00 $
  *  Last modified by:  $Author: jian.huang $
  *
  *  Implementation of PlanDataBaseHelper.
  *  Help to release open cursor automatically.
  */

#include "core/data_access_interface/src/IDatabase.h"
#include "core/utilities/src/DebugUtil.h"

#include "bus/response_plans/PlanDataAccess/src/PlansCommon.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataBaseHelper.h"

namespace TA_Base_Bus
{
		PlanDataBaseHelper::PlanDataBaseHelper(TA_Base_Core::IDatabase* pIDatabase):m_pIDatabase(pIDatabase){}

		PlanDataBaseHelper::~PlanDataBaseHelper()
		{
			try
			{
				if( NULL == m_pIDatabase)
				{
					LOG_GENERIC( SOURCE_INFO, ERROR_LOG_LEVEL, "Error: m_pIDatabase == NULL", "");
				}
				else
				{
					m_pIDatabase->cleanQuery();
				}
				
			}
			catch (std::exception& e)
			{
				LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Unknown Exception: %s", e.what());
			}
			catch (...)
			{
				LOG(SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Unknown Exception");
			}
			
		}
}
