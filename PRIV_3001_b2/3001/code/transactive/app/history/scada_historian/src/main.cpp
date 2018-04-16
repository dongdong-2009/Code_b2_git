/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/scada_historian/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#include "app/history/scada_historian/src/ScadaHistorianAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GenericAgentException.h"

#include "cots/ACE/5_3/ACE_wrappers/ace/OS.h"

using TA_Core::DebugUtil;
using TA_Core::GenericAgentException;
using TA_App::ScadaHistorianAgent;

int main( int argc, char* argv[] )
{
    try
    {
        ScadaHistorianAgent* agent = new ScadaHistorianAgent(argc, argv);
        agent->run();
        if( agent != NULL )
        {
            delete agent;
            agent = NULL;
        }
    }
    catch( const GenericAgentException& gae )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "GenericAgentException", gae.what());
    }

    return 0;
}

