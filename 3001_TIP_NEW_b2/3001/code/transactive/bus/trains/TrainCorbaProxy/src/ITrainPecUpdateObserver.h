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

#if !defined(ITRAINPECUPDATEOBSERVER_H)
#define ITRAINPECUPDATEOBSERVER_H

#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is implemented by clients who wish to receive PEC update messages from the
     * local Train Agent.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:46 PM
     */

    class ITrainPecUpdateObserver
    {

    public:

        /**
         * This will receive a PecUpdate message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processPecUpdate( const PecTypes::PecUpdate& event ) = 0;

    protected:

        ITrainPecUpdateObserver() 
		{

		}

        virtual ~ITrainPecUpdateObserver() 
		{

		}

    };

}

#endif // !defined(ITrainPecUpdateObserver_H)
