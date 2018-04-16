#ifndef IDLL_HANDLER_H
#define IDLL_HANDLER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/utilities/src/IDllHandler.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/12/11 17:56:56 $
  * Last modified by:  $Author: Ouyang $	
  *
  * This is the interface that enables the singletons for the
  * core libraries to be initialised from the singletons in the
  * main application.
  */

#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    class IDllHandler
	{
	public:

        /**
          * ~IDllHandler
          *
          * Standard destructor.
          */

        virtual ~IDllHandler() {};


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
          * @return TA_Base_Core::RunParams::ParamVector& params.
          *         The RunParams for the main application.
          */
		virtual TA_Base_Core::RunParams::ParamVector getRunParams() const = 0;


        /**
          * forwardRunParamUpdates
          *
          * Forwards RunParams updates to the RunParams instance specified.
          *
          * @param runParams The instance to forward updates on to.
          */
        virtual void forwardRunParamUpdates( RunParams* runParams ) = 0;


        /**
          * getCorbaUtil
          *
          * Gets a reference to the main application's CorbaUtil instance.
          *
          * @return TA_Base_Core::CorbaUtil&
          *         A reference to the CorbaUtil singleton instance.
          */
        virtual TA_Base_Core::CorbaUtil& getCorbaUtil() const = 0;

		/**
          * getDebugUtil
          *
          * Gets a reference to the main application's DebugUtil instance.
          *
          * @return TA_Base_Core::DebugUtil&
          *         A reference to the DebugUtil singleton instance.
          */
        virtual TA_Base_Core::DebugUtil& getDebugUtil() const = 0;

	};


} // TA_Base_Core

#endif IDLL_HANDLER_H
