/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/banner_framework/src/SingletonHelper.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $	
  *
  * Provides methods to handle the problems of accessing
  * singletons across a DLL interface.
  */

#ifndef SINGLETON_HELPER_H
#define SINGLETON_HELPER_H

#include <string>
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Core
{
    class CorbaUtil;
    class IDllHandler;
}

namespace TA_Base_Bus
{
    class SingletonHelper
    {
    public:
        /**
		  * SingletonHelper
		  *
		  * Standard constructor.
		  */
        SingletonHelper() {};

		/**
		  * ~SingletonHelper
		  *
		  * Standard destructor.
		  */
        virtual ~SingletonHelper() {};

        /**
          * initialiseUtilities
          *
          * This method initialises the following utilities RunParams, CorbaUtil, DebugUtil
          *
          * @param      const IDllHandler& - A reference to the object in the main application
          *                                  from which the dll utilities can be initialised.
          * @param      const std::string& - The entity name of the DLL.
          * @param      const std::string& - The DLL name to be added to the debug filename.
          *
          * @exception  UtilitiesException - Thrown if CorbaUtil or DebugUtil fail to initialise.
          */
        static void initialiseUtilities( const TA_Base_Core::IDllHandler& handler, 
                                         const std::string& entityName,
                                         const std::string& dllName );
    
    private:
        		
		/**
		  * SingletonHelper
		  *
		  * Copy constructor.  Disabled.
		  */
		SingletonHelper( const SingletonHelper& theSingletonHelper );

		/**
		  * operator=
		  *
		  * Assignment operator. Disabled.
		  */
		SingletonHelper& operator=( SingletonHelper& theSingletonHelper );
        
        /**
          * setRunParams
          *
          * Copies the RunParams from the main application's RunParam singleton
          * into the DLL's RunParam singleton.  Existing RunParams are not overwritten,
          * except for the DLL's entity name.
          *
          * @param  TA_Base_Core::RunParams::ParamVector& - A reference to the main
          *                                                 application's vector of RunParams.
          * @param  const std::string& - The DLL's entity name
          */
         static void setRunParams( TA_Base_Core::RunParams::ParamVector& mainAppRunParams,
                                   const std::string& entityName);

        /**
          * initialiseDebugUtil
          *
          * This method retrieves the debug file run param and initialises
          * DebugUtil.
          *
          * @param      const std::string& - The DLL name to be added to the debug filename.
          */
         static void initialiseDebugUtil( const std::string& dllName );
        
        /**
          * initialiseCorba
          *
          * This method sets the CorbaUtil instance to the main application's
          * one and only CorbaUtil instance.
          */
         static void initialiseCorba( TA_Base_Core::CorbaUtil& corbaUtil );
    }; // SingletonHelper
} // TA_Base_Bus

#endif // SINGLETON_HELPER_H
