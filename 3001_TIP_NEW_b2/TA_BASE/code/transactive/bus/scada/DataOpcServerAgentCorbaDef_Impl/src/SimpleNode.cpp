/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datanode_library/src/SimpleNode.cpp $
  * @author:  Ripple
  * @version: $Revision: #9 $
  *
  * Last modification: $DateTime: 2014/12/24 10:15:19 $
  * Last modified by:  $Author: haijun.li $
  *
  * SimpleNode is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * SimpleNode also realises the DataNodeCorbaDef interface,
  * exposing the SimpleNode's configuration to remote
  * clients.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "SimpleNode.h"
#include "ISimpleNodeObserverNotification.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	SimpleNode::SimpleNode( unsigned long key)
    :
	m_operationMode(TA_Base_Core::Monitor),m_entityKey(key)
    {
        FUNCTION_ENTRY( "SimpleNode()" );
		setDefaults();
        FUNCTION_EXIT;
    }

    SimpleNode::~SimpleNode()
    {
        FUNCTION_ENTRY( "~SimpleNode()" );
        FUNCTION_EXIT;
    }


    TA_Base_Core::EOperationMode SimpleNode::getOperationMode()
    {
        return m_operationMode;
    }


    bool SimpleNode::changeOperationMode(TA_Base_Core::EOperationMode operationMode)
    {
        FUNCTION_ENTRY( "changeOperationMode()" );
		if (m_operationMode != operationMode)
        {
			m_operationMode = operationMode;

			// TD18751
			// if there are observers, need to notify them with complete dynamic infomation when change to control
			// to do this is because the local proxies will not be refreshed when change to control.
			if (TA_Base_Core::Control == m_operationMode)
			{
				boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> dnupdate(new DataNodeDisplayState(m_dynamicState));
				//dnupdate = m_dynamicState;
				notifyRegisteredObservers(dnupdate);
			}
			FUNCTION_EXIT;
			return true;
        }
		else
		{
			FUNCTION_EXIT;
			return false;
		}
    }

    void SimpleNode::checkOperationMode()
    {
        if (m_operationMode != TA_Base_Core::Control)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s - in monitor mode", m_staticConfig.name.c_str() );

            throw TA_Base_Core::OperationModeException();
        }
    }

    void SimpleNode::setDefaults()
    {
        FUNCTION_ENTRY( "setDefaults()" );
		//
		// Initialize SimpleNode Configuration first
		//
		m_dynamicState.entityKey = m_entityKey;
		m_dynamicState.ONITP = "";

		m_dynamicState.alarmState.value = false;
        m_dynamicState.alarmState.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

		m_dynamicState.alarmAckState.value = QUALITY_BAD;
        m_dynamicState.alarmAckState.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

		m_dynamicState.quality.value = QUALITY_BAD;
        m_dynamicState.quality.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        FUNCTION_EXIT;
    }

    bool SimpleNode::registerForUpdateDetails ( TA_Base_Bus::ISimpleNodeObserverNotification * updateDetailsObserver )
    {
		FUNCTION_ENTRY ("registerForUpdateDetails()");

        TA_THREADGUARD( m_updateDetailsObserverlock );

        bool result = false;

        if ( 0 != updateDetailsObserver )
        {
            bool existed = false;

            std::vector < TA_Base_Bus::ISimpleNodeObserverNotification * >::iterator itr;
            for ( itr = m_listOfUpdateDetailsObservers.begin();
                  itr != m_listOfUpdateDetailsObservers.end();
                  itr++ )
            {
                if ( updateDetailsObserver == (*itr) )
                {
                    existed = true;
                    break;
                }
            }

            if ( false == existed )
            {
                m_listOfUpdateDetailsObservers.push_back ( updateDetailsObserver );
            }

            result = true;
        }

		FUNCTION_EXIT;
        return result;
    }

    bool SimpleNode::deregisterForUpdateDetails ( TA_Base_Bus::ISimpleNodeObserverNotification * updateDetailsObserver )
    {
		FUNCTION_ENTRY ("deregisterForUpdateDetails()");
  
        TA_THREADGUARD( m_updateDetailsObserverlock );

        bool result = false;

        if ( 0 != updateDetailsObserver )
        {
            bool existed = false;

            std::vector < TA_Base_Bus::ISimpleNodeObserverNotification * >::iterator itr;
            for ( itr = m_listOfUpdateDetailsObservers.begin();
                  itr != m_listOfUpdateDetailsObservers.end();
                  itr++ )
            {
                if ( updateDetailsObserver == (*itr) )
                {
                    existed = true;
                    break;
                }
            }

            if ( true == existed )
            {
                m_listOfUpdateDetailsObservers.erase ( itr );

                result = true;
            }
        }

		FUNCTION_EXIT;
        return result;
    }
 
	void SimpleNode::setNodeState(TA_Base_Bus::DataNodeDisplayState dn)
	{
		{
			TA_Base_Core::ThreadGuard guard( m_updateLock );
			m_dynamicState = dn;
			notifyRegisteredObservers(getNodeStateSharedPtr());
		}
	}

	 boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> SimpleNode::getNodeStateSharedPtr()
	 {
		FUNCTION_ENTRY("getNodeStateSharedPtr");

        boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> dnSharedPtr(new TA_Base_Bus::DataNodeDisplayState);

		dnSharedPtr->entityKey = m_dynamicState.entityKey;
		dnSharedPtr->ONITP = m_dynamicState.ONITP;

		dnSharedPtr->alarmState.value = m_dynamicState.alarmState.value;
        dnSharedPtr->alarmState.quality = m_dynamicState.alarmState.quality;

		dnSharedPtr->alarmAckState.value = m_dynamicState.alarmAckState.value;
        dnSharedPtr->alarmAckState.quality = m_dynamicState.alarmAckState.quality;

		dnSharedPtr->quality.value = m_dynamicState.quality.value;
        dnSharedPtr->quality.quality = m_dynamicState.quality.quality;

		FUNCTION_EXIT;
		return dnSharedPtr;
	 
	 }

    void SimpleNode::notifyRegisteredObservers ( boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> updateDetails )
    {
		FUNCTION_ENTRY ("notifyRegisteredObservers()");

        TA_THREADGUARD( m_updateDetailsObserverlock );

        std::vector < TA_Base_Bus::ISimpleNodeObserverNotification * >::iterator itr;
        for ( itr = m_listOfUpdateDetailsObservers.begin();
              itr != m_listOfUpdateDetailsObservers.end();
              itr++ )
        {
            try
            {
				//TO DO --- CONVERT :: updateDetails into convertedUpdates ;
	
                (*itr)->processSimpleNodeUpdate ( m_dynamicState.entityKey, updateDetails );
            }
            catch ( ... )
            {
            }
        }

		FUNCTION_EXIT;
    }

    unsigned long SimpleNode::getEntityKey() const
    {
        return m_dynamicState.entityKey;
    }

    const std::string& SimpleNode::getONITP() const
    {
        return m_dynamicState.ONITP;
    }

    TA_Base_Bus::DataNodeDisplayState SimpleNode::getDisplayState()
    {
		FUNCTION_ENTRY("getDisplayState");
		FUNCTION_EXIT;
        return m_dynamicState;
    }

    void SimpleNode::setOperationMode( bool inControlMode )
    {
        std::ostringstream fnName;
        fnName << "setOperationMode() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        changeOperationMode( inControlMode ? TA_Base_Core::Control : TA_Base_Core::Monitor );

        FUNCTION_EXIT;
    }

}
