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
#if !defined(ChangeAreaTask_H)
#define ChangeAreaTask_H

#include "app/trains/train_agent/src/ChangeModeCommandObserver.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"

#include "core/threads/src/IWorkItem.h"

// forward declarations
namespace TA_IRS_App
{
    class ITimsModeManagerTaskCallback;
}

namespace TA_IRS_App
{

    /**
     * This is a task executed in the background to perform change mode and change area commands
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:24 PM
     */
    class ChangeModeChangeAreaTask : public TA_IRS_Bus::GenericObserverManager< ChangeModeCommandObserver >,
                                     public TA_Base_Core::IWorkItem
    {

    public:

        enum ETaskType
        {
            ChangeModeTask = 0,
            ChangeAreaTask = 1
        };

        /**
         * Constructor
         *
         * @param callback    The object that will do the actual task work
         * @param trainId    The train the task will operate on
         */
        ChangeModeChangeAreaTask( ITimsModeManagerTaskCallback& callback,
                                  TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                  ETaskType taskType );


        /**
         * Destructor
         */
        virtual ~ChangeModeChangeAreaTask();


        /**
         * Sets this task to the cancelled state
         */
        void setCancelled();


        /**
         * Executes the change mode task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        ChangeModeChangeAreaTask();


        /**
         * This is the object used to make the call when executing the task
         */
        ITimsModeManagerTaskCallback& m_callback;


        /**
         * The Train the task is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * The actual command to send
         */
        ETaskType m_taskType;


        /**
         * This indicates whether this command has been cancelled after it was scheduled
         */
        bool m_cancelled;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:24 PM
     */
    typedef boost::shared_ptr< ChangeModeChangeAreaTask > ChangeModeChangeAreaTaskPtr;


    /**
     * A map of train ID to change mode tasks
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:24 PM
     */
    typedef std::map< TA_IRS_Bus::CommonTypes::TrainIdType, ChangeModeChangeAreaTaskPtr > ChangeModeChangeAreaTaskMap;

} // TA_IRS_App

#endif // !defined(ChangeAreaTask_H)
