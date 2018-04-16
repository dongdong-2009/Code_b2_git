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
#if !defined(TrainStateLock_H)
#define TrainStateLock_H


#include "bus/trains/TrainTransactionModel/src/TrainState.h"


// forward declarations
namespace TA_Base_Core
{
    class ThreadGuard;
}


namespace TA_IRS_Bus
{

    /**
     * This is a simple wrapper around ThreadGuard. A Threadguard is created in the
     * constructor and deleted in the destructor.
     *
     * This is used exactly the same as a ThreadGuard.
     *
     * If data is changed, it takes care of ensuring observers are notified of the
     * state change.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:52 PM
     */

    class TrainStateLock
    {

    public:

        /**
         * A constructor that locks an object
         *
         * @param trainState    The object to lock.
         */
        TrainStateLock( TrainStatePtr trainState );


        /**
         * Destuctor, unlocks the object.
         * Notifies observers of the state change if changes exist.
         */
        virtual ~TrainStateLock();

    private:

        /**
         * Private default constructor
         */
        TrainStateLock();


        /**
         * A reference to the state object
         */
        TrainStatePtr m_state;


        /**
         * The guard to create
         */
        TA_Base_Core::ThreadGuard* m_guard;

    };

}

#endif // !defined(TrainStateLock_H)
