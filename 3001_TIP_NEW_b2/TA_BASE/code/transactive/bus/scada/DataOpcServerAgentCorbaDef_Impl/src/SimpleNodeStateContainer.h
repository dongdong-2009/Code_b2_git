/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeStateContainer.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Description
  *
  */

#ifndef SIMPLENODE_STATE_CONTAINER_H
#define SIMPLENODE_STATE_CONTAINER_H

#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"

//#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "ISimpleNodeObserverNotification.h"
#include "bus/scada/common_library/src/CommonDefs.h"


namespace TA_Base_Core
{
	class StopwatchUtil;
}

namespace TA_Base_Bus
{
	// forward declaration
	class SimpleNode;
	class SimpleNodeDisplayClient;
	class ISimpleNodeAgent;

	//TD15618, don't purge anything before the observer is destroyed. 
	//The Agent will destroy this observer after timeout.
	class SimpleNodeStateContainer : public TA_Base_Bus::ISimpleNodeObserverNotification
	{
	public:


		SimpleNodeStateContainer ( TA_Base_Bus::ISimpleNodeAgent * specificDataNodeAgent);
		virtual ~SimpleNodeStateContainer();

		//
		// Operations required by IDataNodeObserverNotification
		//

        virtual void processSimpleNodeUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> updateDetails );

		//
		// operations specific to DataNodeStateContainer
		//

	    /**
	      * addDisplayClient
	      * 
	      * added new client for the display
	      * 
	      * @return void 
	      * @param : unsigned long clientID
	      * @param : SimpleNodeDisplayClient * displayClient
	      * 
	      * @exception <exceptions> Optional
	      */
        void addDisplayClient(unsigned long clientID, SimpleNodeDisplayClient * displayClient);

	    /**
	      * removeDisplayClient
	      * 
	      * removed existing client for the display
	      * 
	      * @return void 
	      * @param : unsigned long clientID
	      * @param : SimpleNodeDisplayClient * displayClient
	      * 
	      * @exception <exceptions> Optional
	      */
        void removeDisplayClient(unsigned long clientID);

        /**
	      * >* DataNodeDisplayStateSequence
	      * 
	      * <description>
	      * 
	      * @return TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* 
	      * 
	      * @exception <exceptions> Optional
	      */
        TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* getAllSimpleNodeDisplayState();
        
	private:
        /**
          * removeDataNode
          *
          * This method removes DataNodes whose pkey matches with the specified key
		  * from the observer
		  *
		  * @param entityKey	The entity key of the SimpleNode to remove from the observer.
          *
          */
		void removeDataNode ( unsigned long entityKey );

		/**
          * findExistingDataNode
          *
          * This method finds and returns the reference to the data point whose pkey matches
		  * with the entity key specified in the parameter list
          *
          * @param entityKey	The entity key of the wanted SimpleNode
		  *
          */
		SimpleNode * findExistingDataNode ( unsigned long entityKey );

        void saveSimpleNodeUpdateDetails ( unsigned long pkey, const TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState & updateDetails );

        void registerForUpdates ( SimpleNode * dataNode );

        void loadAllSimpleNodes();
        	
		//TA_Base_Core::StopwatchUtil * m_purgeExistingUpdatesTimer;
		TA_Base_Bus::ISimpleNodeAgent * m_specificDataNodeAgent;

        TA_Base_Core::ReEntrantThreadLockable m_observedDataNodeListLock;
		std::map < unsigned long, SimpleNode * > m_listOfObservedDataNodes;
        TA_Base_Core::ReEntrantThreadLockable m_clientListLock;
        std::map < unsigned long, SimpleNodeDisplayClient * > m_listOfDataNodeClients;

        TA_Base_Core::ReEntrantThreadLockable m_listOfDataNodeDisplayStateLock;
		std::map < unsigned long, TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState * > m_listOfSimpleNodeDisplayState;

		SimpleNodeStateContainer() {};
		SimpleNodeStateContainer ( const SimpleNodeStateContainer & theInstance ) {};
		SimpleNodeStateContainer & operator= ( const SimpleNodeStateContainer & rhs ) {};
	};
}

#endif

