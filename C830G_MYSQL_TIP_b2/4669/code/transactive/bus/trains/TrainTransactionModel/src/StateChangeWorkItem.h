/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(StateChangeWorkItem_H)
#define StateChangeWorkItem_H


#include "bus/trains/TrainTransactionModel/src/TrainState.h"

#include "core/threads/src/IWorkItem.h"


// forward declarations
namespace TA_IRS_Bus
{
    class ITrainStateUpdateListener;
}


namespace TA_IRS_Bus
{
    /**
     * This is a work item for handling a state change,
     * so the state change can be queued in the thread pool.
     *
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:47 PM
     */

    class StateChangeWorkItem : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Creates a new work item to notify observers of state change.
         *
         * @param listener    the listener to notify
         * @param state       the state that changed
         */
        StateChangeWorkItem( ITrainStateUpdateListener& listener, TrainStatePtr state );


        /**
         * Destructor
         */
        virtual ~StateChangeWorkItem();


        /**
         * execute the work item
         */
        virtual void executeWorkItem();

    private:

        StateChangeWorkItem();

        /**
         * listener to notify
         */
        ITrainStateUpdateListener& m_listener;


        /**
         * state update
         */
        TrainStatePtr m_update;

    };

}

#endif // !defined(StateChangeWorkItem_H)
