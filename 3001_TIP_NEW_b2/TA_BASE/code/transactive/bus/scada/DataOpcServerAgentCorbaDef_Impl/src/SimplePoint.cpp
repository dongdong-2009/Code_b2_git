/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/SimplePoint.cpp $
  * @author:	HoaVu
  * @version $Revision: #11 $
  *
  * Last modification : $DateTime: 2014/07/17 10:21:08 $
  * Last modified by : $Author: haijun.li $
  *
  *
  * SimplePoint is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * SimplePoint also realises the DataPointCorbaDef interface,
  * exposing the SimplePoint's configuration to remote
  * clients.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4503)
#endif // defined _MSC_VER

#include "SimplePoint.h"
#include "ISimplePointObserverNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "time.h"
#include <algorithm>
#include <float.h>

using namespace TA_Base_Core;
using namespace TA_Base_Bus;


//bool isSystemSessionID( const std::string& sessionID )//limin++, SystemSession should not access db
//{
//    bool ret = ( TA_Base_Core::SessionAccessFactory::getSuperSessionId() == sessionID ||
//                 TA_Base_Bus::DPT_INTERNAL_SESSION_ID == sessionID ||
//                 TA_Base_Bus::NotLoaded == sessionID );
//    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[sessionID=%s][return=%d]", sessionID.c_str(), ret );
//    return ret;
//}


namespace TA_Base_Bus
{
	SimplePoint::SimplePoint(unsigned long key ):    m_serviceState( OUT_OF_SERVICE ),m_entityKey(key),
    m_operationMode( TA_Base_Core::NotApplicable )
	{
        FUNCTION_ENTRY("SimplePoint");
		m_serviceState = IN_SERVICE;

		setDefaults();
        
		//TD17119
        FUNCTION_EXIT;
	}


	SimplePoint::~SimplePoint()
	{
        FUNCTION_ENTRY("~SimplePoint");
		// remove handle to the ThreadPoolSingletonManager
		
		// clean up the list of dp observers
		m_listOfUpdateDetailsObservers.clear();

        FUNCTION_EXIT;
	}

	TA_Base_Bus::DataPointDisplayState SimplePoint::getDisplayState()
	{
		return m_fieldValue;
	}

   	void SimplePoint::setDisplayState(TA_Base_Bus::DataPointDisplayState dp)
	{
		{
			TA_Base_Core::ThreadGuard guard( m_lock );
			m_fieldValue = dp;
			// notify all registered observers of the latest changes
			notifyRegisteredObservers ( getDisplayStateSharedPtr() );
		}
	}

	boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> SimplePoint::getDisplayStateSharedPtr()
    {
		FUNCTION_ENTRY("getDisplayStateSharedPtr");

        boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> dpDisplayState(new TA_Base_Bus::DataPointDisplayState);

        //return the entitykey
		dpDisplayState->entityKey = m_fieldValue.entityKey;
        //set the dp value
		dpDisplayState->value.dpType = m_fieldValue.value.dpType;
        switch(m_fieldValue.value.dpType)
		{
		case DPT_BOOL_TYPE:
			dpDisplayState->value.booleanValue = m_fieldValue.value.booleanValue;
			break;

		case DPT_ENUM_TYPE:
			dpDisplayState->value.enumValue = m_fieldValue.value.enumValue;
			break;

		case DPT_TEXT_TYPE:
			dpDisplayState->value.stringValue = m_fieldValue.value.stringValue;
			break;

		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			dpDisplayState->value.floatValue = m_fieldValue.value.floatValue;
			break;

		case DPT_NULL_TYPE:
		default:
			break;	
		}

        // convert enum alarmState to boolean representation of alarmState
		dpDisplayState->alarmState = m_fieldValue.alarmState;
		dpDisplayState->alarmAckState = m_fieldValue.alarmAckState;
		dpDisplayState->dataPointStatus = m_fieldValue.dataPointStatus;

		FUNCTION_EXIT;
		return dpDisplayState;
    }

	void SimplePoint::setToControlMode()
	{
        FUNCTION_ENTRY("setToControlMode");
		// save the operation mode
		m_operationMode = TA_Base_Core::Control;

		// initialise data if not done so
		if ( m_lastSavedOperationMode != m_operationMode )
		{
			// save current control mode for later usage
			m_lastSavedOperationMode = m_operationMode;


			boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> dpDUpdate(new DataPointDisplayState);

            notifyRegisteredObservers(dpDUpdate);
        }

        FUNCTION_EXIT;
	}


	void SimplePoint::setToMonitorMode()
	{
        FUNCTION_ENTRY("setToMonitorMode");
		// save the operation mode
		m_operationMode = TA_Base_Core::Monitor;

        // if there is difference between last and current control mode
        if ( m_lastSavedOperationMode != m_operationMode )
        {
            //do not remove observers, they will call unregister to remove by themself
		    //{
		    //	// stop mutil thread modifing the list of observers
		    //	ThreadGuard guard( m_updateDetailsObserverlock );
            //
		    //	// clean up the list of dp observers
		    //	m_listOfUpdateDetailsObservers.clear();
		    //}

		    // save current control mode for later usage
		    m_lastSavedOperationMode = m_operationMode;
        }

        FUNCTION_EXIT;
	}


