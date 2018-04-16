/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/SimpleNodeAgentCorbaDef_Impl/src/SimpleNodeObserver.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Description
  *
  */

#ifndef SIMPLENODE_OBSERVER_H
#define SIMPLENODE_OBSERVER_H

#include <time.h>
#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"

#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeObserver.h"

#include "ISimpleNodeObserverNotification.h"

namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class SimpleNode;
	class ISimpleNodeAgent;

	//The Agent will destroy this observer after timeout.
	class SimpleNodeObserver : public TA_Base_Bus::ISimpleNodeObserverNotification,
		public TA_Base_Core::QueueProcessor< TA_Base_Bus::DnObserverUpdate >
	{
	public:


		SimpleNodeObserver ( TA_Base_Bus::ISimpleNodeAgent * specificSimpleNodeAgent, unsigned long observerID );
		virtual ~SimpleNodeObserver();

		//
		// Operations required by ISimpleNodeObserverNotification
		//

		virtual void processSimpleNodeUpdate(unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> updateDetails);

		//
		// operations specific to SimpleNodeObserver
		//

		/**
		  * getObserverID
		  *
		  * Returns the ID of the observer
		  *
		  */
		unsigned long getObserverID() const;

		/**
		  * addSimpleNodesToObserver
		  *
		  * This method adds SimpleNodes to the observer and returns a sequence containing the current
		  * state and configuration of the SimpleNodes
		  *
		  * @param listOfObservedSimpleNodes		The list of SimpleNodes to add to the observer.
		  *
		  */
		void addSimpleNodesToObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence& entityKeys );

		/**
          * removeSimpleNodesFromObserver
          *
          * This method removes SimpleNodes from the observer
          *
          * @param entityKeys	A sequence of entity keys for SimpleNodes to remove from the observer.
		  *
          */
		void removeSimpleNodesFromObserver ( const TA_Base_Bus::ScadaCorbaTypes::KeySequence & entityKeys );

		/**
          * getUpdates
          *
          * This method returns a sequence of SimpleNodeUpdates for each item that
		  * has changed since the last time getUpdates was called.
          *
		  *	@return				A sequence containing the updates for SimpleNodes that have changed.
		  *						A zero length sequence is returned if there are no updates.
		  *
          */
		TA_Base_Bus::DataNodeDisplayState * getUpdates();

		/**
          * getIdleTime
          *
          * This method returns the number of milliseconds since the last time that
		  * the observer was queried by a remote client for the latest updates of
		  * those SimpleNodes assigned to the observer
          *
          */
		unsigned long getIdleTime();

	protected:

		void processEvent( boost::shared_ptr<DnObserverUpdate> localList );

	private:


		/**
          * removeSimpleNode
          *
          * This method removes SimpleNodes whose pkey matches with the specified key
		  * from the observer
		  *
		  * @param entityKey	The entity key of the SimpleNode to remove from the observer.
          *
          */
		void removeSimpleNode ( unsigned long entityKey );


		/**
          * findExistingSimpleNode
          *
          * This method finds and returns the reference to the data point whose pkey matches
		  * with the entity key specified in the parameter list
          *
          * @param entityKey	The entity key of the wanted SimpleNode
		  *
          */
		SimpleNode * findExistingSimpleNode ( unsigned long entityKey );

		void saveSimpleNodeUpdateDetails ( unsigned long pkey, const TA_Base_Bus::DataNodeDisplayState & updateDetails );

		void registerForUpdates ( TA_Base_Bus::SimpleNode * SimpleNode );

//        void SimpleNodeObserver::activateSimpleNodes( void );
            
        SimpleNodeObserver();
		SimpleNodeObserver ( const SimpleNodeObserver & theInstance );
		SimpleNodeObserver & operator= ( const SimpleNodeObserver & rhs );

    private:

		unsigned long m_observerID;
		
		TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;
		ISimpleNodeAgent * m_specificSimpleNodeAgent;
        
		std::map < unsigned long, SimpleNode * > m_listOfObservedSimpleNodes;
		TA_Base_Core::ReEntrantThreadLockable m_observedSimpleNodeListLock;

		std::vector < TA_Base_Bus::DataNodeDisplayState * > m_listOfUpdateDetails;
		TA_Base_Core::ReEntrantThreadLockable m_updateDetailsLock;
	};
}

#endif
