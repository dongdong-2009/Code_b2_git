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
#if !defined(AtsMessageProcessor_H)
#define AtsMessageProcessor_H


#include "app/trains/train_agent/src/AtsDataTypes.h"
#include "app/trains/train_agent/src/IAtsChangeProcessor.h"
#include "app/trains/train_agent/src/IAtsMessageProcessor.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"

#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"

#include "core/threads/src/QueueProcessor.h"


namespace TA_IRS_App
{
    class ITrainStatusManager;
}


namespace TA_IRS_App
{

    /**
     * This will implement the logic to process ATS messages.
     * It is a QueueProcessor for AtsTrainInformation messages, and a GenericObserverManager for IAtsChangeProcessors
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:17 PM
     */
    class AtsMessageProcessor : public TA_Base_Core::QueueProcessor<AtsTrainInformation>,
                                public TA_IRS_Bus::GenericObserverManager<IAtsChangeProcessor>,
                                public IAtsMessageProcessor
    {

    public:

        /**
         * Constructor
         *
         * @param statusManager    The status manager used to retreive and update train ats information
         */
        AtsMessageProcessor( ITrainStatusManager& statusManager );


        /**
         * Destructor
         */
        ~AtsMessageProcessor();


        /**
         * This will process the train information from the ATS Agent.
         *
         * @param trainInformation    The update from ATS
         */
        virtual void processAtsInformation( const AtsTrainInformation& trainInformation );


    protected:


        /**
         * This will process a queued ATS event
         *
         * @param newEvent
         */
        virtual void processEvent( boost::shared_ptr<AtsTrainInformation> newEvent );


    private:


        /**
         * Private default constructor
         */
        AtsMessageProcessor();


        /**
         * Used to set and retrieve train ats information
         */
        ITrainStatusManager& m_statusManager;
    };

} // TA_IRS_App

#endif // !defined(AtsMessageProcessor_H)
