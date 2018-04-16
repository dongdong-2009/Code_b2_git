#ifndef PLANS_COMMON_H_INCLUDED
#define PLANS_COMMON_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/bus/response_plans/PlanDataAccess/src/PlansCommon.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  General include for common plans code.
  */

// Pragmas
//#pragma warning( disable : 4786 4018 4146 )

// System includes
#include <iostream>
#include <memory>
#include <sstream>
#include <limits>
#include <map>
#include <exception>

#if defined( WIN32 ) || defined( __WIN32__ )
#include <crtdbg.h>
#endif

// COTS includes
#include "ace/Singleton.h"

// Core includes
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/entity_access/src/PlanAgentDai.h"

// Project includes
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataBaseHelper.h"

// Defines
#if defined( WIN32 ) || defined( __WIN32__ )
#define _CRTDBG_MAP_ALLOC
#endif

// Logging defines
#define SOURCE_INFO      SourceInfo

#define GENERIC_LOG_TYPE          TA_Base_Core::DebugUtil::GenericLog
#define EXCEPTION_CATCH_LOG_TYPE  TA_Base_Core::DebugUtil::ExceptionCatch
#define EXCEPTION_DETAIL_LOG_TYPE TA_Base_Core::DebugUtil::ExceptionDetails
#define FUNCTION_ENTRY_LOG_TYPE   TA_Base_Core::DebugUtil::FunctionEntry
#define FUNCTION_EXIT_LOG_TYPE    TA_Base_Core::DebugUtil::FunctionExit

#define FATAL_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugFatal
#define ERROR_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugError
#define WARN_LOG_LEVEL   TA_Base_Core::DebugUtil::DebugWarn
#define INFO_LOG_LEVEL   TA_Base_Core::DebugUtil::DebugInfo
#define DEBUG_LOG_LEVEL  TA_Base_Core::DebugUtil::DebugDebug


#endif // PLANS_COMMON_H_INCLUDED
