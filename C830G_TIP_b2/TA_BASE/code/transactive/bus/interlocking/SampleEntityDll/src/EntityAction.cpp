/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/SampleEntityDll/src/EntityAction.cpp $
  * @author:  Robert Stagg
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

#include "core/utilities/src/DllHandler.h"
#include "EntityAction.h"


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
        // Add code here to retrieve allowed actions for this entity
        // The key has been provided in case different entities of the
        // same type can have different actions

        // For this sample code actions are hard coded
        std::vector<std::string> actions;
        actions.push_back("Flying");
        actions.push_back("Grounded");
        actions.push_back("Parked");
        actions.push_back("Taxing");
        actions.push_back("Circuit");
        
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

        // For this sample code parameters are hard coded
        std::vector<std::string> parameters;

        if (action.compare("Grounded") == 0)
        {
            parameters.push_back("mechanical");
            parameters.push_back("requires service");
        }
        if (action.compare("Parked") == 0)
        {
            parameters.push_back("Hanger");
            parameters.push_back("Tarmac");
        }
        if (action.compare("Circuit") == 0)
        {
            parameters.push_back("Entry");
            parameters.push_back("Downwind");
            parameters.push_back("Crosswind");
            parameters.push_back("Final");
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
        // nothing to do here for sample code
    }

} // namespace
