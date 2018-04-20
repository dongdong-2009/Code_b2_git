/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/security/authentication_agent/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
