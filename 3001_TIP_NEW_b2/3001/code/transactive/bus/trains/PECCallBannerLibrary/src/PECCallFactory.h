/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision: #3 $
  *
  * Last modification: $Date: 2012/12/23 $
  * Last modified by:  $Author: raghu.babu $
  *
  */
#if !defined(PECCallFactory_H)
#define PECCallFactory_H


#include "bus/banner_pages/call_page_library/src/AbstractCallEntryFactory.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainInformationUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainPecUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainPecCorbaProxy.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"

#include "bus/trains/PECCallBannerLibrary/src/CallFailureObserver.h"

// forward declarations
namespace TA_IRS_Bus
{
    class CallEntryManager;
}


namespace TA_IRS_Bus
{
    /**
     * @author Robert van Hugten
     * @version 1.0
     * @created 21-Dec-2007 3:49:56 PM
     */

    class PECCallFactory : public AbstractCallEntryFactory,
                           public ITrainPecUpdateObserver,
                           public ITrainInformationUpdateObserver
    {

    public:

        /**
         * Constructor
         *
         * @param callManager    the call entry manager that is used to manage the memory
         * for this factory's call entries.
         */
        PECCallFactory( CallEntryManager& callManager );


        /**
         * Destructor
         */
        ~PECCallFactory();


        /**
         * This will receive a AgentOperationMode message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processAgentOperationMode( const TrainInformationTypes::AgentOperationMode& event );


        /**
         * This will receive a TrainSelectionUpdate message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainSelectionUpdate( const TrainInformationTypes::TrainSelectionUpdate& event );


        /**
         * This will receive a TrainDetails message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainDetails( const TrainInformationTypes::TrainDetails& event );


        /**
         * This will receive a PecUpdate message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processPecUpdate( const PecTypes::PecUpdate& event );


        /** 
         * Checks to see if this operator is involved in any ongoing PECS.
         *
         * @return true if the operator has an ongoing PEC call
         */
        bool isOperatorBusy();


        /** 
         * Checks to see if this train has any PEC answered by driver
         *
         * @return true if any PEC on this train are answered by the driver
         */
        bool trainHasPecAnsweredByDriver( CommonTypes::TrainIdType trainId );


        /**
         * Given a train ID, get the details of that train.
         * If the train is not found, this will assert - its not possible to have a PEC
         * for a train without its basic details being available.
         *
         * @return the details for that train.
         *
         * @param trainId    The train ID to retrieve details for
         */
        TrainInformationTypes::TrainDetails getDetailsForTrain( CommonTypes::TrainIdType trainId );


        /**
         * Gets whether radio is in fallback or not at this location
         */
        bool isRadioInFallback();


        /**
         * Given a PEC number, get the string to display to the user
         *
         * @return the PEC display string
         *
         * @param pecNumber    The PEC number to lookup
         */
        std::string getPecStringForPecId( ProtocolPecTypes::EPecNumber pecNumber );


		/**
		 * Generate display string from PEC car number of a PEC update
		 *
		 * @param carNumber  The PEC car number to lookup
		 *
		 * @return  the PEC car number display string
		 */
		std::string getPecStringForPecCarNum( ProtocolPecTypes::EPecCarNumber carNumber );


		/**
		 * Generate display string from PEC train source of a PEC update
		 *
		 * @param pecSource  The PEC train source to lookup
		 *
		 * @return  the PEC train source display string
		 */
		std::string getPecStringForPecTrainSource( PecTypes::EPecTrainSource pecTrainSource );


        /**
         * Checks if CCTV is available at this location.
         * If false, cctv will not be activated for a PEC, and the CCTV button on the PEC banner will be greyed out.
         *
         * @return true if CCTV should be activated, false otherwise
         */
        bool isCctvAvailable();


        /**
         * From a PEC update, generate its unique, reproducible call ID
         *
         * @param event    The event to process
         */
        virtual std::string getPecIdAsString( const PecTypes::PecUpdate& event );


        /**
         * This will add a cctv image to the train alarm stack for the given train and the given operator.
         *
         * @exception TrainCctvException if the operation fails
         *
         * @param trainId    The train the alarm image is for
         * @param location    Where the train is
         * @param cameraList    The cameras to add
         * @param sessionId    The operator activating the camera
         */
        void addImageToAlarmStack( CommonTypes::TrainIdType trainId,
                                   unsigned long location,
                                   const CctvTypes::TrainCameraSequence& cameraList,
                                   const std::string& sessionId );


		
    private:


        /**
         * Disabled operations
         */
        PECCallFactory();
        PECCallFactory( const PECCallFactory& thePECCallFactory );


        void createProxies( const std::string& sessionId );


        /**
         * This will add/update a PEC call entry
         *
         * @param event    The event to process
         */
        void processPecUpdated( const PecTypes::PecUpdate& event );


        /**
         * This will notify a PEC call entry it was reset, and remove the entry
         *
         * @param event    The event to process
         */
        void processPecReset( const PecTypes::PecUpdate& event );


        /**
         * This will notify a PEC call entry it was reset due to MPU changeover and remove the entry
         *
         * @param event    The event to process
         */
        void processPecMpuChangeover( const PecTypes::PecUpdate event );


        /**
         * This will notify a PEC call entry its call was lost
         *
         * @param event    The event to process
         */
        void processPecCallLost( const PecTypes::PecUpdate& event );


        /**
         * The call manager that this factory informs of updates to PEC messages
         */
        CallEntryManager& m_callManager;


        /**
         * The console the banner is running on
         */
        unsigned long m_consoleKey;


        /**
         * map holds the train details. Should be updated by the train information
         * observer when train details change.
         */
        TrainInformationTypes::TrainDetailsMap m_trainDetailsStore;


        /**
         * Used to lock m_trainDetailsStore
         */
        TA_Base_Core::NonReEntrantThreadLockable m_trainDetailsLock;


        /**
         * The reference to the train information CORBA proxy interface
         */
        TrainInformationCorbaProxyPtr m_trainInformationProxy;


        /**
         * The reference to the train pec CORBA proxy interface that the PEC Call Entries use.
         */
        TrainPecCorbaProxyPtr m_trainPecCorbaProxy;


        /**
         * A simple lookup table to map PEC numbers to the display string
         */
        std::map< ProtocolPecTypes::EPecNumber, std::string > m_pecNumberLookupTable;

		/**
		 * A look up table to map PEC Car numbers to the display string
		 */
		std::map< ProtocolPecTypes::EPecCarNumber, std::string > m_pecCarNumberLookupTable;

		/**
		 * A look up table to map PEC Source to the display string
		 */
		std::map< PecTypes::EPecTrainSource, std::string> m_pecTrainSourceLookupTable;


        /**
         * Whether CCTV is available at this location
         */
        bool m_cctvAvailable;


        /**
         * Whether radio is in fallback or not at this location
         */
        bool m_radioInFallback;


		TA_IRS_Bus::CallFailureObserver * m_CallFailureObserver;

    };

}

#endif // !defined(PECCallFactory_H)
