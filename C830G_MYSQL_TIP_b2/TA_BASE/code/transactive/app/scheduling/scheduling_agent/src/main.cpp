/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/scheduling/scheduling_agent/src/main.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This file contains the main() function for the Scheduling Agent.
  *
  */


#include <vector>
#include <string>

#include "ace/OS.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAgent.h"
#include "app/scheduling/scheduling_agent/src/SchedulingException.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

using TA_Base_App::SchedulingAgent;
using TA_Base_Bus::SchedulingAgentCorbaDef;
using TA_Base_App::SchedulingException;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::CorbaUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;

int main( int argc, char* argv[] )
{
    try
    {
        SchedulingAgent* agent = new SchedulingAgent(argc, argv);
        TA_ASSERT(agent != NULL, "agent should never be NULL");
        agent->run();
        delete agent;
    }
    catch( const GenericAgentException& gae )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "GenericAgentException", gae.what());
    }
    catch( const DatabaseException& dbe )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "DatabaseException", dbe.what());
    }
    catch( const DataException& de )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "DataException", de.what());
    }
    catch( const TA_Base_Bus::SchedulingAgentCorbaDef::BadScheduleException& bse )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "BadScheduleException",
             std::string(bse.what).c_str());
    }
    catch( const SchedulingException& se )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "SchedulingException", se.what());
    }
    catch( ... )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "Unknown Exception");
    }

    return 0;
}
