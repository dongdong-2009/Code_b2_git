/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/src/SingletonHelper.h $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $	
  *
  * Provides methods to handle the problems of accessing
  * singletons across a DLL interface.
  */

#ifndef SINGLETON_HELPER_H
#define SINGLETON_HELPER_H

#include "app/administration/configuration/configuration_editor/src/IDllHandler.h"

namespace TA_Core
{
    class RunParams;
    class CorbaUtil;
}


namespace TA_IRS_App
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
          * @param      const std::string& - The DLL name to be added to the debug filename.
          *
          * @exception  UtilitiesException - Thrown if CorbaUtil or DebugUtil fail to initialise.
          */
        static void initialiseUtilities( const IDllHandler& handler, const std::string& dllName );


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
          * into the DLL's RunParam singleton.  Existing RunParams are not overwritten.
          *
          * @param  TA_Base_Core::RunParams::ParamVector& - A reference to the main
          *                                            application's vector of RunParams.
          */
         static void setRunParams( TA_Base_Core::RunParams::ParamVector& mainAppRunParams );


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
         static void initialiseCorba( TA_Core::CorbaUtil& corbaUtil );

    }; // SingletonHelper


} // TA_App


#endif // SINGLETON_HELPER_H
