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

#if !defined(ITrainEventProcessor_H)
#define ITrainEventProcessor_H

#include "bus/trains/TrainTransactionModel/src/TrainEvent.h"


namespace TA_IRS_Bus
{
    /**
     * This is implemented by classes that want to consume train events.
     * 
     * They can register for different types of train events.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:41 PM
     */
    class ITrainEventProcessor
    {
    public:

		/**
		 * This is implemented by concrete classes to process incoming events.
		 * 
		 * @param event
		 */
        virtual void processEvent(TrainEventPtr event) =0;

    };

}
#endif // !defined(ITrainEventProcessor_H)