	void SimplePoint::setToStandbyMode()
	{
		FUNCTION_ENTRY("setToMonitorMode");
		// save the operation mode
		m_operationMode = TA_Base_Core::Standby;

        // if there is difference between last and current control mode
        if ( m_lastSavedOperationMode != m_operationMode )
        {
            //do not remove observers, they will call unregister to remove by themself
		    //{
		    //	// stop mutil thread modifing the list of observers
		    //	ThreadGuard guard( m_updateDetailsObserverlock );
            //
		    //	// clean up the list of dp observers
		    //	m_listOfUpdateDetailsObservers.clear();
		    //}

		    // save current control mode for later usage
		    m_lastSavedOperationMode = m_operationMode;
           
        }

        FUNCTION_EXIT;
	}
  
	void SimplePoint::setDefaults()
	{
        FUNCTION_ENTRY("setDefaults");
		m_fieldValue.entityKey = m_entityKey;
		m_fieldValue.alarmAckState = ScadaAlarmNotPresent;//which is 2, ScadaAlarmNotPresent
		m_fieldValue.alarmState = false;
		m_fieldValue.dataPointStatus = 0;
		m_fieldValue.value.dpType = DPT_BOOL_TYPE;
		m_fieldValue.value.booleanValue = false;
		m_fieldValue.value.enumValue = 0;
		m_fieldValue.value.floatValue = 0;
		//m_fieldValue.value.stringValue = "";
        FUNCTION_EXIT;
	}

	bool SimplePoint::registerForUpdateDetails ( TA_Base_Bus::ISimplePointObserverNotification * updateDetailsObserver )
	{
        FUNCTION_ENTRY("registerForUpdateDetails");
		// stop mutil thread modifing the list of observers
		ThreadGuard guard( m_updateDetailsObserverlock );

		bool result = false;

		// only proceed if there is a valid observer
		if ( 0 != updateDetailsObserver )
		{
			bool existed = false;

			// check if we already have this observer in the list
			std::vector < TA_Base_Bus::ISimplePointObserverNotification * >::iterator itr;
			for ( itr = m_listOfUpdateDetailsObservers.begin();
				  itr != m_listOfUpdateDetailsObservers.end();
				  itr++ )
			{
				// if found a match
				if ( updateDetailsObserver == (*itr) )
				{
					// set the flag and exit loop
					existed = true;
					break;
				}
			}

			// if found no match
			if ( false == existed )
			{
				// put the observer to the list for later usage
				m_listOfUpdateDetailsObservers.push_back ( updateDetailsObserver );
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Observer [%u] is added into observer list of SimplePoint ", updateDetailsObserver);
            }
			// else do nothing if the specified observer already in the list

			result = true;
		}
		// else do nothing if the specified observer is null
        FUNCTION_EXIT;
		return result;
	}


	bool SimplePoint::deregisterForUpdateDetails ( TA_Base_Bus::ISimplePointObserverNotification * updateDetailsObserver )
	{
        FUNCTION_ENTRY("deregisterForUpdateDetails");
		// stop mutil thread modifing the list of observers
		ThreadGuard guard( m_updateDetailsObserverlock );

		bool result = false;

		// only proceed if there is a valid observer
		if ( 0 != updateDetailsObserver )
		{
			bool existed = false;

			// check if we already have this observer in the list
			std::vector < TA_Base_Bus::ISimplePointObserverNotification * >::iterator itr;
			for ( itr = m_listOfUpdateDetailsObservers.begin();
				  itr != m_listOfUpdateDetailsObservers.end();
				  itr++ )
			{
				// if found a match
				if ( updateDetailsObserver == (*itr) )
				{
					// set the flag and exit loop
					existed = true;
					break;
				}
			}

			// if found a match
			if ( true == existed )
			{
				// remove the observer from the list
				m_listOfUpdateDetailsObservers.erase ( itr );
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Observer [%u] is removed from observer list of SimplePoint ", updateDetailsObserver);
                
				result = true;
			}
			// else do nothing is the specified observer is not in the list
		}
		// else do nothing if the specified observer is null
        FUNCTION_EXIT;
		return result;
	}


	void SimplePoint::notifyRegisteredObservers ( boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> updateDetails )
	{
        FUNCTION_ENTRY("notifyRegisteredObservers");
		// stop mutil thread modifing the list of observers
		ThreadGuard guard( m_updateDetailsObserverlock );

		// for each of the registered observers in the list
		std::vector < TA_Base_Bus::ISimplePointObserverNotification * >::iterator itr;
		for ( itr = m_listOfUpdateDetailsObservers.begin();
			  itr != m_listOfUpdateDetailsObservers.end();
			  itr++ )
		{
			try
			{
				(*itr)->processSimplePointUpdate ( m_entityKey, updateDetails );

				// allow time for the observers to do their updates
				//TA_Base_Core::Thread::sleep ( 10 );
			}
			catch ( ... )
			{
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught UnknownException when notify observer [%u] of SimplePoint ", (*itr) );
			}
		}
        FUNCTION_EXIT;
	}
    

    void SimplePoint::submitUpdate(boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> dpState )
	{
        FUNCTION_ENTRY("submitUpdate");

		// only if the data point is in control mode
		if ( TA_Base_Core::Control == m_operationMode )
		{
			            

			// notify all registered observers of the latest changes
			notifyRegisteredObservers ( dpState );
		
		}
	    FUNCTION_EXIT;
	}

	void SimplePoint::executeWorkItem()
	{
        FUNCTION_ENTRY("executeWorkItem");
		//
		// callback from ThreadPool for scheduled work
		//
		boost::shared_ptr<TA_Base_Bus::DataPointDisplayState> dpState;
		///dpState-> m_fieldValue
		submitUpdate(dpState);
        FUNCTION_EXIT;
	}

}
