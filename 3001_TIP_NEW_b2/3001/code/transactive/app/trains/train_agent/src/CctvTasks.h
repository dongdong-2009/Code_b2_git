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
#if !defined(CctvTasks_H)
#define CctvTasks_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/threads/src/IWorkItem.h"

#include <map>

// forward declarations
namespace TA_IRS_App
{
    class ITimsCctvManagerTaskCallback;
}

namespace TA_IRS_App
{

    /**
     * This is a task executed in the background to perform change area commands
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:21 PM
     */
    class CarrierOffTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Constructor
         *
         * @param callback    The object that will do the actual task work
         * @param trainId    The train the task will operate on
         * @param owningTrainId    If the carrier off is due to conflict, which train is the owning train
         */
        CarrierOffTask( ITimsCctvManagerTaskCallback& callback,
                        TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                        TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId );


        /**
         * Destructor
         */
        virtual ~CarrierOffTask();


        /**
         * Checks if this task has been scheduled in the thread pool yet
         *
         * @return true if it has been scheduled, false if it is still waiting
         */
        bool isScheduled();


        /**
         * Sets this task to the scheduled state
         */
        void setScheduled();


        /**
         * Executes the change area task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        CarrierOffTask();


        /**
         * This is the object used to make the call when executing the task
         */
        ITimsCctvManagerTaskCallback& m_callback;


        /**
         * The Train the task is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * If the carrier off is due to conflict, which train is the owning train
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_owningTrainId;


        /**
         * This indicates whether this command has been scheduled to execute,
         * or if it is waiting for a change mode task to complete successfully.
         */
        bool m_scheduled;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:22 PM
     */
    typedef boost::shared_ptr< CarrierOffTask > CarrierOffTaskPtr;

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:22 PM
     */
    typedef std::map< TA_IRS_Bus::CommonTypes::TrainIdType, CarrierOffTaskPtr > CarrierOffTaskMap;

}

#endif // !defined(CctvTasks_H)
