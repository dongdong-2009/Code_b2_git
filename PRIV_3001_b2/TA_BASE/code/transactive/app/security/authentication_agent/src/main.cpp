/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/authentication_agent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#include "bus/security/authentication_agent/IDL/src/SessionInfoCorbaDef.h"

#include "app/security/authentication_agent/src/SessionRegister.h"
#include "app/security/authentication_agent/src/AuthenticationMessageReceiver.h"
#include "app/security/authentication_agent/src/AuthenticationAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GenericAgentException.h"

#include "ace/OS.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

using TA_Base_App::AuthenticationAgent;

int main( int argc, char* argv[] )
{
    //
    try
    {
        AuthenticationAgent authAgent(argc,argv);

        authAgent.run();
    }
    catch( const GenericAgentException& gae )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch,
             "GenericAgentException", gae.what());
    }

    return 0;
}
