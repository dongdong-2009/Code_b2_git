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

#if !defined(ITRAINRADIOGROUPUPDATEOBSERVER_H)
#define ITRAINRADIOGROUPUPDATEOBSERVER_H


#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"


namespace TA_IRS_Bus
{

    /**
     * This is implemented by clients who wish to receive Radio Group update messages
     * from the Train Agent.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:46 PM
     */

    class ITrainRadioGroupUpdateObserver
    {

    public:

        /**
         * This will receive a TrainRadioGroup message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainRadioGroup( const RadioGroupTypes::TrainRadioGroup& event ) = 0;


    protected:

        /**
         * Constructor
         */
        ITrainRadioGroupUpdateObserver() {}

        /**
         * Destructor
         */
        virtual ~ITrainRadioGroupUpdateObserver() {}

    };

}

#endif // !defined(ITrainRadioGroupUpdateObserver_H)
