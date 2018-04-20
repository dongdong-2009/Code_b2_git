/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/IVariable.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */
///////////////////////////////////////////////////////////
//  IVariable.h
//  Implementation of the Interface IVariable
//  Created on:      08-Feb-2005 02:20:36 PM
///////////////////////////////////////////////////////////

#if !defined(IVariable_0B0A2488_214F_4bf2_B4EE_F6D5033A67D0__INCLUDED_)
#define IVariable_0B0A2488_214F_4bf2_B4EE_F6D5033A67D0__INCLUDED_

namespace TA_Base_Core
{
    class IDllHandler;
}
namespace TA_Base_Bus
{
    class IAddVariable;

    /**
     * Every variable type supported must instantiate this interface.
     * 
     * Typically it will be done in a dll that is loaded on demand..
     * 
     * This interface works through a callback that is passed in to the constructor.
     * Thus no other functions are required.
     *
     */
    class IVariable
    {

    public:

        /**
         * showSelector
         *
         * Display the variable selector dialog
         */
       virtual void showSelector() =0; 

        /**
         * destroySelector
         *
         * Destroy the variable selector dialog after the show is called
         */
       virtual void destroySelector() =0;
	   
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

        // This is the DLL instance. We need to store this so we can close and free the library
        // when we're done.
	    HINSTANCE libraryInstance;
        
    };
} // namespace
#endif // !defined(IVariable_0B0A2488_214F_4bf2_B4EE_F6D5033A67D0__INCLUDED_)