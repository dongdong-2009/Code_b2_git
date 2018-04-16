/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/BooleanDatapointEntityDll/src/EntityAction.h $
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
///////////////////////////////////////////////////////////
//  IEntityAction.h
//  Implementation of the Interface IEntityAction
//  Created on:      03-Feb-2005 09:21:30 AM
///////////////////////////////////////////////////////////

#if !defined(EntityAction_H)
#define EntityAction_H

#include "bus/interlocking/InterlockingConfig/src/IEntityAction.h"

namespace TA_Base_Bus
{
    class IDllHandler;

    /**
     * EntityAction
     *
     * instantiates the IEntityAction interface.
     * 
     */
    class EntityAction : public IEntityAction
    {

    public:

	    /**
         * getActions
         *
         * Returns a list of action for a given entity
         *
	     * @param entityKey - the entity to get the action list for
	     * 
	     */
        std::vector<std::string> getActions(const unsigned long entityKey = 0);

        /**
         * getParameters
         *
         * This returns a list of valid parameters for the given action
         *
	     * @param action - tha action to get the parameter list for
	     * 
	     */
        std::vector<std::string> getParameters(const std::string& action);

        /** 
         * setUpSingletons
         *
         * This will set up the singletons in the DLL to match those in the main application.
         *
         * @param IDllHandler& - This is a handler from the main application that can be used to
         *                       retrieve the details of it's singletons. This can then be used to
         *                       set up the singletons in the DLL.
         */
        void setUpSingletons(const TA_Base_Core::IDllHandler& dllHandler);

    private:
        unsigned long m_entityKey;

    };
} // namespace
#endif // !defined(EntityAction_H)
