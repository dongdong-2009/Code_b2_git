#ifndef IDLL_HANDLER_H
#define IDLL_HANDLER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/utilities/src/IDllHandler.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $	
  *
  * This is the interface that enables the singletons for the
  * core libraries to be initialised from the singletons in the
  * main application.
  */

#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"

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

        ~IDllHandler() {};


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

	};


} // TA_Base_Core

#endif IDLL_HANDLER_H
