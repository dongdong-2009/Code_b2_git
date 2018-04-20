/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/SingletonHelper.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $	
  * Provides methods to handle the problems of accessing
  * singletons across a DLL interface.
  */

#pragma warning (disable : 4786)
#pragma warning (disable : 4284)

#include "StdAfx.h"
#include "SingletonHelper.h"

#include "core/exceptions/src/UtilitiesException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"

namespace TA_Base_Core
{
    void SingletonHelper::initialiseUtilities( const IDllHandler& handler )
    {
        // Pick a random runparam that always gets set and see if it is already set
        if (TA_Base_Core::RunParams::getInstance().isSet( RPARAM_APPNAME ))
        {
            // We have already initialised the singletons so don't do it again
            return;
        }

        initialiseCorba( handler.getCorbaUtil() );
        setRunParams( handler.getRunParams() );
        initialiseDebugUtil( );
    }


    void SingletonHelper::initialiseCorba( TA_Base_Core::CorbaUtil& corbaUtil  )
    {
        TA_Base_Core::CorbaUtil::setInstance( corbaUtil );
    }


    void SingletonHelper::setRunParams( TA_Base_Core::RunParams::ParamVector& mainAppRunParams )
    {
        TA_Base_Core::RunParams& dllRunParams = TA_Base_Core::RunParams::getInstance();
        TA_Base_Core::RunParams::ParamVector::iterator it = 0;
        for( it = mainAppRunParams.begin(); it != mainAppRunParams.end(); it++ )
        {
            if( !dllRunParams.isSet( it->name.c_str() ) )
            {
                dllRunParams.set( it->name.c_str(), it->value.c_str() );
            }
        }
    }


    void SingletonHelper::initialiseDebugUtil(  )
    {
        // Set the debug level
        std::string param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().setLevel( TA_Base_Core::DebugUtil::getDebugLevelFromString(param.c_str()));
        }

        // Set the maximum size of a debug file in bytes
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILEMAXSIZE);
        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().setMaxSize(atoi(param.c_str()));
        }

        // Set the maximum number of log files
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGMAXFILES);
        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().setMaxFiles(atoi(param.c_str()));
        }

    }
 

} // TA_Base_Core

        
