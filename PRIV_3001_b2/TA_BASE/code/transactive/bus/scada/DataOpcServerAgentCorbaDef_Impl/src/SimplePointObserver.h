/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/SimplePointObserver.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Description
  *
  */

#ifndef SIMPLEPOINT_OBSERVER_H
#define SIMPLEPOINT_OBSERVER_H

#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"

#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointObserver.h"
#include "ISimplePointObserverNotification.h"

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
	class SimplePointObserver : public ISimplePointObserverNotification,
							  public TA_Base_Core::QueueProcessor< TA_Base_Bus::DpObserverUpdate >
	{
	public:

		SimplePointObserver ( TA_Base_Bus::ISimplePointAgent * specificSimplePointAgent, unsigned long observerID );
		virtual ~SimplePointObserver();

		//
		// Operations required by ISimplePointObserverNotification
		//

		virtual void processSimplePointUpdate ( unsigned long pkey, 
			boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> updateDetails );

		//
		// operations specific to SimplePointObserver
		//
		/**
		  * getObserverID
		  *
		  * Returns the ID of the observer
		  *
		  */	
		unsigned long getObserverID() const;

		/**
		  * addSimplePointsToObserver
		  *
		  * This method adds SimplePoints to the observer and returns a sequence containing the current
		  * state and configuration of the SimplePoints	
		  *
		  * @param listOfObservedSimplePoints		The list of SimplePoints to add to the observer.
		  *
		  */	
		void addSimplePointsToObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence& entityKeys );
		/**
          * removeSimplePointsFromObserver
          *
          * This method removes SimplePoints from the observer
          *
          * @param entityKeys	A sequence of entity keys for SimplePoints to remove from the observer.
		  *
          */
		void removeSimplePointsFromObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence& entityKeys );
		/**
          * getUpdates
          *
          * This method returns a sequence of SimplePointUpdates for each item that
		  * has changed since the last time getUpdates was called.
          *
		  *	@return				A sequence containing the updates for SimplePoints that have changed.
		  *						A zero length sequence is returned if there are no updates.
		  *
          */
		//TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState * getUpdates();
		/**
          * getIdleTime
          *
          * This method returns the number of milliseconds since the last time that
		  * the observer was queried by a remote client for the latest updates of
		  * those DataPoints assigned to the observer
          *
          */
		unsigned long getIdleTime();

	protected:

		void processEvent( boost::shared_ptr<DpObserverUpdate> localList );

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

		void saveSimplePointUpdateDetails ( unsigned long pkey, TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState& dpUpdate );

		void registerForUpdates ( TA_Base_Bus::SimplePoint * datapoint );

		unsigned long m_observerID;
		
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;
		TA_Base_Bus::ISimplePointAgent * m_specificSimplePointAgent;

		std::map < unsigned long, SimplePoint * > m_listOfObservedSimplePoints;
		TA_Base_Core::ReEntrantThreadLockable m_observedSimplePointListLock;

		std::vector < TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState* > m_listOfUpdateDetails;
		TA_Base_Core::ReEntrantThreadLockable m_updateDetailsLock;

		SimplePointObserver() {};
		SimplePointObserver ( const SimplePointObserver & theInstance ) {};
		SimplePointObserver & operator= ( const SimplePointObserver & rhs ) {};
	};
}

#endif

