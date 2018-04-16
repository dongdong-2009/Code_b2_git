/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataPointAgentCorbaDef_Impl/src/OpcSimplePointAgentCorbaDef_Impl.h $
  * @author  HoaVu
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2014/07/11 18:24:37 $
  * Last modified by : $Author: haijun.li $
  *
  *	OpcSimplePointAgentCorbaDef_Impl is an abstract base class that implements
  * the DataPointAgentCorbaDef interface, providing SimplePoint configurations
  * and updates to remote clients.
  *
  */

#ifndef OPC_DATAPOINT_CORBA_IMPL_H
#define OPC_DATAPOINT_CORBA_IMPL_H

#include <map>
#include <sys/timeb.h>

#include "core/corba/src/ServantBase.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"
#include "core/timers/src/SingletonTimerUtil.h"

#include "bus/scada/common_library/IDL/src/ScadaCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/common_library/IDL/src/IDataPointAgentCorbaDef.h"


namespace TA_Base_Bus
{
	class SimplePointObserver;
	class SimplePoint;
    class SimplePointDisplayClient;
    class SimplePointFactory;

	class SimplePointStateContainer;
	class ISimplePointAgent;

	class OpcSimplePointAgentCorbaDef_Impl : public virtual TA_Base_Core::ServantBase,
										public virtual POA_TA_Base_Bus::IDataPointAgentCorbaDef,
										public virtual TA_Base_Core::AbstractThreadedTimeoutUser
	{
	public:


		OpcSimplePointAgentCorbaDef_Impl ( const std::string & agentName,
									  TA_Base_Bus::ISimplePointAgent * specificSimplePointAgent );


		virtual ~OpcSimplePointAgentCorbaDef_Impl();

		//
		// Operations required by TA_Base_Bus::DataPointAgentCorbaDef interface
		//
		virtual CORBA::ULong createObserver();
		virtual void addToObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& dpKeys);
		virtual void removeFromObserver(CORBA::ULong observerId, const ScadaCorbaTypes::KeySequence& dpKeys);
		virtual void destroyObserver(CORBA::ULong observerId);
		virtual DataPointCorbaTypes::DynamicSyncUpdateSequence* getUpdates(CORBA::ULong observerId);
		//for opc bridge use 
		virtual CORBA::ULong regiesterDisplayDataPointClient();
		virtual void unregiesterDisplayDataPointClient(CORBA::ULong cleintId);
		virtual DataPointCorbaTypes::DisplayStateSequence* getDisplayDataPointUpdate(CORBA::ULong clientId, CORBA::Boolean getall);

        //Mintao++ for display datapoint only
    	SimplePointDisplayClient * findDisplayClient ( unsigned long clientId );
        //Mintao++ for display datapoint only
		//
		// Operations required by SimplePoint synchronization
		//
		//regiesterDataPointSynState
		/**
		  * regiesterDataPointSynState
		  *
		  * This method requests the Agent to create an registered client and if successful
		  * returns a non-zero registered client id.
		  *
		  * @return The id of the observer that has been created
		  *
		  * @exception			TA_Base_Core::OperationModeException raised if the agent is in Monitor mode.
		  */
		virtual CORBA::ULong regiesterDataPointSynState();
		
		/**
		 * setToControl
		 *
		 * This method let the OpcSimplePointAgentCorbaDef_Impl instance knows that it
		 * must activate its servant base and start receiving incoming requests
		 *
		 */
		virtual void setToControl();


		/**
		 * setToMonitor
		 *
		 * This method let the OpcSimplePointAgentCorbaDef_Impl instance knows that it
		 * must deactivate its servant base and stop receiving incoming requests
		 *
		 */
		virtual void setToMonitor();

		/**
		 * setToStandby
		 *
		 * This method let the OpcDataNodeAgentCorbaDef_Impl instance knows that it
		 * must deactivate its servant base and stop receiving incoming requests
		 *
		 */
		virtual void setToStandby();
	
		virtual void setOperationMode(TA_Base_Core::EOperationMode mode);

		//
		// Operations required by ITimeoutCallback
		//
	
		/**
		*
         * threadedTimerExpired
		*
		* To be implemented by users of TimerUtil.  This operation will be called
		* when the registered timeout period has expired.
		*
		*/
		virtual void threadedTimerExpired(long timerId, void* userData);
        
	private:

        OpcSimplePointAgentCorbaDef_Impl() {};
		OpcSimplePointAgentCorbaDef_Impl ( const OpcSimplePointAgentCorbaDef_Impl & theInstance ) {};
		OpcSimplePointAgentCorbaDef_Impl & operator= ( const OpcSimplePointAgentCorbaDef_Impl & rhs ) {};

		/**
		 * generateNextObserverID
		 *
		 * Returns the next observer ID
		 *
		 */
		unsigned long generateNextObserverID();

		/**
		 * findObserver
		 *
		 * Returns a pointer to the existing observer whose ID matches with the ID specified
		 * in the parameter list.  A null pointer is returned if no match is found
		 *
		 * @param	observerId	The ID of the wanted observer
		 *
		 */
		SimplePointObserver * findObserver ( unsigned long observerId );

		/**
		 * generateNextClientID
		 *
		 * Returns the next client ID
		 *
		 */
	    unsigned long generateNextClientID();

	    /**
	      * cleanupDisplayContainer
	      * 
	      *  clean up the display container
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        void cleanupDisplayContainer();

		void cleanupObserversAndClients();

		std::string m_agentName;
		TA_Base_Core::EOperationMode m_operationMode;
		unsigned long m_generatedObservedID;
		TA_Base_Bus::ISimplePointAgent * m_specificAgent;
		std::map < unsigned long, SimplePointObserver * > m_listOfObservers;
		TA_Base_Core::ReEntrantThreadLockable m_observerListLock;

		unsigned long m_generatedClientID;
		std::map < unsigned long, SimplePointDisplayClient * > m_listOfDisplayClients;
		TA_Base_Core::ReEntrantThreadLockable m_clientListLock;
        SimplePointStateContainer * m_stateContainer;

	};
}

#endif

