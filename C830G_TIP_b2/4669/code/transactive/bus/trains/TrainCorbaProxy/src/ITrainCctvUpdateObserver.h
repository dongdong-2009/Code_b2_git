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

#if !defined(ITrainCctvUpdateObserver_H)
#define ITrainCctvUpdateObserver_H

#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"

namespace TA_IRS_Bus
{

    /**
     * This is implemented by clients who wish to receive CCTV update messages from
     * the Train Agent.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:45 PM
     */

    class ITrainCctvUpdateObserver
    {

    public:

        /**
         * This will receive a CctvSwitchState message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processCctvSwitchState( const CctvTypes::CctvSwitchState& event ) = 0;

    protected:

        /**
         * Constructor
         */
        ITrainCctvUpdateObserver() {}


        /**
         * Constructor
         */
        virtual ~ITrainCctvUpdateObserver() {}

    };

}

#endif // !defined(ITrainCctvUpdateObserver_H)
