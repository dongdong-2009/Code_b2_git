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
#if !defined(DriverCallTasks_H)
#define DriverCallTasks_H

#include "app/trains/train_agent/src/TrainVoiceCallRequest.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/threads/src/IWorkItem.h"


namespace TA_IRS_App
{
    class ITimsDriverCallManagerTaskCallback;
}


namespace TA_Base_Core
{
    class SingleThreadBarrier;
}


namespace TA_IRS_App
{

    /**
     * This is a task executed in the background to perform change area commands
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:20 PM
     */
    class CallResetTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Constructor
         *
         * @param callback    The object that will do the actual task work
         * @param trainId    The train the task will operate on
         */
        CallResetTask( ITimsDriverCallManagerTaskCallback& callback,
                       TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Destructor
         */
        virtual ~CallResetTask();


        /**
         * Executes the change area task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        CallResetTask();


        /**
         * This is the object used to make the call when executing the task
         */
        ITimsDriverCallManagerTaskCallback& m_callback;


        /**
         * The Train the task is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;

    };


    /**
     * This is a task executed in the background to perform change area commands
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:20 PM
     */
    class CallSetupTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Constructor
         *
         * @param callback    The object that will do the actual task work
         * @param request    A reference to the request object to use and update
         * @param barrier    Used to ensure all commands to trains are synchronised
         */
        CallSetupTask( ITimsDriverCallManagerTaskCallback& callback,
                       TrainVoiceCallRequestPtr request,
                       TA_Base_Core::SingleThreadBarrier& barrier );


        /**
         * Destructor
         */
        virtual ~CallSetupTask();


        /**
         * Executes the change area task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        CallSetupTask();


        /**
         * This is the object used to make the call when executing the task
         */
        ITimsDriverCallManagerTaskCallback& m_callback;


        /**
         * A reference to the request object
         */
        TrainVoiceCallRequestPtr m_request;


        /**
         * This is a barrier used to allow all commands to finish at the same time
         */
        TA_Base_Core::SingleThreadBarrier& m_barrier;

    };

} // TA_IRS_App

#endif // !defined(DriverCallTasks_H)
