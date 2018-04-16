/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/response_plans/PlanDataAccess/src/PlanDataBaseHelper.cpp $
  *  @author:  Jian Huang
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  Implementation of PlanDataBaseHelper.
  *  Help to release open cursor automatically.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IDatabase.h"

#include "bus/response_plans/PlanDataAccess/src/PlanDataBaseHelper.h"

namespace TA_Base_Bus
{

PlanDataBaseHelper::PlanDataBaseHelper(TA_Base_Core::IDatabase* pIDatabase) : m_pIDatabase(pIDatabase)
{
}

PlanDataBaseHelper::~PlanDataBaseHelper()
{
	try
	{
		if( NULL == m_pIDatabase)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error: m_pIDatabase == NULL", "");
		}
		else
		{
			m_pIDatabase->cleanQuery();
		}
	}
	catch (std::exception& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception: %s", e.what());
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown Exception");
	}
}

}
