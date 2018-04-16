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

#if !defined(ITrainTisUpdateObserver_H)
#define ITrainTisUpdateObserver_H


#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"


namespace TA_IRS_Bus
{
    /**
     * This is implemented by clients who wish to receive TIS update messages from the
     * Train Agent.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:47 PM
     */

    class ITrainTisUpdateObserver
    {

    public:

        /**
         * This will receive a TisLibraryUpdate message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTisLibraryUpdate( const TtisTypes::TisLibraryUpdate& event ) = 0;


        /**
         * This will receive a TisLibraryVersionUpdate message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTisLibraryVersionUpdate( const TtisTypes::TisLibraryVersionUpdate& event ) = 0;


        /**
         * This will receive a TisReturnStatus message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTisReturnStatus( const TtisTypes::TisReturnStatus& event ) = 0;

    protected:

        ITrainTisUpdateObserver() {}

        virtual ~ITrainTisUpdateObserver() {}

    };

}

#endif // !defined(ITrainTisUpdateObserver_H)
