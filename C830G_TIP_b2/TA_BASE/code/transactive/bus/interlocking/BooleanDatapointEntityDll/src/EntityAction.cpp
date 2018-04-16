/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/BooleanDatapointEntityDll/src/EntityAction.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the 'mainline' of the DLL. This is the entry point and is run when the 
  * Interlocking Rule Trigger Editor loads the DLL.
  *
  */
#include "stdafx.h"

#pragma warning(disable:4786)

#include <string>
#include <vector>

#include "EntityAction.h"

//#include "core/data_access_interface/signs_dai/src/SignSceneReadOnlyFactory.h"
//#include "core/data_access_interface/signs_dai/src/SignTypeReadOnlyFactory.h"
//#include "core/data_access_interface/signs_dai/src/ISignType.h"
//#include "core/data_access_interface/signs_dai/src/ISignScene.h"

#include "core/utilities/src/DllHandler.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/interlocking/interlockingConfig/src/SingletonHelper.h"

//using namespace TA_Base_App::TA_Base_Signs::TA_SignAgentLib;


namespace TA_Base_Bus
{
	/**
     * getActions
     *
     * Returns a list of action for a given entity
     *
	 * @param entityKey - the entity to get the action list for
	 * 
	 */
    std::vector<std::string> EntityAction::getActions(const unsigned long entityKey)
    {
        // actions for data points
        std::vector<std::string> actions;
        actions.push_back( "setValue" );        
        return actions;
    }

    /**
     * getParameters
     *
     * This returns a list of valid parameters for the given action
     *
	 * @param action - tha action to get the parameter list for
	 * 
	 */
    std::vector<std::string> EntityAction::getParameters(const std::string& action)
    {
        // Add code here to retrieve any parameters for this action
        std::vector<std::string> parameters;

        if (action.compare("setValue") == 0)
        {
            parameters.push_back( "true" );
            parameters.push_back( "false" );
        }
        return parameters;
    }

    /** 
     * setUpSingletons
     *
     * This will set up the singletons in the DLL to match those in the main application.
     *
     * @param IDllHandler& - This is a handler from the main application that can be used to
     *                       retrieve the details of it's singletons. This can then be used to
     *                       set up the singletons in the DLL.
     */
    void EntityAction::setUpSingletons(const TA_Base_Core::IDllHandler& dllHandler)
    {       
        // initialise CORBA
        TA_Base_Core::CorbaUtil::setInstance( dllHandler.getCorbaUtil() );

        // initialise debug util
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
        // end initialise debug util


        // Pick a random runparam that always gets set and see if it is already set
        if (TA_Base_Core::RunParams::getInstance().isSet( RPARAM_APPNAME ))
        {
            // We have already initialised the singletons so don't do it again
            return;
        }

        // Now initialise all the singletons inside the DLLs
        TA_Base_Core::RunParams& dllRunParams = TA_Base_Core::RunParams::getInstance();
        TA_Base_Core::RunParams::ParamVector mainAppRunParams(dllHandler.getRunParams());
        TA_Base_Core::RunParams::ParamVector::iterator it = 0;
        for( it = mainAppRunParams.begin(); it != mainAppRunParams.end(); it++ )
        {
            if( !dllRunParams.isSet( it->name.c_str() ) )
            {
                dllRunParams.set( it->name.c_str(), it->value.c_str() );
            }
        }
    }

} // namespace
