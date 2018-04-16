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

#if !defined(ITRAININFORMATIONUPDATEOBSERVER_H)
#define ITRAININFORMATIONUPDATEOBSERVER_H


#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"


namespace TA_IRS_Bus
{
    /**
     * This is implemented by clients who wish to receive Train Information update
     * messages from the Local Train Agent.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:45 PM
     */

    class ITrainInformationUpdateObserver
    {

    public:

        /**
         * This will receive a AgentOperationMode message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processAgentOperationMode( const TrainInformationTypes::AgentOperationMode& event ) = 0;


        /**
         * This will receive a TrainDetails message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainDetails( const TrainInformationTypes::TrainDetails& event ) = 0;


        /**
         * This will receive a TrainSelectionUpdate message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainSelectionUpdate( const TrainInformationTypes::TrainSelectionUpdate& event ) = 0;

    protected:

        ITrainInformationUpdateObserver() {}

        virtual ~ITrainInformationUpdateObserver() {}

    };

}

#endif // !defined(ITrainInformationUpdateObserver_H)
