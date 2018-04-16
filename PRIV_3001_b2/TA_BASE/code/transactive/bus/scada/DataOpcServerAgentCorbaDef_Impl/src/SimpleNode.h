/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datanode_library/src/SimpleNode.h $
  * @author:  Ripple
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2014/10/27 14:34:18 $
  * Last modified by:  $Author: haijun.li $
  *
  *	SimpleNode is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * SimpleNode also realises the DataNodeCorbaDef interface,
  * exposing the SimpleNode's configuration to remote 
  * clients.
  *
  *
  * Modified: Daniel Hoey : 11/11/02 : Added isVirtual() method
  */

#ifndef SIMPLENODE_H
#define SIMPLENODE_H

#include <string>
#include <vector>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_Base_Core
{
	class ConfigUpdateDetails;
}

namespace TA_Base_Bus
{
	class ISimpleNodeObserverNotification;

    class SimpleNode //: public IEntityUpdateEventProcessor
	{

	public:

		/**
  		 * SimpleNode::SimpleNode()
  		 *
  		 * SimpleNode, initialising all basic configuration items.
		 *
		 * @param	DataNodeEntityAccess	Reference to db entity access
		 *
  		 */
		SimpleNode( unsigned long key =0);

		virtual ~SimpleNode();	
		// ========================================================================	
		// Implementation of IEntityUpdateEventProcessor
		void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType);

		//
		// Implementation to support SimpleNode Agent BulkPolledInterface
		//
		virtual bool registerForUpdateDetails( TA_Base_Bus::ISimpleNodeObserverNotification * updateDetailsObserver );
		virtual bool deregisterForUpdateDetails( TA_Base_Bus::ISimpleNodeObserverNotification * updateDetailsObserver );
		virtual void notifyRegisteredObservers( boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> updateDetails );

		//
		// access methods of SimpleNode
		//
	    /**
	      * getEntityKey
	      * 
	      * return the entitykey
	      * 
	      * @return unsigned long 
	      * 
	      */
		unsigned long getEntityKey() const;

	    /**
	      * getName
	      * 
	      * return the name of the datanode
	      * 
	      * @return const std::string& 
	      * 
	      * @exception <exceptions> Optional
	      */
        const std::string& getONITP() const;

	    /**
	      * getCompleteDynamicStates
	      * 
	      * return the complete dynamic states
	      * 
	      * @return TA_Base_Bus::DnCompleteDynamicSyncState& 
	      * 
	      */
        boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> getCompleteDynamicStates();
	    /**
	      * getDisplayState
	      * 
	      * <description>
	      * 
	      * @return virtual boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual TA_Base_Bus::DataNodeDisplayState getDisplayState();

        /**
	      * setOperationMode
	      * 
	      * set the operation mode of the datanode. This must be called by the datanode agent
	      * 
	      * @return virtual void 
	      * @param : bool inControlMode
	      * 
	      */
        virtual void setOperationMode( bool inControlMode );

		unsigned long getKey(){ return m_entityKey;}

		bool changeOperationMode( TA_Base_Core::EOperationMode operationMode ); 
        //******************************************
        //"sets" method for the datapoint
        //******************************************
   		void setNodeState(TA_Base_Bus::DataNodeDisplayState dp);

    protected:
		boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> getNodeStateSharedPtr();

        /**
	      * readStaticConfiguration
	      * 
	      * read the configuration from the database
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
		void readStaticConfiguration( );

	    /**
	      * updateConfiguration
	      * 
	      * update the configuration 
	      * 
	      * @return void 
	      * 
	      */
		void updateStaticConfiguration( );

	    /**
	      * readDynamicState
	      * 
	      * read dynamic state of the datanode
	      * 
	      * @return void 
	      * 
	      */
		void readDynamicState();

		/**
  		 * SimpleNode::submitUpdateEvent()
  		 *
  		 * Submits a new item to the EventQueue, requesting the 
		 * EventProcessor to generate an event describing
		 * the SimpleNode's current state.
		 *
  		 */
		void submitUpdateEvent(ScadaEntityUpdateType type);

		// ========================================================================
		// Operation mode related methods  
		//
		TA_Base_Core::EOperationMode getOperationMode();
		void checkOperationMode();
	    void setDefaults();		
	protected:


		// Configuration
        TA_Base_Bus::DataNodeStaticConfiguration	m_staticConfig;
		
		//Dynamic information
		//TA_Base_Bus::DnCompleteDynamicSyncState	    m_dynamicState;
		TA_Base_Bus::DataNodeDisplayState	m_dynamicState;

		// Internal stuff...
		TA_Base_Core::EOperationMode		        m_operationMode;
		unsigned long								m_entityKey;
		std::vector < TA_Base_Bus::ISimpleNodeObserverNotification * > m_listOfUpdateDetailsObservers;
		TA_Base_Core::ReEntrantThreadLockable		m_updateDetailsObserverlock;
		TA_Base_Core::ReEntrantThreadLockable		m_updateLock;
		//TA_Base_Bus::DataNodeProxySmartPtr*			m_parentProxy;
	
	private:
		//
		// Declare the copy constructor as private 
		// to prevent pass-by-value of this monolithic object
		//
		SimpleNode( const SimpleNode & );
	};
}
#endif
