/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/utilities/src/DllHandler.h $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2018/02/08 10:46:11 $
  * Last modified by:  $Author: limin.zhu $	
  *
  * This implements the IDLLHandler so it can be passed to the SingletonHelper to allow it
  * to initialise all singletons.
  */


#ifndef DLL_HANDLER_H
#define DLL_HANDLER_H

#include "core/utilities/src/IDllHandler.h"

#include "core/utilities/src/RunParams.h"

namespace TA_Base_Core
{
    class CorbaUtil;
    class DebugUtil;

    class DllHandler : public IDllHandler
	{
	public:

        /**
          * ~DllHandler
          *
          * Standard destructor.
          */

        ~DllHandler() {};


        ///////////////////////////////////////////////////////////////////
        //
        // These methods are to overcome the problems of singletons that
        // cannot be shared across the DLL interface because DLLs have
        // their own memory space.
        //
        ///////////////////////////////////////////////////////////////////
        

        /**
          * getRunParams
          *
          * Gets the RunParams from the main application.
          *
          * @return TA_Base_Core::RunParams::ParamVector& - The RunParams for the main application.
          */
        virtual TA_Base_Core::RunParams::ParamVector getRunParams() const;


        /**
          * forwardRunParamUpdates
          *
          * Forwards RunParams updates to the RunParams instance specified.
          *
          * @param runParams The instance to forward updates on to.
          */
        virtual void forwardRunParamUpdates( RunParams* runParams );


        /**
          * getCorbaUtil
          *
          * Gets a reference to the main application's CorbaUtil instance.
          *
          * @return TA_Base_Core::CorbaUtil& - A reference to the CorbaUtil singleton instance.
          */
        virtual TA_Base_Core::CorbaUtil& getCorbaUtil() const;

        /**
          * getDebugUtil
          *
          * Gets a reference to the main application's DebugUtil instance.
          *
          * @return TA_Base_Core::DebugUtil& - A reference to the DebugUtil singleton instance.
          */
        virtual TA_Base_Core::DebugUtil& getDebugUtil() const;

	};

} // TA_Base_Core

#endif //DLL_HANDLER_H
