/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/SimplePointStateContainer.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Description
  *
  */

#ifndef SIMPLEPOINT_STATE_CONTAINER_H
#define SIMPLEPOINT_STATE_CONTAINER_H

#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"

#include "ISimplePointObserverNotification.h"
#include "SimplePointDisplayClient.h"
#include "SimplePointObserver.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"

namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class SimplePoint;
	class ISimplePointAgent;

	//TD15618, don't purge anything before the observer is destroyed. 
	//The Agent will destroy this observer after timeout.
	class SimplePointStateContainer : public TA_Base_Bus::ISimplePointObserverNotification
//									public TA_Base_Core::QueueProcessor< DpObserverUpdate >
	{
	public:


		SimplePointStateContainer ( TA_Base_Bus::ISimplePointAgent * specificSimplePointAgent);
		virtual ~SimplePointStateContainer();


		//
		// Operations required by ISimplePointObserverNotification
		//


        virtual void processSimplePointUpdate ( unsigned long pkey, 
			boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> updateDetails );

		//
		// operations specific to SimplePointStateContainer
		//

	    /**
	      * addDisplayClient
	      * 
	      * added new client for the display
	      * 
	      * @return void 
	      * @param : unsigned long clientID
	      * @param : SimplePointDisplayClient * displayClient
	      * 
	      * @exception <exceptions> Optional
	      */
        void addDisplayClient(unsigned long clientID, SimplePointDisplayClient * displayClient);

	    /**
	      * removeDisplayClient
	      * 
	      * removed existing client for the display
	      * 
	      * @return void 
	      * @param : unsigned long clientID
	      * @param : SimplePointDisplayClient * displayClient
	      * 
	      * @exception <exceptions> Optional
	      */
        void removeDisplayClient(unsigned long clientID);

        /**
	      * >* getAllSimplePointDisplayState
	      * 
	      * <description>
	      * 
	      * @return TA_Base_Bus::SimplePointCorbaDef::DisplayStateSequence* 
	      * 
	      * @exception <exceptions> Optional
	      */
        DataPointCorbaTypes::DisplayStateSequence* getAllSimplePointDisplayState();
        
	protected:


	private:
        /**
          * removeSimplePoint
          *
          * This method removes SimplePoints whose pkey matches with the specified key
		  * from the observer
		  *
		  * @param entityKey	The entity key of the SimplePoint to remove from the observer.
          *
          */
		void removeSimplePoint ( unsigned long entityKey );

		/**
          * findExistingSimplePoint
          *
          * This method finds and returns the reference to the data point whose pkey matches
		  * with the entity key specified in the parameter list
          *
          * @param entityKey	The entity key of the wanted SimplePoint
		  *
          */
		SimplePoint * findExistingSimplePoint ( unsigned long entityKey );

        void saveSimplePointUpdateDetails ( unsigned long pkey, const DataPointCorbaTypes::DataPointDisplayState & updateDetails );

        void registerForUpdates ( SimplePoint * datapoint );

        void loadAllSimplePoints();
        	
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;
		TA_Base_Bus::ISimplePointAgent * m_specificSimplePointAgent;

        TA_Base_Core::ReEntrantThreadLockable m_observedSimplePointListLock;
		std::map < unsigned long, SimplePoint * > m_listOfObservedSimplePoints;
        TA_Base_Core::ReEntrantThreadLockable m_clientListLock;
        std::map < unsigned long, SimplePointDisplayClient * > m_listOfSimplePointClients;

        TA_Base_Core::ReEntrantThreadLockable m_listOfSimplePointDisplayStateLock;
		std::map < unsigned long, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * > m_listOfSimplePointDisplayState;

		SimplePointStateContainer() {};
		SimplePointStateContainer ( const SimplePointStateContainer & theInstance ) {};
		SimplePointStateContainer & operator= ( const SimplePointStateContainer & rhs ) {};
	};
}

#endif

