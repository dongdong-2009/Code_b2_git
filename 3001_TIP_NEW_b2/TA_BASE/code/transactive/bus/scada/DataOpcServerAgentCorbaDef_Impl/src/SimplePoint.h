/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/SimplePoint.h $
  * @author  J. Welton
  * @version $Revision: #6 $
  * Last modification : $DateTime: 2014/07/14 20:26:16 $
  * Last modified by : $Author: Noel $
  *
  *
  *	SimplePoint is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * SimplePoint also realises the SimplePointCorbaDef interface,
  * exposing the SimplePoint's configuration to remote
  * clients.
  *
  *
  * Modified: Daniel Hoey : 11/11/02 : Added isVirtual() method
  */

#ifndef SIMPLEPOINT_H
#define SIMPLEPOINT_H

#include <sys/timeb.h>
#include <iostream>
#include <string>
#include <vector>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/src/NameValuePair.h"

namespace TA_Base_Bus
{
    // forward declaration
	class ISimplePointObserverNotification;

	class SimplePoint //: public TA_Base_Core::IThreadPoolExecuteItem,
					  //public TA_Base_Core::IWorkItem
	{

	public:

		/**
  		 * SimplePoint::SimplePoint()
  		 *
  		 * SimplePoint, initialising all basic configuration items, activate its
		 * corba servant and register itself to naming service.
		 *
		 * @param	dataPointEntityAccess	Reference to db entity access
		 * @param	dpType					The type of the data point
		 * @param	dpDataType				The type of value being represented
		 * 									by the SimplePoint
         * @param   persistence             Interface for reading/writing persisted
         *                                  state values
		 *
  		 */
		SimplePoint( unsigned long key =0);  

		virtual ~SimplePoint();

		//
		// Operations specific to IWorkItemCallback
		//

		virtual void executeWorkItem();

		unsigned long getEntityKey() 
		{	
			m_entityKey = m_fieldValue.entityKey;
			return m_fieldValue.entityKey;
		}


	    /**
	      * * SimplePoint::getDisplayState
	      * 
	      * get display state
	      * 
	      * @return virtual TA_Base_Bus::DataPointDisplayState 
	      * 
	      */
        virtual boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> getDisplayStateSharedPtr();
		virtual TA_Base_Bus::DataPointDisplayState getDisplayState();

        //******************************************
        // register/de-register for the notifying datapoint details
        //******************************************
	    /**
	      * registerForUpdateDetails 
	      * 
	      * register for the notifying datapoint details
	      * 
	      * @return virtual bool 
	      * @param : TA_Base_Bus::IDataPointObserverNotification * updateDetailsObserver
	      * 
	      */
		virtual bool registerForUpdateDetails ( TA_Base_Bus::ISimplePointObserverNotification * updateDetailsObserver );

	    /**
	      * deregisterForUpdateDetails 
	      * 
	      * de-register for the notifying datapoint details
	      * 
	      * @return virtual bool 
	      * @param : TA_Base_Bus::IDataPointObserverNotification * updateDetailsObserver
	      * 
	      */
		virtual bool deregisterForUpdateDetails ( TA_Base_Bus::ISimplePointObserverNotification * updateDetailsObserver );

	    /**
	      * notifyRegisteredObservers 
	      * 
	      * notifyRegistered Observers
	      * 
	      * @return virtual void 
	      * @param : const TA_Base_Bus::DpDynamicSyncUpdate & updateDetails
	      * 
	      */
        virtual void notifyRegisteredObservers ( boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> updateDetails );
		

        //******************************************
        //"sets" method for the datapoint
        //******************************************
   		void setDisplayState(TA_Base_Bus::DataPointDisplayState dp);
	
    protected:

	
        /**
  		 * submitUpdate()
  		 *
  		 * Submits a new item to the EventQueue, requesting the
		 * UpdateEventProcessor to generate an event describing
		 * the SimplePoint's current state.
		 *
		 * @param type [IN]				The type of changes in data point internal state
		 * @param controlStatus [IN]	The control status of the data point, defaulted
		 *								to TA_Base_Bus::noControl
		 * @param configChanges [IN]	The list of configuration changes applied to
		 *								the data point
		 *
  		 */
		virtual void submitUpdate(  boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> );   // dpValueStatus
     
    protected:

	private:
	    //
		// Declare the copy constructor as private
		// to prevent pass-by-value of this monolithic object
		//
		SimplePoint ( const SimplePoint & theInstance );
		SimplePoint & operator= ( const SimplePoint & rhs );

        void setDefaults();
		void setToControlMode();
		void setToMonitorMode();
		void setToStandbyMode();

	private:

		TA_Base_Core::ReEntrantThreadLockable m_lock;
		TA_Base_Core::ReEntrantThreadLockable m_updateDetailsObserverlock;

    protected:
		TA_Base_Bus::DataPointDisplayState			m_fieldValue;
		unsigned long								m_entityKey;
		TA_Base_Bus::EDataPointServiceState 		m_serviceState;
		TA_Base_Core::EOperationMode m_operationMode, m_lastSavedOperationMode;


		std::vector < ISimplePointObserverNotification * > m_listOfUpdateDetailsObservers;
		
	};

} // close namespace TA_Base_Bus

#endif
