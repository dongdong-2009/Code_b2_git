/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(ChangeModeCommandObserver_H)
#define ChangeModeCommandObserver_H

#include "app/trains/train_agent/src/EChangeModeCommandResult.h"

#include "core/synchronisation/src/Semaphore.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * This is an observer of a change mode command.
     * Change mode commands are executed in the background but can also be manually triggered,
     * for manually triggered this observer is attached to a new, or in progress command.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:25 PM
     */
    class ChangeModeCommandObserver
    {

    public:

        /**
         * Constructor
         */
        ChangeModeCommandObserver();


        /**
         * Destructor
         */
        ~ChangeModeCommandObserver();


        /**
         * This will block for the command to execute, and return the result when complete.
         *
         * @return the result of the change mode command
         */
        EChangeModeCommandResult waitForCompletion();


        /**
         * Once the command is complete, if there was an error, this is used to retrieve the error information.
         *
         * @return the error detail string
         */
        std::string getExtraStatusInformation();


        /**
         * This is used by the change mode task to signal the task has completed, and its status.
         *
         * @param result    The result of the command
         * @param information    Any extra information if applicable
         */
        void signalCompletion( EChangeModeCommandResult result,
                               const std::string& information );


    private:


        /**
         * The semaphore used to wait for a response from the task.
         * Initialised so it blocks on the first call to wait
         */
        TA_Base_Core::Semaphore m_semaphore;


        /**
         * The result of the command once executed
         */
        EChangeModeCommandResult m_result;


        /**
         * Extra status information about the command result
         */
        std::string m_status;

    };

} // TA_IRS_App

#endif // !defined(ChangeModeCommandObserver_H)
