/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/sound/sound_manager/src/RunParamFetcher.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class is used fetch the Run Params from the
  * Process Manager.
  *
  */

#ifndef RUNPARAMFETCHER_H
#define RUNPARAMFETCHER_H

#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core/corba/src/CorbaUtil.h"

#include <string>
#include <vector>

namespace TA_Base_App
{

    class RunParamFetcher
    {

        public:

            /**
              * Constructor
              */
            RunParamFetcher(std::string hostNames);
            
            /**
              * Destructor
              */
            ~RunParamFetcher()
            {
            }

		private:

            /**
              * locateProcessManager
              *
              * This method retrieves a handle to the Process Manager and, if
			  * successful, assigned its to the m_processManager member variable.
              *
			  * @param hostName The hostname of the Process Manager
			  *
              * @exception ManagedProcessException raised if the Process Manager
              *            cannot be found.
              */
            void locateProcessManager(const std::string& hostName);

            /**
              * retrieveSystemParameters
              *
              * This method retrieves system parameters from the Process Manager.
              * If any parameters have already been set via the command line, they
              * wont be updated (command line has priority).
              * 
              * @exception ManagedProcessException raised if the Process Manager
              *            cant be contacted.
              */
            void retrieveSystemParameters();

			/**
			  * parseHostnames
			  *
			  * Parses a comma-separated list of hostnames into a vector of strings.
			  *
			  * @param hostNames  A comma-separated list of hostnames.
			  *
			  * @return The hostnames as a vector of strings.
			  */
			std::vector<std::string> parseHostnames(const std::string& hostNames);
			

            // Disable the copy constructor and assignment operator
            RunParamFetcher( const RunParamFetcher& );
            RunParamFetcher& operator=(const RunParamFetcher &);

            // Handle to the Process Manager
            TA_Base_Core::IProcessManagerCorbaDef_ptr m_processManager;

    };

} // namespace TA_Base_App

#endif // RUNPARAMFETCHER_H
