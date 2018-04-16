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

#if !defined(ITRAINPAUPDATEOBSERVER_H)
#define ITRAINPAUPDATEOBSERVER_H

#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"

namespace TA_IRS_Bus
{
    /**
    * This is implemented by clients who wish to receive PA update messages from the
    * local Train Agent.
    * @author adamr
    * @version 1.0
    * @created 22-Oct-2007 4:46:46 PM
    */

    class ITrainPaUpdateObserver
    {

    public:

        /**
         * This will receive a PaAnnouncementUpdate message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processPaAnnouncementUpdate( const TA_IRS_Bus::PaTypes::PaAnnouncementUpdate& event ) = 0;


        /**
         * This will receive a PaUpdateEvent message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processPaUpdateEvent( const TA_IRS_Bus::PaTypes::PaUpdateEvent& event ) = 0;


        /**
         * This will receive a TrainLibraryInfo message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainLibraryInfo( const TA_IRS_Bus::PaTypes::TrainLibraryInfo& event ) = 0;

    protected:

        ITrainPaUpdateObserver() 
		{

		}

        virtual ~ITrainPaUpdateObserver() 
		{

		}

    };

}

#endif // !defined(ITrainPaUpdateObserver_H)
