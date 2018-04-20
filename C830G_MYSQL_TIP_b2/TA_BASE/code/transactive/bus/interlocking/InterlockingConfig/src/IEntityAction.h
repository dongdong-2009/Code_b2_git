/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/IEntityAction.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
///////////////////////////////////////////////////////////
//  IEntityAction.h
//  Implementation of the Interface IEntityAction
//  Created on:      03-Feb-2005 09:21:30 AM
///////////////////////////////////////////////////////////

#if !defined(IEntityAction_97A39C2A_C5DA_43f5_9A5F_1E6B8A5C38A7__INCLUDED_)
#define IEntityAction_97A39C2A_C5DA_43f5_9A5F_1E6B8A5C38A7__INCLUDED_

#include <string>
#include <vector>

namespace TA_Base_Core
{
    class IDllHandler;
}
namespace TA_Base_Bus
{
    class TA_Base_Core::IDllHandler;
    /**
     * Every entity type supported must instantiate this interface.
     * 
     * Typically it will be done in a dll that is loaded on demand..
     */
    class IEntityAction
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
        virtual std::vector<std::string> getActions(const unsigned long entityKey) =0;

        /**
         * getParameters
         *
         * This returns a list of valid parameters for the given action
         *
	     * @param action - tha action to get the parameter list for
	     * 
	     */
        virtual std::vector<std::string> getParameters(const std::string& action) =0;

        /** 
         * setUpSingletons
         *
         * This will set up the singletons in the DLL to match those in the main application.
         *
         * @param IDllHandler& - This is a handler from the main application that can be used to
         *                       retrieve the details of it's singletons. This can then be used to
         *                       set up the singletons in the DLL.
         */
        virtual void setUpSingletons(const TA_Base_Core::IDllHandler& dllHandler) =0;

    };
} // namespace
#endif // !defined(IEntityAction_97A39C2A_C5DA_43f5_9A5F_1E6B8A5C38A7__INCLUDED_)