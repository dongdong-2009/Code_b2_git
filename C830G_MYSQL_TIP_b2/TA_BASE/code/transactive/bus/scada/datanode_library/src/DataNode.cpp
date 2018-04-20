/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * DataNode is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * DataNode also realises the DataNodeCorbaDef interface,
  * exposing the DataNode's configuration to remote
  * clients.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datanode_library/src/IDataNodeObserverNotification.h"
#include "bus/scada/datanode_library/src/DataNodeStates.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/DataNodeStateUpdate_MessageTypes.h"
#include "core/message/types/DataNodeAudit_MessageTypes.h"
#include "core/naming/src/Naming.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    DataNode::DataNode( TA_Base_Core::DataNodeEntityDataPtr dataNodeEntityAccess, ScadaPersistencePtr persistence )
    :
    m_dataNodeEntityAccess(dataNodeEntityAccess),
    m_scadaProxyFactory(TA_Base_Bus::ScadaProxyFactory::getInstance()),
    m_summaryQ( NULL ),
    m_auditSender( NULL ),
    m_persistence( persistence )
    {
        std::ostringstream fnName;
        fnName << "DataNode() - " << m_dataNodeEntityAccess->getName();
        FUNCTION_ENTRY( fnName.str().c_str() );

        //
        // Initialize Operation Mode and Local Message Receiver
        //
        m_operationMode = TA_Base_Core::Monitor;

        //
        // Initialize DataNode Configuration
        //
        m_staticConfig.name = "";
        m_staticConfig.description = "";
        m_staticConfig.entityKey = 0;
        m_staticConfig.subsystemKey = 0;
        m_staticConfig.locationKey = 0;
        m_staticConfig.agentName = "";
        m_staticConfig.label = "";
        m_staticConfig.shortLabel = "";
        m_staticConfig.assetName = "";
        m_staticConfig.displayOrder = 0 ;

        //
        // Retrieve the DataNode configuration
        //
        readConfiguration();
        
        //
        // Initialise DataNode State
        //
        // Default initialisation
        m_dynamicState.tagState.tagState = TA_Base_Bus::NotTagged;
        m_dynamicState.tagState.tagInhibitState.inputInhibit = TA_Base_Bus::NoInputInhibit;
        m_dynamicState.tagState.tagInhibitState.outputInhibit = TA_Base_Bus::NoOutputInhibit;
        m_dynamicState.tagState.ptwId = "";
        m_dynamicState.tagState.comment = "";
        m_dynamicState.notes = "";
        m_dynamicState.inhibitMMSState = false;
        m_dynamicState.lastOperator.sessionID = "";
        m_dynamicState.lastOperator.timeStamp.time = 0;

        //
        // Retrieve the DataNode State
        //
        readState();
    
        //
        // Clear the summaries
        //
        clearSummaries();

        //
        // Set up the proxies
        // 
        

        //
        // Get a pointer to the Summary queue
        //
        m_summaryQ = SummaryProcessor::getInstance();

        updateActivation();
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "DataNode() - constructed %s",
            m_staticConfig.name.c_str() );

        FUNCTION_EXIT;
    }


    DataNode::~DataNode()
    {
        std::ostringstream fnName;
        fnName << "~DataNode() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        SummaryProcessor::removeInstance(m_summaryQ);
        m_summaryQ = NULL;

        {
            // Destroy the proxies
            destroyProxies();
        }
        
        //
        // Remove queue instances
        //

        if (m_auditSender != NULL)
        {
            delete m_auditSender;
            m_auditSender = NULL;
        }

        // remove handle to ScadaProxyFactory
        TA_Base_Bus::ScadaProxyFactory::removeInstance();

		//remove handle to ScadaUtilities
		TA_Base_Bus::ScadaUtilities::removeInstance();

        FUNCTION_EXIT;
    }

    //
    //  IEntity Interface Implementation
    //

    /**
      * isValid
      *
      * This is part of the implementation of the IEntity Interface
      * This method returns true if the DataNode entity is 
      * valid. Otherwise it will return false.
      *
      * @return TRUE if the entity is valid, FALSE otherwise 
      */
    bool DataNode::isValid()
    {
        return true;
    }

    
    /**
      * start
      *
      * This is part of the implementation of the IEntity Interface
      * This method must start any object specific tasks,
      * such as creating its own thread.
      */
    void DataNode::start()
    {
    }

    /**
      * stop
      *
      * This is part of the implementation of the IEntity Interface
      * This method will stop any object specific tasks, such 
      * as stopping its own thread. 
      */
    void DataNode::stop()
    {
    }

    /**
      * update
      *
      * This is part of the implementation of the IEntity Interface
      * This method is called by the agent controlling the entity to ask 
      * the entity to update its configuration. Called after an update 
      * configuration event is received.
      */
    void DataNode::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        updateConfiguration();
    }

    /**
      * remove
      *
      * This is part of the implementation of the IEntity Interface
      * When a configuration update event is received to delete an entity 
      * from an agent, the agent can call this method to tell the entity 
      * to commence any clean up opperations.
      * 
      * The agent then still has to remove the object from its vector of entities.
      */
    void DataNode::remove()
    {
    }

    /**
      * setToControlMode
      *
      * This is part of the implementation of the IEntity Interface
      * This method can be called by the agent responsible for the entity in
      * the event of its mode of operation changing to control. This enables the
      * entity to perform tasks such as connecting to any field devices it is
      * responsible for communicating with.
      */
    void DataNode::setToControlMode()
    {
        std::ostringstream fnName;
        fnName << "setToControlMode() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        //dont assert, just ingore the command
        //TA_ASSERT( m_operationMode == TA_Base_Core::Control,
        //    "DataNode needs to be set to control mode in DataNodeAgent::agentSetControl" );

        FUNCTION_EXIT;
    }


    /**
      * setToMonitorMode
      *
      * This is part of the implementation of the IEntity Interface
      * This method can be called by the agent responsible for the entity in
      * the event of its mode of operation changing to monitor. This enables the
      * entity to perform tasks such as disconnecting from any field devices it was
      * communicating with.
      */
    void DataNode::setToMonitorMode()
    {
        std::ostringstream fnName;
        fnName << "setToMonitorMode() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        //dont assert, just ingore the command
        //TA_ASSERT( m_operationMode == TA_Base_Core::Monitor,
        //    "DataNode needs to be set to monitor mode in DataNodeAgent::agentSetMonitor" );

        FUNCTION_EXIT;
    }


    TA_Base_Core::EOperationMode DataNode::getOperationMode()
    {
        return m_operationMode;
    }


    bool DataNode::changeOperationMode(TA_Base_Core::EOperationMode operationMode)
    {

        if (m_operationMode == operationMode)
        {
            // Already in this Mode
            return false;
        }

        // save the operation mode
        m_operationMode = operationMode;

        updateActivation();

        return true;
    }

    void DataNode::checkOperationMode()
    {

        if (m_operationMode != TA_Base_Core::Control)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s - in monitor mode", m_staticConfig.name.c_str() );

            throw TA_Base_Core::OperationModeException();
        }
    }

    void DataNode::processProxyUpdate(ScadaEntityUpdateType updateType)
    {
        std::ostringstream fnName;
        fnName << "processProxyUpdate() - " << m_staticConfig.name;
        fnName << " - type: " << ((int)updateType);
        FUNCTION_ENTRY( fnName.str().c_str() );

        TA_THREADGUARD(m_childproxieslock);

        // process proxy update even in Monitor Mode for failover.
        /*if (false == isActivated())
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Not processing datanode summary for %s, not activated.",
                m_staticConfig.name.c_str() );

            FUNCTION_EXIT;
            return;
        }*/

		//xinsong++
		bool fRet = m_childProxies.checkConfigValidForChildDataPoints(m_staticConfig.name);
		if ( !fRet )
		{
			FUNCTION_EXIT;
			return;
		}
		fRet = m_childProxies.checkConfigValidForChildDataNodes(m_staticConfig.name);
		if ( !fRet )
		{
			FUNCTION_EXIT;
			return;
		}

        // all valid config, can now process summaries
        switch ( updateType )
        {
        // DataPoints
        case ValueStateUpdate:
            //TD19046 no need to update DataNode when DP value changes
            recalculateQualitySummary();
            recalculateAlarmSummary();
            //recalculateAlarmAckSummary();
            recalculateOverrideSummary();
            //recalculateLastOperatorInfo();
            break;

        case ControlStateUpdate:
            recalculateControlPendingSummary();
            break;

        case ConfigStateLastOperatorUpdate:
            recalculateLastOperatorInfo();
            break;

        case InhibitStateUpdate:
            recalculateScanInhibitSummary();
            recalculateAlarmInhibitSummary();
            recalculateMmsAlarmInhibitSummary();
            recalculateControlInhibitSummary();
            recalculateLastOperatorInfo();
            break;

        case ConfigStateUpdate:
            recalculateAlarmAckSummary();
            break;

        // DataNodes
        case ScanInhibitSummaryUpdate:
            recalculateScanInhibitSummary();
            recalculateLastOperatorInfo();
            break;

        case AlarmInhibitSummaryUpdate:
            recalculateAlarmInhibitSummary();
            recalculateLastOperatorInfo();
            break;

        case MmsAlarmInhibitSummaryUpdate:
            recalculateMmsAlarmInhibitSummary();
            recalculateLastOperatorInfo();
            break;

        case ControlInhibitSummaryUpdate:
            recalculateControlInhibitSummary();
            recalculateLastOperatorInfo();
            break;

        case ControlSummaryUpdate:
            recalculateControlPendingSummary();
            recalculateLastOperatorInfo();
            break;

        case AlarmSummaryUpdate:
            recalculateAlarmSummary();
            break;

        case AckSummaryUpdate:
            recalculateAlarmAckSummary();
            break;

        case OverrideSummaryUpdate:
            recalculateOverrideSummary();
            recalculateLastOperatorInfo();
            break;

        case QualitySummaryUpdate:
            recalculateQualitySummary();
            break;

        case TagStateUpdate:
        case NotesStateUpdate:
            break;

        case ConfigAvailable:
            // recalculate everything on config available
            // used to be done on creating the proxy
            recalculateScanInhibitSummary();
            recalculateAlarmInhibitSummary();
            recalculateMmsAlarmInhibitSummary();
            recalculateControlInhibitSummary();
            recalculateControlPendingSummary();
            recalculateAlarmSummary();
            recalculateAlarmAckSummary();
            recalculateOverrideSummary();
            recalculateQualitySummary();
            recalculateLastOperatorInfo();
            break;

        default:
            break;
        }

        FUNCTION_EXIT;
    }


    //
    // Protected methods
    //
    void DataNode::createProxies()
    {
        std::ostringstream fnName;
        fnName << "createProxies() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );
        
        TA_THREADGUARD(m_childproxieslock);

		m_childProxies.checkChilidProxiesCreated(m_staticConfig, m_scadaProxyFactory, *this);

        if ( areChildProxiesCreated() )
        {
            // TD18751
            // if there are observers, need to notify them with complete dynamic infomation when change to control
            // to do this is because the local proxies will not be refreshed when change to control.
            boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> dnupdate(new DnSyncUpdate);
            dnupdate->updateType = TA_Base_Bus::DnCompleteSyncState;
            
            boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> completeState = getCompleteDynamicStates();
            dnupdate->completeUpdate = *(completeState.get());
            
            notifyRegisteredObservers(dnupdate);
        }

        //if not all child proxies are created successfully, schedule a request to recreate
        if (false == areChildProxiesCreated())
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "createProxies() - %s: Not all child proxies created, will retry later.",
                m_staticConfig.name.c_str() );
            
            updateActivation();
        }

        FUNCTION_EXIT;
    }


    void DataNode::destroyProxies()
    {
        std::ostringstream fnName;
        fnName << "destroyProxies() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        TA_THREADGUARD(m_childproxieslock);
        
		m_childProxies.freeChildProxy();
 
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "destroyProxies() - %s: Proxies destroyed",
            m_staticConfig.name.c_str() );

        //
        // Clear out the summaries
        //
        clearSummaries();

        FUNCTION_EXIT;
    }


    void DataNode::recalculateScanInhibitSummary()
    {
        std::ostringstream fnName;
        fnName << "recalculateScanInhibitSummary() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        BooleanTag scanInhibit;
        scanInhibit.value = false;
        scanInhibit.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
        
        bool proxiesOk = areChildProxiesCreated();

        //
        // Update the summary by reading all DataPoints
        //
        
        ChildDataPointMapIt itDataPoint;
        for ( itDataPoint = m_childProxies.getDataPoints().begin();
              itDataPoint != m_childProxies.getDataPoints().end() && false == scanInhibit.value; 
              ++itDataPoint )
        {
            DataPointProxySmartPtr* dp = (*itDataPoint).second;

            try
            {
                if ( false == (*dp)->isWriteable() && true == (*dp)->isScanInhibited())
                {
                    scanInhibit.value = true;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateScanInhibitSummary() - %s - Could not retrieve state from DataPoint[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dp)->getEntityKey(), e.what());

                // set the flag to indicate data point is not available
                proxiesOk = false;
            }
        }

        //
        // Update the summary by reading all DataNodes
        //
        
        ChildDataNodeMapIt itDataNode;
        for (itDataNode = m_childProxies.getDataNodes().begin();
             itDataNode != m_childProxies.getDataNodes().end() && false == scanInhibit.value;
             ++itDataNode )
        {
            DataNodeProxySmartPtr* dn = (*itDataNode).second;
            try
            {

                BooleanTag dnScanInhibit = (*dn)->getScanInhibitSummary();
                if (true == dnScanInhibit.value)
                {
                    scanInhibit.value = true;
                }
            
                if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnScanInhibit.quality)
                {
                    // set the flag to indicate data node data is uncertain
                    proxiesOk = false;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e) 
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateScanInhibitSummary() - %s - Could not retrieve state from DataNode[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dn)->getEntityKey(), e.what());

                // set the flag to indicate data point is not available
                proxiesOk = false;
            }
        }
        
        // Update the quality
        if (false == proxiesOk && false == scanInhibit.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // inhibit state
            scanInhibit.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }
        
        // Check to see if anything has changed
        if ( ( scanInhibit.value != m_dynamicState.scanInhibitSummary.value ) ||
             ( scanInhibit.quality != m_dynamicState.scanInhibitSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.scanInhibitSummary.value = scanInhibit.value;
            m_dynamicState.scanInhibitSummary.quality = scanInhibit.quality;

            submitSummaryUpdateEvent(ScanInhibitSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateAlarmInhibitSummary()
    {
        std::ostringstream fnName;
        fnName << "recalculateAlarmInhibitSummary() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        BooleanTag alarmInhibit;
        
        alarmInhibit.value = false;
        alarmInhibit.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

        bool proxiesOk = areChildProxiesCreated();

        // Update the summary by reading all DataPoints
        ChildDataPointMapIt itDataPoint;
        for ( itDataPoint = m_childProxies.getDataPoints().begin();
              itDataPoint != m_childProxies.getDataPoints().end() && false == alarmInhibit.value;
              ++itDataPoint)
        {
            DataPointProxySmartPtr* dp = (*itDataPoint).second;

            try
            {
                if ( false == (*dp)->isWriteable() &&
                     true == (*dp)->isAlarmInhibited() )
                {
                    alarmInhibit.value = true;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateAlarmInhibitSummary() - %s - Could not retrieve state from DataPoint[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dp)->getEntityKey(), e.what());

                proxiesOk = false;
            }
        }

        //
        // Update the summary by reading all DataNodes
        //

        ChildDataNodeMapIt itDataNode;
        for (itDataNode = m_childProxies.getDataNodes().begin();
             itDataNode != m_childProxies.getDataNodes().end() && false == alarmInhibit.value;
             ++itDataNode)
        {
			DataNodeProxySmartPtr* dn = (*itDataNode).second;
            try
            {
                 BooleanTag dnAlarmInhibit = (*dn)->getAlarmInhibitSummary();
                 if (dnAlarmInhibit.value)
                {
                    alarmInhibit.value = true;
                }
            
                if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnAlarmInhibit.quality)
                {
                    // set the flag to indicate data node data is uncertain
                    proxiesOk = false;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateAlarmInhibitSummary() - %s - Could not retrieve state from DataNode[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dn)->getEntityKey(), e.what());

                proxiesOk = false;
            }

        }
                
        // Update the quality
        if (false == proxiesOk && false == alarmInhibit.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // inhibit state
            alarmInhibit.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }

        // Check to see if anything has changed
        if ( ( alarmInhibit.value != m_dynamicState.alarmInhibitSummary.value ) ||
             ( alarmInhibit.quality != m_dynamicState.alarmInhibitSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.alarmInhibitSummary.value = alarmInhibit.value;
            m_dynamicState.alarmInhibitSummary.quality = alarmInhibit.quality;

            submitSummaryUpdateEvent(AlarmInhibitSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateMmsAlarmInhibitSummary()
    {
        std::ostringstream fnName;
        fnName << "recalculateMmsAlarmInhibitSummary() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        BooleanTag mmsAlarmInhibit;
        
        mmsAlarmInhibit.value = false;
        mmsAlarmInhibit.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

        bool proxiesOk = areChildProxiesCreated();
        
        // Update the summary by reading all DataPoints
        ChildDataPointMapIt itDataPoint;
        for ( itDataPoint = m_childProxies.getDataPoints().begin();
              itDataPoint != m_childProxies.getDataPoints().end() && false == mmsAlarmInhibit.value;
              ++itDataPoint)
        {
            DataPointProxySmartPtr* dp = (*itDataPoint).second;

            try
            {
                if ( false == (*dp)->isWriteable() && true == (*dp)->isMmsAlarmInhibited())
                {
                    mmsAlarmInhibit.value = true;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateMmsAlarmInhibitSummary() - %s - Could not retrieve state from DataPoint[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dp)->getEntityKey(), e.what());
                
                proxiesOk = false;
            }
        }

        //
        // Update the summary by reading all DataNodes
        //

        ChildDataNodeMapIt itDataNode;
        for (itDataNode = m_childProxies.getDataNodes().begin();
             itDataNode != m_childProxies.getDataNodes().end() && false == mmsAlarmInhibit.value;
             ++itDataNode)
        {
			DataNodeProxySmartPtr* dn = (*itDataNode).second;
            try
            {
                BooleanTag dnMmsAlarmInhibit = (*dn)->getMmsAlarmInhibitSummary();
                if (dnMmsAlarmInhibit.value)
                {
                    mmsAlarmInhibit.value = true;
                }
                    
                if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnMmsAlarmInhibit.quality)
                {
                    // set the flag to indicate data node data is uncertain
                    proxiesOk = false;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateMmsAlarmInhibitSummary() - %s - Could not retrieve state from DataNode[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dn)->getEntityKey(), e.what());

                proxiesOk = false;
            }
        }
        
        // Update the quality
        if (false == proxiesOk && false == mmsAlarmInhibit.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // inhibit state
            mmsAlarmInhibit.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }
        
        // Check to see if anything has changed
        if ( ( mmsAlarmInhibit.value != m_dynamicState.mmsAlarmInhibitSummary.value ) ||
             ( mmsAlarmInhibit.quality != m_dynamicState.mmsAlarmInhibitSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.mmsAlarmInhibitSummary.value = mmsAlarmInhibit.value;
            m_dynamicState.mmsAlarmInhibitSummary.quality = mmsAlarmInhibit.quality;

            submitSummaryUpdateEvent(MmsAlarmInhibitSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateControlInhibitSummary()
    {
        std::ostringstream fnName;
        fnName << "recalculateControlInhibitSummary() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        BooleanTag controlInhibit;
        
        controlInhibit.value = false;
        controlInhibit.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

        bool proxiesOk = areChildProxiesCreated();

        // Update the summary by reading all DataPoints         
        ChildDataPointMapIt itDataPoint;
        for ( itDataPoint = m_childProxies.getDataPoints().begin();
              itDataPoint != m_childProxies.getDataPoints().end() && false == controlInhibit.value;
              ++itDataPoint )
        {
            DataPointProxySmartPtr* dp = itDataPoint->second;

            try
            {
                // only proceed if the data point is an output as you cannot
                // control inhibit an input data point
                if ( true == (*dp)->isWriteable() &&
                     true == (*dp)->isControlInhibited() )
                {
                    controlInhibit.value = true;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateControlInhibitSummary() - %s - Could not retrieve state from DataPoint[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dp)->getEntityKey(), e.what());

                proxiesOk = false;
            }
        }

        //
        // Update the summary by reading all DataNodes
        //

        ChildDataNodeMapIt itDataNode;
        for (itDataNode = m_childProxies.getDataNodes().begin();
             itDataNode != m_childProxies.getDataNodes().end() && false == controlInhibit.value;
             ++itDataNode)
        {
			DataNodeProxySmartPtr* dn = (*itDataNode).second;
            try
            {
                BooleanTag dnControlInhibit = (*dn)->getControlInhibitSummary();
                if (dnControlInhibit.value)
                {
                    controlInhibit.value = true;
                }
                    
                if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnControlInhibit.quality)
                {
                    // set the flag to indicate data node data is uncertain
                    proxiesOk = false;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateControlInhibitSummary() - %s - Could not retrieve state from DataNode[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dn)->getEntityKey(), e.what());
                
                proxiesOk = false;
            }
        }
        
        // Update the quality
        if (false == proxiesOk && false == controlInhibit.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // inhibit state
            controlInhibit.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }
        
        // Check to see if anything has changed
        if ( ( controlInhibit.value != m_dynamicState.controlInhibitSummary.value ) ||
             ( controlInhibit.quality != m_dynamicState.controlInhibitSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.controlInhibitSummary.value = controlInhibit.value;
            m_dynamicState.controlInhibitSummary.quality = controlInhibit.quality;

            submitSummaryUpdateEvent(ControlInhibitSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateAlarmSummary()
    {
        std::ostringstream fnName;
        fnName << "recalculateAlarmSummary() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        BooleanTag alarmSummary;

        // default value
        alarmSummary.value = false;
        alarmSummary.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

        bool proxiesOk = areChildProxiesCreated();

        //
        // Update the alarm summary by reading all DataPoints
        //

        ChildDataPointMapIt itDataPoint;
        for (itDataPoint = m_childProxies.getDataPoints().begin();
             itDataPoint != m_childProxies.getDataPoints().end() && false == alarmSummary.value && true == proxiesOk;
             ++itDataPoint)
        {
            DataPointProxySmartPtr* dp = (*itDataPoint).second;

            try
            {
                // only proceed if the data point is an input
                if ( false == (*dp)->isWriteable() && true == (*dp)->isInAlarmState() )
                {
                    // update the Alarm Summary
                    alarmSummary.value = true;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateAlarmSummary() - %s - Could not retrieve state from DataPoint[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dp)->getEntityKey(), e.what());
                
                // set the flag to indicate data point is not available
                proxiesOk = false;
            }
        }

        //
        // Update the summary by reading all DataNodes
        //

        ChildDataNodeMapIt itDataNode;
        for (itDataNode = m_childProxies.getDataNodes().begin();
             itDataNode != m_childProxies.getDataNodes().end() && false == alarmSummary.value && true == proxiesOk;
             ++itDataNode)
        {
			DataNodeProxySmartPtr* dn = (*itDataNode).second;
            try
            {
                BooleanTag dnAlarmSummary = (*dn)->getAlarmSummary();

                // if the data node alarm summary is on
                if (true == dnAlarmSummary.value)
                {
                    // update the alarm summary of this node
                    alarmSummary.value = true;
                }
            
                if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnAlarmSummary.quality)
                {
                    // set the flag to indicate data node data is uncertain
                    proxiesOk = false;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateAlarmSummary() - %s - Could not retrieve state from DataNode[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dn)->getEntityKey(), e.what());
                
                // set the flag to indicate data point is not available
                proxiesOk = false;
            }
            
        }
        
        // Update the quality
        if (false == proxiesOk)
        {   
            // There is something wrong with the proxies and we haven't detected
            // alarm state
            alarmSummary.value = true;
            alarmSummary.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }

        if ( ( alarmSummary.value != m_dynamicState.alarmSummary.value ) ||
             ( alarmSummary.quality != m_dynamicState.alarmSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.alarmSummary.value  = alarmSummary.value;
            m_dynamicState.alarmSummary.quality = alarmSummary.quality;

            submitSummaryUpdateEvent(AlarmSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateAlarmAckSummary()
    {
        std::ostringstream fnName;
        fnName << "recalculateAlarmAckSummary() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        bool summaryCertain = areChildProxiesCreated();

        EnumTag alarmAck;
        alarmAck.value = ScadaAlarmNotPresent;
        alarmAck.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
        
        // Update the summary by reading all DataPoints
        ChildDataPointMapIt itDataPoint;
        for (itDataPoint = m_childProxies.getDataPoints().begin();
             itDataPoint != m_childProxies.getDataPoints().end() && alarmAck.value != ScadaAlarmNack && true == summaryCertain;
             ++itDataPoint)
        {
            DataPointProxySmartPtr* dp = (*itDataPoint).second;

            try
            {
                if (false == (*dp)->isWriteable())
                {
                    // Update the Alarm Ack Summary for input points
                    switch ((*dp)->getAlarmAckState())
                    {
                    case ScadaAlarmNack:
                        alarmAck.value = ScadaAlarmNack;
                        break;

                    case ScadaAlarmAck:
                        if (alarmAck.value != ScadaAlarmNack)
                        {
                            alarmAck.value = ScadaAlarmAck;
                        }
                        break;

                    case ScadaAlarmNotPresent:
                        // No change
                        break;

                    case ScadaAlarmUnknown:
                    default:
                        summaryCertain = false;

                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                            "recalculateAlarmAckSummary() - %s - Alarm ack state for DataPoint %d is not yet available",
                            m_staticConfig.name.c_str(), (*dp)->getEntityKey());
                        break;
                    }
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateAlarmAckSummary() - %s - Could not retrieve state from DataPoint[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dp)->getEntityKey(), e.what());
                
                summaryCertain = false;
            }
        }
    
        // Update the summary by reading all DataNodes
        ChildDataNodeMapIt itDataNode;
        for (itDataNode = m_childProxies.getDataNodes().begin();
             itDataNode != m_childProxies.getDataNodes().end() && alarmAck.value != ScadaAlarmNack && true == summaryCertain;
             ++itDataNode)
        {
			DataNodeProxySmartPtr* dn = (*itDataNode).second;
            try
            {
                EnumTag dnAckSummary = (*dn)->getAlarmAckStateSummary();
                
                if (dnAckSummary.value == ScadaAlarmNack)
                {
                    alarmAck.value = ScadaAlarmNack;
                }
                else if (dnAckSummary.value == ScadaAlarmAck && alarmAck.value != ScadaAlarmNack)
                {
                    alarmAck.value =  ScadaAlarmAck;
                }
        
                if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnAckSummary.quality)
                {
                    // set the flag to indicate data node data is uncertain
                    summaryCertain = false;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateAlarmAckSummary() - %s - Could not retrieve state from DataNode[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dn)->getEntityKey(), e.what());
                
                summaryCertain = false;
            }
        }

        if (false == summaryCertain)
        {
            // We have some uncertainty with the summary
            // We can only be sure of an NACK
            alarmAck.value = ScadaAlarmNack;
            alarmAck.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }

        if ( alarmAck.value != m_dynamicState.ackSummary.value ||
             alarmAck.quality != m_dynamicState.ackSummary.quality )
        {
            m_dynamicState.ackSummary.value = alarmAck.value;
            m_dynamicState.ackSummary.quality = alarmAck.quality;

            submitSummaryUpdateEvent(AckSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateOverrideSummary()
    {
        std::ostringstream fnName;
        fnName << "recalculateOverrideSummary() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        BooleanTag override;

        // default values
        override.value = false;
        override.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

        bool proxiesOk = areChildProxiesCreated();

        //
        // Update the summary by reading all DataPoints
        //
        
        ChildDataPointMapIt itDataPoint;
        for ( itDataPoint = m_childProxies.getDataPoints().begin();
              itDataPoint != m_childProxies.getDataPoints().end() && false == override.value;
              ++itDataPoint )
        {
            DataPointProxySmartPtr* dp = itDataPoint->second;

            try
            {
                // only proceed if the data point is an input
                if ( false == (*dp)->isWriteable()  && true == (*dp)->isOverrideEnabled() )
                {
                    // update the Override summary of this node
                    override.value = true;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateOverrideSummary() - %s - Could not retrieve state from DataPoint[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dp)->getEntityKey(), e.what());

                // set the flag to indicate the data point is not available
                proxiesOk = false;
            }
        }

        //
        // Update the override summary by reading all DataNodes
        //

        ChildDataNodeMapIt itDataNode;
        for (itDataNode = m_childProxies.getDataNodes().begin();
             itDataNode != m_childProxies.getDataNodes().end() && false == override.value;
             ++itDataNode)
        {
			DataNodeProxySmartPtr* dn = (*itDataNode).second;
            try
            {
                BooleanTag dnOverrideSummary = (*dn)->getOverrideSummary();

                // if the data node override summary is on
                if (dnOverrideSummary.value)
                {
                    // update override summary of this node
                    override.value = true;
                }
                    
                if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnOverrideSummary.quality)
                {
                    // set the flag to indicate data node data is uncertain
                    proxiesOk = false;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateOverrideSummary() - %s - Could not retrieve state from DataNode[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dn)->getEntityKey(), e.what());

                // set the flag to indicate the data point is not available
                proxiesOk = false;
            }
            
        }
        
        // Update the quality
        if (false == proxiesOk && false == override.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // override state
            override.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }

        if ( ( override.value != m_dynamicState.overrideSummary.value ) ||
             ( override.quality != m_dynamicState.overrideSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.overrideSummary.value = override.value;
            m_dynamicState.overrideSummary.quality = override.quality;

            submitSummaryUpdateEvent(OverrideSummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateQualitySummary()
    {
        std::ostringstream fnName;
        fnName << "recalculateQualitySummary() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        unsigned long qualitySummary = QUALITY_GOOD;

        if (false == areChildProxiesCreated())
        {
            // Not all proxies created so we just set the quality to uncertain
            qualitySummary = QUALITY_UNCERTAIN;
        }
        else
        {
            //
            // Update the quality summary by reading all DataPoints
            //

            ChildDataPointMapIt itDataPoint;
            for (itDataPoint = m_childProxies.getDataPoints().begin();
                 itDataPoint != m_childProxies.getDataPoints().end() && qualitySummary != QUALITY_BAD;
                 ++itDataPoint)
            {
                DataPointProxySmartPtr* dp = (*itDataPoint).second;

                try
                {
                    // only proceed if data point is an input
                    if ( false == (*dp)->isWriteable() )
                    {
                        // if data point quality status is not good
                        // (don't care what type of bad quality it is at this stage)
                        if ( QUALITY_GOOD != ((*dp)->getStatus() &  QUALITY_MASK) )
                        {
                            // force quality summary value to bad
                            qualitySummary = QUALITY_BAD;
                        }
                    }
                }
                catch (TA_Base_Core::ScadaProxyException &e)
                {
                    // already logged by the exception throw. This is too noisy
                    // make it debug rather than an exception catch
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                        "Caught ScadaProxyException. recalculateQualitySummary() - %s - Could not retrieve state from DataPoint[%d]; Reason: %s",
                        m_staticConfig.name.c_str(), (*dp)->getEntityKey(), e.what());

                    // data point is not available, force quality summary value to bad
                    qualitySummary = QUALITY_BAD;
                }
            }

            //
            // Update the quality summary by reading all DataNodes
            //

            ChildDataNodeMapIt itDataNode;
            for (itDataNode = m_childProxies.getDataNodes().begin();
                 itDataNode != m_childProxies.getDataNodes().end() &&  qualitySummary != QUALITY_BAD;
                 ++itDataNode)
            {
				DataNodeProxySmartPtr* dn = (*itDataNode).second;	 
                try
                {
                    EnumTag dnQualitySummary = (*dn)->getQualitySummary();

                    // if the data node quality summary value is not good
                    // (don't care what type of bad quality it is at this stage)
                    if ( QUALITY_GOOD != dnQualitySummary.value ||
                         QUALITY_GOOD_NO_SPECIFIC_REASON != dnQualitySummary.quality)
                    {
                        // force quality summary value to bad
                        qualitySummary = QUALITY_BAD;
                    }
                }
                catch (TA_Base_Core::ScadaProxyException &e)
                {
                    // already logged by the exception throw. This is too noisy
                    // make it debug rather than an exception catch
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                        "Caught ScadaProxyException. recalculateQualitySummary() - %s - Could not retrieve state from DataNode[%d]; Reason: %s",
                        m_staticConfig.name.c_str(), (*dn)->getEntityKey(), e.what());

                    // data point is not available, force quality summary value to bad
                    qualitySummary = QUALITY_BAD;
                }
            }
        }

        if ( qualitySummary != m_dynamicState.qualitySummary.value )
        {
            // Update the cached summaries
            m_dynamicState.qualitySummary.value = qualitySummary;
            m_dynamicState.qualitySummary.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

            submitSummaryUpdateEvent(QualitySummaryUpdate);
        }

        FUNCTION_EXIT;
    }


    void DataNode::recalculateControlPendingSummary()
    {
        std::ostringstream fnName;
        fnName << "recalculateControlPendingSummary() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        BooleanTag controlPending;

        // default values
        controlPending.value = false;
        controlPending.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;

        bool proxiesOk = areChildProxiesCreated();

        //
        // Update the control pending summary by reading all DataPoints
        //
        ChildDataPointMapIt itDataPoint;
        for (itDataPoint = m_childProxies.getDataPoints().begin();
             itDataPoint != m_childProxies.getDataPoints().end() && false == controlPending.value;
             ++itDataPoint)
        {
            DataPointProxySmartPtr* dp = itDataPoint->second;

            try
            {
                // only proceed if the data point is an output
                if ( true == (*dp)->isWriteable() && pending == (*dp)->getControlState() )
                {
                    // update the ControlPending Summary
                    controlPending.value = true;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateControlPendingSummary() - %s - Could not retrieve state from DataPoint[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dp)->getEntityKey(), e.what());

                // set the flag to indicate data point is not available
                proxiesOk = false;
            }
        }

        //
        // Update the summary by reading all DataNodes
        //

        ChildDataNodeMapIt itDataNode;
        for (itDataNode = m_childProxies.getDataNodes().begin();
             itDataNode != m_childProxies.getDataNodes().end() && false == controlPending.value;
             ++itDataNode)
        {
			DataNodeProxySmartPtr* dn = (*itDataNode).second;
            try
            {
                BooleanTag dnControlPending = (*dn)->getControlPendingSummary();

                // if the data node alarm summary is on
                if ( true == dnControlPending.value )
                {
                    // update the control pending summary of this node
                    controlPending.value = true;
                }
                                
                if (QUALITY_GOOD_NO_SPECIFIC_REASON != dnControlPending.quality)
                {
                    // set the flag to indicate data node data is uncertain
                    proxiesOk = false;
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateControlPendingSummary() - %s - Could not retrieve state from DataNode[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dn)->getEntityKey(), e.what());

                // set the flag to indicate data point is not available
                proxiesOk = false;
            }
        }
        
        // Update the quality
        if (false == proxiesOk && false == controlPending.value)
        {   
            // There is something wrong with the proxies and we haven't detected
            // control pending state
            controlPending.quality = QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;
        }

        if ( ( controlPending.value != m_dynamicState.controlPendingSummary.value ) ||
             ( controlPending.quality != m_dynamicState.controlPendingSummary.quality ) )
        {
            // Update the cached summaries
            m_dynamicState.controlPendingSummary.value = controlPending.value;
            m_dynamicState.controlPendingSummary.quality = controlPending.quality;

            submitSummaryUpdateEvent(ControlSummaryUpdate);
        }

        FUNCTION_EXIT;
    }

    void DataNode::recalculateLastOperatorInfo()
    {
        std::ostringstream fnName;
        fnName << "recalculateLastOperatorInfo() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        time_t b4updatetimestamp = m_dynamicState.lastOperator.timeStamp.time;
        std::string b4updatesessionid = m_dynamicState.lastOperator.sessionID;


        bool proxiesOk = areChildProxiesCreated();

        //
        // Update the summary by reading all DataPoints
        //
        
        ChildDataPointMapIt itDataPoint;
        for ( itDataPoint = m_childProxies.getDataPoints().begin();
              itDataPoint != m_childProxies.getDataPoints().end() ;
              ++itDataPoint )
        {
            DataPointProxySmartPtr* dp = itDataPoint->second;

            try
            {
                long latesttimestamp = (*dp)->getLastStatusChangeTimestamp();
                if (latesttimestamp >= m_dynamicState.lastOperator.timeStamp.time)
                {
                    m_dynamicState.lastOperator.timeStamp.time = latesttimestamp;
                    m_dynamicState.lastOperator.sessionID = (*dp)->getLastStatusChangeSessionId();
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateLastOperatorInfo() - %s - Could not retrieve state from DataPoint[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dp)->getEntityKey(), e.what());

                // set the flag to indicate the data point is not available
                proxiesOk = false;
            }
        }

        //
        // Update the last operator summary by reading all DataNodes
        //

        ChildDataNodeMapIt itDataNode;
        for (itDataNode = m_childProxies.getDataNodes().begin();
             itDataNode != m_childProxies.getDataNodes().end() ;
             ++itDataNode)
        {
			DataNodeProxySmartPtr* dn = (*itDataNode).second;
            try
            {
                long latesttimestamp = (*dn)->getLastStatusChangeTimestamp();
                if (latesttimestamp >= m_dynamicState.lastOperator.timeStamp.time)
                {
                    m_dynamicState.lastOperator.timeStamp.time = latesttimestamp;
                    m_dynamicState.lastOperator.sessionID = (*dn)->getLastStatusChangeSessionId();
                }
            }
            catch (TA_Base_Core::ScadaProxyException &e)
            {
                // already logged by the exception throw. This is too noisy
                // make it debug rather than an exception catch
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Caught ScadaProxyException. recalculateLastOperatorInfo() - %s - Could not retrieve state from DataNode[%d]; Reason: %s",
                    m_staticConfig.name.c_str(), (*dn)->getEntityKey(), e.what());

                // set the flag to indicate the data point is not available
                proxiesOk = false;
            }
        }

        if ( ( b4updatetimestamp != m_dynamicState.lastOperator.timeStamp.time ) ||
             ( b4updatesessionid.compare(m_dynamicState.lastOperator.sessionID) != 0 ) )
        {
            submitSummaryUpdateEvent(ConfigStateLastOperatorUpdate);
        }

        
        FUNCTION_EXIT;
    }

    void DataNode::readConfiguration()
    {
        std::ostringstream fnName;
        fnName << "readConfiguration() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        // Retrieve configuration
        m_staticConfig.name = m_dataNodeEntityAccess->getName();
        m_staticConfig.description = m_dataNodeEntityAccess->getDescription();
        m_staticConfig.entityKey = m_dataNodeEntityAccess->getKey();
        m_staticConfig.subsystemKey = m_dataNodeEntityAccess->getSubsystem();
        m_staticConfig.locationKey = m_dataNodeEntityAccess->getLocation();
        m_staticConfig.agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
        m_staticConfig.assetName = m_dataNodeEntityAccess->getAssetName();
        m_staticConfig.label = m_dataNodeEntityAccess->getLabel();
        m_staticConfig.shortLabel = m_dataNodeEntityAccess->getShortLabel();
        m_staticConfig.displayOrder = m_dataNodeEntityAccess->getDisplayOrder();

        //
        // retrieve the corba names of the children, based on the configured
        // synthesis type
        //
        m_staticConfig.childDataNodeEntities.clear();
        m_staticConfig.childDataPointEntities.clear();

		m_staticConfig.childDataNodeEntities = m_dataNodeEntityAccess->getChildDataNodes();
		m_staticConfig.childDataPointEntities = m_dataNodeEntityAccess->getChildDataPoints();

        FUNCTION_EXIT;
    }

    void DataNode::updateConfiguration()
    {
        std::ostringstream fnName;
        fnName << "updateConfiguration() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        // invalidate the DataNodeEntityData access so that latest values are
        // retrieved from the database
        m_dataNodeEntityAccess->invalidate();

        // Retrieve online updatable configuration
        m_staticConfig.assetName = m_dataNodeEntityAccess->getAssetName();
        m_staticConfig.label = m_dataNodeEntityAccess->getLabel();
        m_staticConfig.shortLabel = m_dataNodeEntityAccess->getShortLabel();
        m_staticConfig.displayOrder = m_dataNodeEntityAccess->getDisplayOrder();

        FUNCTION_EXIT;
    }

    void DataNode::readState()
    {
        std::ostringstream fnName;
        fnName << "readState() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        //
        // Retrieve current state for items stored in the database while we're at it
        //
        TA_ASSERT( m_persistence.get() != NULL, "ScadaPersistence has not been intialised" );
        m_persistence->readDataNodeState( m_staticConfig.entityKey, m_dynamicState );

        FUNCTION_EXIT;
    }

    void DataNode::processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType)
    {
        std::ostringstream fnName;
        fnName << "processEntityUpdateEvent() - " << m_staticConfig.name;
        fnName << " - type: " << ((int)updateType);
        FUNCTION_ENTRY (fnName.str().c_str());

        // Don't insert if this is in the queue already!
		boost::shared_ptr<SummaryRequest> request (new SummaryRequest(m_staticConfig.entityKey, updateType));
        m_summaryQ->queueItemUnique(request);

        FUNCTION_EXIT;
    }


    void DataNode::submitUpdateEvent( ScadaEntityUpdateType type )
    {
        std::ostringstream fnName;
        fnName << "submitUpdateEvent() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        // only proceed if operates in Control Mode
        if (m_operationMode != TA_Base_Core::Control)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "%s - in monitor mode", m_staticConfig.name.c_str() );
            
            return;
        }
        
        boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> toupdate(new TA_Base_Bus::DnSyncUpdate);
        std::stringstream ss;

        switch (type)
        {
        case TagStateUpdate:
            {
                toupdate->updateType = DnTagStateUpdate;
                toupdate->tagState.comment = m_dynamicState.tagState.comment;
                toupdate->tagState.ptwId = m_dynamicState.tagState.ptwId;
                toupdate->tagState.tagInhibitState = m_dynamicState.tagState.tagInhibitState;
                toupdate->tagState.tagState = m_dynamicState.tagState.tagState;
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "submitUpdateEvent() - %s - Tag state updated",
                     m_staticConfig.name.c_str() );
            }
            break;
        
        case NotesStateUpdate:
            {
                toupdate->updateType = DnNotesUpdate;
                toupdate->notes = m_dynamicState.notes;
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "submitUpdateEvent() - %s - Notes updated",
                     m_staticConfig.name.c_str() );
            }
            break;

        case InhibitStateUpdate:
            {

                toupdate->updateType = DnInhibitMMSUpdate;
                toupdate->inhibitMMSState = m_dynamicState.inhibitMMSState;
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "submitUpdateEvent() - %s - MMS inhibit state updated",
                     m_staticConfig.name.c_str() );
            }
            break;

        default:
            TA_ASSERT(true, "Bad Update Type");
            break;
        }

        // notify all registered observers of the latest changes
        notifyRegisteredObservers ( toupdate );

        if (type != NotesStateUpdate)
        {
            //submit the last operator info for mms inhibit and tag update
            submitSummaryUpdateEvent(ConfigStateLastOperatorUpdate);
        }

        FUNCTION_EXIT;
    }

    void DataNode::submitSummaryUpdateEvent(ScadaEntityUpdateType updateType)
    {
        std::ostringstream fnName;
        fnName << "submitSummaryUpdateEvent() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        // only proceed if operates in Control Mode
        if (m_operationMode != TA_Base_Core::Control)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "%s - in monitor mode", m_staticConfig.name.c_str() );
            
            return;
        }

        boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> toupdate(new TA_Base_Bus::DnSyncUpdate);
        std::stringstream ss;
        ss << "submitSummaryUpdateEvent() - " << m_staticConfig.name;

        
        switch (updateType)
        {
        case AlarmSummaryUpdate:
            {
                toupdate->updateType = DnAlarmSummaryUpdate;
                toupdate->alarmSummary.value = m_dynamicState.alarmSummary.value;
                toupdate->alarmSummary.quality = m_dynamicState.alarmSummary.quality;
                ss << " - Alarm Summary Updated: m_dynamicState.alarmSummary.value = " << m_dynamicState.alarmSummary.value << ", m_dynamicState.alarmSummary.quality = " << m_dynamicState.alarmSummary.quality;
            }
            break;

        case AckSummaryUpdate:
            {
                toupdate->updateType = DnAlarmAckSummaryUpdate;
                toupdate->ackSummary.value = m_dynamicState.ackSummary.value;
                toupdate->ackSummary.quality = m_dynamicState.ackSummary.quality;
                ss << " - Alarm Ack Summary Updated: m_dynamicState.ackSummary.value = " << m_dynamicState.ackSummary.value << ", m_dynamicState.ackSummary.quality = " << m_dynamicState.ackSummary.quality;
            }
            break;
            
        case ScanInhibitSummaryUpdate:
            {
                toupdate->updateType = DnScanInhibitSummaryUpdate;
                toupdate->scanInhibitSummary.value = m_dynamicState.scanInhibitSummary.value;
                toupdate->scanInhibitSummary.quality = m_dynamicState.scanInhibitSummary.quality;
                ss << " - Scan Inhibit Summary Updated: m_dynamicState.scanInhibitSummary.value = " << m_dynamicState.scanInhibitSummary.value << ", m_dynamicState.scanInhibitSummary.quality = " << m_dynamicState.scanInhibitSummary.quality;
            }
            break;

        case AlarmInhibitSummaryUpdate:
            {
                toupdate->updateType = DnAlarmInhibitSummaryUpdate;
                toupdate->alarmInhibitSummary.value = m_dynamicState.alarmInhibitSummary.value;
                toupdate->alarmInhibitSummary.quality = m_dynamicState.alarmInhibitSummary.quality;
                ss << " - Alarm Inhibit Summary Updated: m_dynamicState.alarmInhibitSummary.value = " << m_dynamicState.alarmInhibitSummary.value << ", m_dynamicState.alarmInhibitSummary.quality = " << m_dynamicState.alarmInhibitSummary.quality;
            }
            break;

        case MmsAlarmInhibitSummaryUpdate:
            {
                toupdate->updateType = DnMmsAlarmInhibitSummaryUpdate;
                toupdate->mmsAlarmInhibitSummary.value = m_dynamicState.mmsAlarmInhibitSummary.value;
                toupdate->mmsAlarmInhibitSummary.quality = m_dynamicState.mmsAlarmInhibitSummary.quality;
                ss << " - MMS Inhibit Summary Updated: m_dynamicState.mmsAlarmInhibitSummary.value = " << m_dynamicState.mmsAlarmInhibitSummary.value << ", m_dynamicState.mmsAlarmInhibitSummary.quality = " << m_dynamicState.mmsAlarmInhibitSummary.quality;
            }
            break;

        case ControlInhibitSummaryUpdate:            
            {
                toupdate->updateType = DnControlInhibitSummaryUpdate;
                toupdate->controlInhibitSummary.value = m_dynamicState.controlInhibitSummary.value;
                toupdate->controlInhibitSummary.quality = m_dynamicState.controlInhibitSummary.quality;
                ss << " - Control Inhibit Summary Updated: m_dynamicState.controlInhibitSummary.value = " << m_dynamicState.controlInhibitSummary.value << ", m_dynamicState.controlInhibitSummary.quality = " << m_dynamicState.controlInhibitSummary.quality;
            }
            break;

        case OverrideSummaryUpdate:
            {
                toupdate->updateType = DnOverrideSummaryUpdate;
                toupdate->overrideSummary.value = m_dynamicState.overrideSummary.value;
                toupdate->overrideSummary.quality = m_dynamicState.overrideSummary.quality;
                ss << " - Override Inhibit Summary Updated: m_dynamicState.overrideSummary.value = " << m_dynamicState.overrideSummary.value << ", m_dynamicState.overrideSummary.quality = " << m_dynamicState.overrideSummary.quality;
            }
            break;
        
        case QualitySummaryUpdate:
            {
                toupdate->updateType = DnQualitySummaryUpdate;
                toupdate->qualitySummary.value = m_dynamicState.qualitySummary.value;
                toupdate->qualitySummary.quality = m_dynamicState.qualitySummary.quality;
                ss << " - Quality Inhibit Summary Updated: m_dynamicState.qualitySummary.value = " << m_dynamicState.qualitySummary.value << ", m_dynamicState.qualitySummary.quality = " << m_dynamicState.qualitySummary.quality;
            }
            break;
            
        case ControlSummaryUpdate:
            {
                toupdate->updateType = DnControlPendingSummaryUpdate;
                toupdate->controlPendingSummary.value = m_dynamicState.controlPendingSummary.value;
                toupdate->controlPendingSummary.quality = m_dynamicState.controlPendingSummary.quality;
                ss << " - Control Pending Inhibit Summary Updated: m_dynamicState.controlPendingSummary.value = " << m_dynamicState.controlPendingSummary.value << ", m_dynamicState.controlPendingSummary.quality = " << m_dynamicState.controlPendingSummary.quality;

            }
            break;

        case ConfigStateLastOperatorUpdate:
            {
                m_persistence->writePersistedValue( DN_LAST_OPERATOR_SESSION_ID, m_staticConfig.entityKey, m_dynamicState.lastOperator.sessionID );
                m_persistence->writePersistedValue( DN_LAST_OPERATOR_TIMESTAMP, m_staticConfig.entityKey, m_dynamicState.lastOperator.timeStamp.time ); 
                toupdate->updateType = DnLastOperatorUpdate;
                toupdate->lastOperator.sessionID = m_dynamicState.lastOperator.sessionID;
                toupdate->lastOperator.timeStamp.time = m_dynamicState.lastOperator.timeStamp.time;
                ss << " - Last Operator Summary Updated: m_dynamicState.lastOperator.sessionID = " << m_dynamicState.lastOperator.sessionID.c_str() << ", m_dynamicState.lastOperator.timeStamp.time = " << m_dynamicState.lastOperator.timeStamp.time;
            }
            break;		

        default:
            TA_ASSERT(true, "Bad Summary Update Type");
            break;
        }

        //
        // notify all registered observers (BulkPolledInterface ) of the latest changes
        //
        notifyRegisteredObservers ( toupdate );

        FUNCTION_EXIT;
    }

    void DataNode::submitAuditEvent( const std::string & sessionID,
                                     const TA_Base_Core::DescriptionParameters & description,
                                     const TA_Base_Core::MessageType & messageType )
    {
        // get handle to AuditMessageSender for the audit message type DataNodeAudit,
        // if not already done so
        if ( 0 == m_auditSender )
        {
            m_auditSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender ( TA_Base_Core::DataNodeAudit::Context );
        }

        // if there is handle to AuditMessageSender
        if ( 0 != m_auditSender )
        {
            try
            {
                // send the new audit message type
                m_auditSender->sendAuditMessageWithAsset ( messageType,                     // message type
                                                          m_staticConfig.entityKey,         // entity key
                                                          description,                      // description
                                                          "",                               // details
                                                          sessionID,                        // session ID
                                                          "",                               // alarm ID
                                                          "",                               // incidentkey
                                                          "",                               // parent event ID
                                                          m_staticConfig.assetName );   // asset name
            }
            catch ( ... )
            {
            }
        }
        // else do nothing
    }

    bool DataNode::isActionPermitted ( const std::string & sessionID, unsigned long actionKey )
    {
        bool response = false;
        
        // action is only permitted when data point is running in control mode
        if ( TA_Base_Core::Control == m_operationMode )
        {
            if ( ( DPT_INTERNAL_SESSION_ID == sessionID ) || ( true == sessionID.empty() ) )
            {
                response = true;
            }
            else
            {
                // get the response from the ScadaUtilities
                response = TA_Base_Bus::ScadaUtilities::getInstance().isActionAllowedWithLocSub (
                    actionKey, m_staticConfig.entityKey, m_staticConfig.locationKey, m_staticConfig.subsystemKey, sessionID );
                
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "isActionAllowedWithLocSub() returned: %s", ( true == response ) ? "TRUE" : "FALSE" );
			}
		}
		return response;
    }

    unsigned short DataNode::calculateFlexibleTimeout()
    {
        std::ostringstream fnName;
        fnName << "calculateFlexibleTimeout() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        unsigned short timeout = 20*3*m_staticConfig.childDataPointEntities.size();

        FUNCTION_EXIT;
        return timeout; 
    }

    void DataNode::inhibitDataPoints(TA_Base_Bus::EOutputInhibitState tagOutputInhibitState, 
                                     TA_Base_Bus::EInputInhibitState tagInputInhibitState,
                                     const std::string& sessionIdStr)
    {
        std::ostringstream fnName;
        fnName << "inhibitDataPoints() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

		if(m_childProxies.isChildDataPointProxyCreated())
		{
		
			TA_Base_Core::ThreadGuard guard(m_childproxieslock);

			// Perform inhibits on DataPoints
			ChildDataPointMapIt itDataPoint;
			for ( itDataPoint = m_childProxies.getDataPoints().begin();
				  itDataPoint != m_childProxies.getDataPoints().end(); 
				  ++itDataPoint )
			{
				DataPointProxySmartPtr* dp = (*itDataPoint).second;  
				try
				{
					if ((*dp)->isWriteable())
					{
						(*dp)->setOutputInhibitStatus(tagOutputInhibitState, sessionIdStr);
					}
					else
					{
						(*dp)->setInputInhibitStatus(tagInputInhibitState, sessionIdStr);
					}
				}
				catch (...)
				{
					// Any error - need to roll back all the other points...
					ChildDataPointMapIt rollbackIt;
					// Increment to make the next loop inclusive
					itDataPoint++;

					for (rollbackIt = m_childProxies.getDataPoints().begin(); 
						 rollbackIt != itDataPoint; 
						 rollbackIt++)
					{
						DataPointProxySmartPtr* rollbackDp = (*rollbackIt).second;   
						try
						{
							if ((*dp)->isWriteable())
							{
								(*rollbackDp)->setOutputInhibitStatus(tagOutputInhibitState, sessionIdStr);
							}
							else
							{
								(*rollbackDp)->setInputInhibitStatus(tagInputInhibitState, sessionIdStr);
							}

						}
						catch (...)
						{
							// Not much we can do here
						}
					}   
                
					TA_THROW(TA_Base_Core::ScadaCommonException("Can not inhibit datapoints - rolled back"));
				} 
			}
        }
		else
		{
			TA_THROW(TA_Base_Core::ScadaCommonException("the child datapoint proxies are not config valid."));
		}

        FUNCTION_EXIT;
    }


    void DataNode::setOutputInhibitStatus(TA_Base_Bus::DataPointAgentInterface& dataPointNamedObject, 
                                          unsigned long entitykey,
                                          TA_Base_Bus::EOutputInhibitState currentInhibit,
                                          TA_Base_Bus::EOutputInhibitState newInhibit,
                                          std::string sessionId)
    {
        TA_Base_Bus::EOutputInhibitState fromdp;
		TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState ret;

		CORBA_CALL_RETURN( ret,
		                   dataPointNamedObject,
		                   getOutputInhibitState,
		                   ( entitykey ) );
		convertOutputInhibitState( ret, fromdp );

        if ( ( currentInhibit != newInhibit) || (currentInhibit != fromdp) )
        {
            try
            {
                TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState toset;
                convertOutputInhibitState(newInhibit, toset);
                CORBA_CALL( dataPointNamedObject, setOutputInhibitState, ( entitykey, sessionId.c_str(), toset ) );//limin
            }   
            catch (...)
            {
                std::string text = "Error: " + m_staticConfig.name + " Internal Error setting DataPoint input inhibit status";
                TA_THROW(TA_Base_Core::ScadaCommonException(text));
            }
        }
		
    }

    void DataNode::setInputInhibitStatus(TA_Base_Bus::DataPointAgentInterface& dataPointNamedObject,
                                         unsigned long entitykey,
                                         TA_Base_Bus::EInputInhibitState currentInhibit,
                                         TA_Base_Bus::EInputInhibitState newInhibit, 
                                         std::string sessionId)
    {
        TA_Base_Bus::EInputInhibitState fromdp;


		TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState ret;
		CORBA_CALL_RETURN( ret,
		                   dataPointNamedObject,
		                   getInputInhibitState,
		                   ( entitykey ) );
		convertInputInhibitState(ret, fromdp);

        if ( (currentInhibit != newInhibit) || (currentInhibit != fromdp ) )
        {
            try
            {
                TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState toset;
                convertInputInhibitState(newInhibit, toset);
                CORBA_CALL( dataPointNamedObject, setInputInhibitState, ( entitykey, sessionId.c_str(),toset ) );//limin
            }   
            catch (...)
            {
                std::string text = "Error: " + m_staticConfig.name + " Internal Error setting DataPoint input inhibit status";
                TA_THROW(TA_Base_Core::ScadaCommonException(text));
            }
        }
    }


    bool DataNode::isActivated()
    {
        //the datanode will be always activated if it is running control mode
		return (m_operationMode == TA_Base_Core::Control);
    }

    
    bool DataNode::areChildProxiesCreated()
    {
        return ( m_childProxies.getDataNodes().size() == m_staticConfig.childDataNodeEntities.size() &&
                 m_childProxies.getDataPoints().size() == m_staticConfig.childDataPointEntities.size() );
    }


    void DataNode::updateActivation()
    {
		boost::shared_ptr<SummaryRequest> request (new SummaryRequest(m_staticConfig.entityKey));
        m_summaryQ->queueItemUnique(request);
    }

    void DataNode::processUpdateActivation()
    {
        std::ostringstream fnName;
        fnName << "updateActivation() - " << m_staticConfig.name;
        FUNCTION_ENTRY (fnName.str().c_str());

        TA_THREADGUARD(m_childproxieslock);

		// create proxies regardless of operation mode
        createProxies();
        FUNCTION_EXIT;
    }

    
    bool DataNode::registerForUpdateDetails ( TA_Base_Bus::IDataNodeObserverNotification * updateDetailsObserver )
    {
        // stop mutil thread modifing the list of observers
        TA_THREADGUARD( m_updateDetailsObserverlock );

        bool result = false;

        // only proceed if there is a valid observer
        if ( 0 != updateDetailsObserver )
        {
            bool existed = false;

            // check if we already have this observer in the list
            std::vector < TA_Base_Bus::IDataNodeObserverNotification * >::iterator itr;
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
            }
            // else do nothing if the specified observer already in the list

            result = true;
        }
        // else do nothing if the specified observer is null

        return result;
    }

    bool DataNode::deregisterForUpdateDetails ( TA_Base_Bus::IDataNodeObserverNotification * updateDetailsObserver )
    {
        // stop mutil thread modifing the list of observers
        TA_THREADGUARD( m_updateDetailsObserverlock );

        bool result = false;

        // only proceed if there is a valid observer
        if ( 0 != updateDetailsObserver )
        {
            bool existed = false;

            // check if we already have this observer in the list
            std::vector < TA_Base_Bus::IDataNodeObserverNotification * >::iterator itr;
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

                result = true;
            }
            // else do nothing is the specified observer is not in the list
        }
        // else do nothing if the specified observer is null

        return result;
    }

    void DataNode::notifyRegisteredObservers ( boost::shared_ptr<TA_Base_Bus::DnSyncUpdate> updateDetails )
    {
        // stop mutil thread modifing the list of observers
        TA_THREADGUARD( m_updateDetailsObserverlock );

        // for each of the registered observers in the list
        std::vector < TA_Base_Bus::IDataNodeObserverNotification * >::iterator itr;
        for ( itr = m_listOfUpdateDetailsObservers.begin();
              itr != m_listOfUpdateDetailsObservers.end();
              itr++ )
        {
            try
            {
                (*itr)->processDataNodeUpdate ( m_staticConfig.entityKey, updateDetails );
            }
            catch ( ... )
            {
            }
        }
    }


    unsigned long DataNode::getEntityKey() const
    {
        return m_staticConfig.entityKey;
    }

    const std::string& DataNode::getName() const
    {
        return m_staticConfig.name;
    }

    boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> DataNode::getCompleteStaticConfigurations()
    {
        boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> toReturn(new TA_Base_Bus::DataNodeStaticConfiguration);
        *toReturn = m_staticConfig;
        return toReturn;
    }

    boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> DataNode::getCompleteDynamicStates()
    {
        boost::shared_ptr<TA_Base_Bus::DnCompleteDynamicSyncState> toReturn(new TA_Base_Bus::DnCompleteDynamicSyncState);
        *toReturn = m_dynamicState;
        return toReturn;
    }

    bool DataNode::getInhibitMMS()
    {
        return m_dynamicState.inhibitMMSState;
    }
        
    boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> DataNode::getDisplayState()
    {
		FUNCTION_ENTRY("getDisplayState");

        boost::shared_ptr<TA_Base_Bus::DataNodeDisplayState> dnstate(new TA_Base_Bus::DataNodeDisplayState);

        //entity key
        dnstate->entityKey = getEntityKey();
        //ack state
        dnstate->alarmAckState.quality  = m_dynamicState.ackSummary.quality;
        dnstate->alarmAckState.value  = m_dynamicState.ackSummary.value;
        //alarm state
        dnstate->alarmState.quality = m_dynamicState.alarmSummary.quality;
        dnstate->alarmState.value = m_dynamicState.alarmSummary.value;
        //quality
        dnstate->quality.quality = m_dynamicState.qualitySummary.quality;
        dnstate->quality.value = m_dynamicState.qualitySummary.value;
        
        std::ostringstream onitp;
		//
		// Override
		//
		{
			if (true == m_dynamicState.overrideSummary.value)
			{
				onitp << 'O';
			}
		}
		//
		// Notes
		//
		{
			bool notes = (0 < m_dynamicState.notes.size());
			
			if ( true == notes )
			{
				onitp << 'N';
			}
		}

		//
		// Inhibit
		//
		{
				
            bool inhibittag = false;
			if ( m_dynamicState.scanInhibitSummary.value )
			{
                inhibittag = true;
			}
            else if (m_dynamicState.alarmInhibitSummary.value) 
            {
                inhibittag = true;
            }
            else if (m_dynamicState.mmsAlarmInhibitSummary.value)
            {
                inhibittag = true;
            }
            else if (m_dynamicState.controlInhibitSummary.value)
            {
                inhibittag = true;
            }
            
			if ( true == inhibittag )
			{
				onitp << 'I';
			}

		}

		//
		// Tag
		//
		{
			if ( TA_Base_Bus::NotTagged != m_dynamicState.tagState.tagState )
			{
				onitp << 'T';
			
				if ( TA_Base_Bus::TaggedPtw == m_dynamicState.tagState.tagState )
				{
					onitp << 'P';
				}
			}
		}
        //ONITP
        dnstate->ONITP = onitp.str();

		FUNCTION_EXIT;
        return dnstate;
    }

    void DataNode::setOperationMode( bool inControlMode )
    {
        std::ostringstream fnName;
        fnName << "setOperationMode() - " << m_staticConfig.name;
        FUNCTION_ENTRY( fnName.str().c_str() );

        changeOperationMode( inControlMode ? TA_Base_Core::Control : TA_Base_Core::Monitor );

        FUNCTION_EXIT;
    }

    void DataNode::changeDataNodeTag( const std::string& sessionId, 
                            TA_Base_Bus::ETagState tagState,
                            TA_Base_Bus::EOutputInhibitState tagOutputInhibitState,
                            TA_Base_Bus::EInputInhibitState tagInputInhibitState,
                            const std::string& ptwId, const std::string& comment)
    {
		std::ostringstream fnName;
        fnName << m_staticConfig.name << " - changeDataNodeTag()";
		FUNCTION_ENTRY( fnName.str().c_str() );

		checkOperationMode();

		if (tagState != TA_Base_Bus::TaggedPtw)
		{
			if ( !ptwId.empty())
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Unexpected PTW ID", m_staticConfig.name.c_str());
				FUNCTION_EXIT;

				throw TA_Base_Core::BadParameterException("changeDataNodeTag: Unexpected PTW ID");
			}
		}
		else
		{
			if( ptwId.empty() || comment.empty() )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Missing PTW ID and Comment for Tagged with PTW", m_staticConfig.name.c_str());
				FUNCTION_EXIT;

				throw TA_Base_Core::BadParameterException("changeDataNodeTag: Missing PTW ID and Comment");
			}
		}
		
		if (tagState == TA_Base_Bus::NotTagged)
		{
			if (tagOutputInhibitState != TA_Base_Bus::NoOutputInhibit ||
				tagInputInhibitState != TA_Base_Bus::NoInputInhibit)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Unexpected Inhibit State", m_staticConfig.name.c_str());
				FUNCTION_EXIT;

				throw TA_Base_Core::BadParameterException("changeDataNodeTag: Unexpected Inhibit State");
			}

			if (!comment.empty())
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Unexpected PTW ID", m_staticConfig.name.c_str());
				FUNCTION_EXIT;

				throw TA_Base_Core::BadParameterException("changeDataNodeTag: Unexpected Comment");
			}
		}

		// if the requested action is not authorised for the sessionID
		if ( (true != isActionPermitted ( sessionId, TA_Base_Bus::aca_DATA_NODE_TAG )) && 
			 (true != isActionPermitted( sessionId, TA_Base_Bus::aca_DATA_NODE_TAG_PTW)) )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "changeDataNodeTag() - %s - Access denied",
                m_staticConfig.name.c_str() );
			FUNCTION_EXIT;

			throw TA_Base_Core::AccessDeniedException();
		}

		// Inhibit DataPoints first
		try
		{
			inhibitDataPoints(tagOutputInhibitState, tagInputInhibitState, sessionId);
		}
		catch (TA_Base_Core::ScadaCommonException& e)
		{
			throw TA_Base_Core::ScadaCommonException(e.what());
		}

		TA_Base_Bus::ETagState originalTagState = m_dynamicState.tagState.tagState;
		std::string originalPtwId = m_dynamicState.tagState.ptwId;
		std::string originalComment = m_dynamicState.tagState.comment;
		TA_Base_Bus::InhibitState originalInhibitState = m_dynamicState.tagState.tagInhibitState;
      
        //
		// Persist to database first
        //
        TA_ASSERT( m_persistence.get() != NULL, "ScadaPersistence has not been initialised" );
        if ( ! m_persistence->writePersistedValue( DN_TAG_STATUS, m_staticConfig.entityKey, tagStateToString(tagState) ) )
		{
			throw TA_Base_Core::DatabaseErrorException("changeDataNodeTag: Can not persist to Database");
        }

        if ( ! m_persistence->writePersistedValue( DN_TAG_OUTPUT_INHIBIT, m_staticConfig.entityKey, 
                                                  tagOutputInhibitStateToString(tagOutputInhibitState) ) )
        {
			if ( ! m_persistence->writePersistedValue( DN_TAG_STATUS, m_staticConfig.entityKey, tagStateToString( originalTagState ) ) )
			{
				// Rollback Inhibits
				try
				{
					inhibitDataPoints(originalInhibitState.outputInhibit, originalInhibitState.inputInhibit, sessionId);
				}
				catch (TA_Base_Core::ScadaCommonException&)
				{
				}
			}
			throw TA_Base_Core::DatabaseErrorException("changeDataNodeTag: Can not persist to Database");
		}
			
		if ( ! m_persistence->writePersistedValue( DN_TAG_INPUT_INHIBIT, m_staticConfig.entityKey, 
                                                   tagInputInhibitStateToString(tagInputInhibitState) ) )
		{
            // Rollback Inhibits
            try
            {
                inhibitDataPoints(originalInhibitState.outputInhibit, originalInhibitState.inputInhibit, sessionId);
            }
            catch (TA_Base_Core::ScadaCommonException&)
            {
            }

            m_persistence->writePersistedValue( DN_TAG_STATUS, m_staticConfig.entityKey, tagStateToString( originalTagState ) );
            m_persistence->writePersistedValue( DN_TAG_OUTPUT_INHIBIT, m_staticConfig.entityKey, 
                                                tagOutputInhibitStateToString(originalInhibitState.outputInhibit) );

			throw TA_Base_Core::DatabaseErrorException("changeDataNodeTag: Can not persist to Database");
		}

		if ( ! m_persistence->writePersistedValue( DN_TAG_PTW, m_staticConfig.entityKey, ptwId ) )
		{
            // Rollback Inhibits
            try
            {
                inhibitDataPoints(originalInhibitState.outputInhibit, originalInhibitState.inputInhibit, sessionId);
            }
            catch (TA_Base_Core::ScadaCommonException&)
            {
            }

            m_persistence->writePersistedValue( DN_TAG_STATUS, m_staticConfig.entityKey, tagStateToString( originalTagState ) );
            m_persistence->writePersistedValue( DN_TAG_OUTPUT_INHIBIT, m_staticConfig.entityKey, 
                                                tagOutputInhibitStateToString(originalInhibitState.outputInhibit) );
            m_persistence->writePersistedValue( DN_TAG_INPUT_INHIBIT, m_staticConfig.entityKey, 
                                                tagInputInhibitStateToString(originalInhibitState.inputInhibit) );

			throw TA_Base_Core::DatabaseErrorException("changeDataNodeTag: Can not persist to Database");
		}
			
        if ( ! m_persistence->writePersistedValue( DN_PTW_COMMENTS, m_staticConfig.entityKey, comment ) )
		{
            // Rollback Inhibits
            try
            {
                inhibitDataPoints(originalInhibitState.outputInhibit, originalInhibitState.inputInhibit, sessionId);
            }
            catch (TA_Base_Core::ScadaCommonException&)
            {
            }

            m_persistence->writePersistedValue( DN_TAG_STATUS, m_staticConfig.entityKey, tagStateToString( originalTagState ) );
            m_persistence->writePersistedValue( DN_TAG_OUTPUT_INHIBIT, m_staticConfig.entityKey, 
                                                tagOutputInhibitStateToString(originalInhibitState.outputInhibit) );
            m_persistence->writePersistedValue( DN_TAG_INPUT_INHIBIT, m_staticConfig.entityKey, 
                                                tagInputInhibitStateToString(originalInhibitState.inputInhibit) );
            m_persistence->writePersistedValue( DN_TAG_PTW, m_staticConfig.entityKey, originalPtwId );

			throw TA_Base_Core::DatabaseErrorException("changeDataNodeTag: Can not persist to Database");
		}

		// Update local store
		m_dynamicState.tagState.tagState = tagState;
		m_dynamicState.tagState.tagInhibitState.outputInhibit = tagOutputInhibitState;
		m_dynamicState.tagState.tagInhibitState.inputInhibit = tagInputInhibitState;
		m_dynamicState.tagState.ptwId = ptwId;
		m_dynamicState.tagState.comment = comment;
        saveLastOperatorInfo(sessionId);	
		// Send update event
		submitUpdateEvent (TagStateUpdate);

		//
		// submit audit event
		//

		std::stringstream inhibitStateStr;
		inhibitStateStr << "With";
		switch ( m_dynamicState.tagState.tagInhibitState.inputInhibit )
		{
			case NoInputInhibit:
			{
				if ( TA_Base_Bus::NoOutputInhibit == m_dynamicState.tagState.tagInhibitState.outputInhibit )
				{
					inhibitStateStr << " No Inhibit";
				}
				
				break;
			}
			case InhibitScan:
			{
				inhibitStateStr << " Scan Inhibit";
				break;
			}
			case InhibitAlarm:
			{
				inhibitStateStr << " Alarm Inhibit";
				break;
			}
			case InhibitMmsAlarm:
			{
				inhibitStateStr << " MMS Inhibit";
				break;
			}
			default:
			{
				break;
			}
		}

        if ( TA_Base_Bus::NoOutputInhibit == m_dynamicState.tagState.tagInhibitState.outputInhibit )
		{
			inhibitStateStr << " Control Inhibit";
		}

		// construct the DescriptionParameters
		TA_Base_Core::DescriptionParameters descriptionParameters;
		descriptionParameters.clear();
		TA_Base_Core::NameValuePair pair1 ( "EquipmentDescription", m_staticConfig.description );
		descriptionParameters.push_back ( &pair1 );

		// Tag Added
		if ( ( TA_Base_Bus::Tagged != originalTagState ) &&
			 ( TA_Base_Bus::Tagged == m_dynamicState.tagState.tagState ) )
		{				
			TA_Base_Core::NameValuePair pair2 ( "InhibitState", inhibitStateStr.str() );				
			descriptionParameters.push_back ( &pair2 );

			submitAuditEvent ( sessionId, descriptionParameters, TA_Base_Core::DataNodeAudit::DataNodeTagAdd );
		}

		// Tag With PTW Added
		else if ( ( TA_Base_Bus::TaggedPtw != originalTagState ) &&
				  ( TA_Base_Bus::TaggedPtw == m_dynamicState.tagState.tagState ) )
		{				
			TA_Base_Core::NameValuePair pair2 ( "InhibitState", inhibitStateStr.str() );
			descriptionParameters.push_back ( &pair2 );

			submitAuditEvent ( sessionId, descriptionParameters, TA_Base_Core::DataNodeAudit::DataNodePTWAdd );
		}

		// Tag Removed
		else if ( ( TA_Base_Bus::Tagged == originalTagState ) &&
				  ( TA_Base_Bus::Tagged != m_dynamicState.tagState.tagState ) )
		{
			submitAuditEvent ( sessionId, descriptionParameters, TA_Base_Core::DataNodeAudit::DataNodeTagRemove );
		}

		// Tag With PTW Removed
		else if ( ( TA_Base_Bus::TaggedPtw == originalTagState ) &&
				  ( TA_Base_Bus::TaggedPtw != m_dynamicState.tagState.tagState ) )
		{				
			submitAuditEvent ( sessionId, descriptionParameters, TA_Base_Core::DataNodeAudit::DataNodePTWRemove );
		}

		FUNCTION_EXIT;
    }


    void DataNode::setNotes(const std::string& sessionId, const std::string& notes)
    {
		std::ostringstream fnName;
        fnName << m_staticConfig.name << " - setDataNodeNotes()";
		FUNCTION_ENTRY( fnName.str().c_str() );

		checkOperationMode();

		// if the requested action is authorised for the sessionID
		if ( false == isActionPermitted ( sessionId, TA_Base_Bus::aca_DATA_NODE_NOTES ) )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "setDataNodeNotes() - %s - Access denied", m_staticConfig.name.c_str());
			FUNCTION_EXIT;

			throw TA_Base_Core::AccessDeniedException();
		}

		{
			try
			{
				// persist to entity first
                m_persistence->writePersistedValue( DN_NOTES, m_staticConfig.entityKey, notes );

				// Update local store for notes
				m_dynamicState.notes = notes;
			}
			catch (...)
			{
				throw TA_Base_Core::DatabaseErrorException("setDataNodeNotes: Can not persist to Database");
			}
		
			// Send update event
			submitUpdateEvent (NotesStateUpdate);

			//
			// submit audit message
			//

			// construct the DescriptionParameters
			TA_Base_Core::DescriptionParameters descriptionParameters;
			descriptionParameters.clear();
			TA_Base_Core::NameValuePair pair1 ( "EquipmentDescription", m_staticConfig.description );
			descriptionParameters.push_back ( &pair1 );

			submitAuditEvent ( sessionId, descriptionParameters, ( false == notes.empty() ? TA_Base_Core::DataNodeAudit::DataNodeNotesAdd : TA_Base_Core::DataNodeAudit::DataNodeNotesRemove ) );

		}

		FUNCTION_EXIT;
    }

    void DataNode::setInhibitMMS( const std::string& sessionID, bool inhibitMms )
    {
		std::ostringstream fnName;
        fnName << m_staticConfig.name << " - setInhibitMMS()";
		FUNCTION_ENTRY( fnName.str().c_str() );

		checkOperationMode();		
		{

			// only proceed if there is a difference
			if ( inhibitMms != m_dynamicState.inhibitMMSState )
			{
				// if the requested action is authorised for the sessionID
				if ( true == isActionPermitted ( sessionID, ( true == inhibitMms ? TA_Base_Bus::aca_DATA_NODE_MMS_INHIBIT : TA_Base_Bus::aca_DATA_NODE_MMS_UNINHIBIT ) ) )
				{
					// persist new state
                    TA_ASSERT( m_persistence.get() != NULL, "ScadaPersistence is not initialised" );
                    m_persistence->writePersistedValue( DN_MMS_INHIBIT_STATE, m_staticConfig.entityKey, inhibitMms );

					// update local MMS inhibit state
					m_dynamicState.inhibitMMSState = inhibitMms;
                    saveLastOperatorInfo(sessionID);

					// send update event
					submitUpdateEvent ( InhibitStateUpdate );
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "setInhibitMMS() - %s - Access denied", m_staticConfig.name.c_str());
					FUNCTION_EXIT;

					throw TA_Base_Core::AccessDeniedException();
				}
			}
		}

		FUNCTION_EXIT;
    }

	void DataNode::saveLastOperatorInfo ( const std::string & sessionID )
	{
        FUNCTION_ENTRY("saveLastOperatorInfo");
		struct timeb currentSystemTime;
		ftime ( &currentSystemTime );
		m_dynamicState.lastOperator.sessionID = sessionID;
		m_dynamicState.lastOperator.timeStamp.time = currentSystemTime.time;
		m_dynamicState.lastOperator.timeStamp.millitm = currentSystemTime.millitm;
        FUNCTION_EXIT;
	}

    void DataNode::clearSummaries()
    {
        m_dynamicState.qualitySummary.value = QUALITY_BAD;
        m_dynamicState.qualitySummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.alarmSummary.value = QUALITY_BAD;
        m_dynamicState.alarmSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.ackSummary.value = QUALITY_BAD;
        m_dynamicState.ackSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.scanInhibitSummary.value = QUALITY_BAD;
        m_dynamicState.scanInhibitSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.alarmInhibitSummary.value = QUALITY_BAD;
        m_dynamicState.alarmInhibitSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.mmsAlarmInhibitSummary.value = QUALITY_BAD;
        m_dynamicState.mmsAlarmInhibitSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.controlInhibitSummary.value = QUALITY_BAD;
        m_dynamicState.controlInhibitSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.overrideSummary.value = QUALITY_BAD;
        m_dynamicState.overrideSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;

        m_dynamicState.controlPendingSummary.value = QUALITY_BAD;
        m_dynamicState.controlPendingSummary.quality = QUALITY_BAD_NO_SPECIFIC_REASON;
    }

    
    void DataNode::updateSynState(const TA_Base_Bus::DnSyncUpdate& toupdate)
    {
        FUNCTION_ENTRY("updateSynState");
        //only the monitor mode datanode required to syn the control mode
        if ( TA_Base_Core::Control == m_operationMode )
        {
            throw( TA_Base_Core::OperationModeException() );
        }
        
        switch(toupdate.updateType) 
        {		
        case DnCompleteSyncState:
            m_dynamicState = toupdate.completeUpdate;
            break;
        case DnTagStateUpdate:
            m_dynamicState.tagState = toupdate.tagState;
            break;
        case DnAlarmSummaryUpdate:
            m_dynamicState.alarmSummary = toupdate.alarmSummary;
            break;
        case DnAlarmAckSummaryUpdate:
            m_dynamicState.ackSummary = toupdate.ackSummary;
            break;
        case DnScanInhibitSummaryUpdate:
            m_dynamicState.scanInhibitSummary = toupdate.scanInhibitSummary;
            break;
        case DnAlarmInhibitSummaryUpdate:
            m_dynamicState.alarmInhibitSummary = toupdate.alarmInhibitSummary;
            break;
        case DnMmsAlarmInhibitSummaryUpdate:
            m_dynamicState.mmsAlarmInhibitSummary = toupdate.mmsAlarmInhibitSummary;
            break;
        case DnControlInhibitSummaryUpdate:
            m_dynamicState.controlInhibitSummary = toupdate.controlInhibitSummary;
            break;
        case DnOverrideSummaryUpdate:
            m_dynamicState.overrideSummary = toupdate.overrideSummary;
            break;
        case DnQualitySummaryUpdate:
            m_dynamicState.qualitySummary = toupdate.qualitySummary;
            break;
        case DnControlPendingSummaryUpdate:
            m_dynamicState.controlPendingSummary = toupdate.controlPendingSummary;
            break;
        case DnNotesUpdate:
            m_dynamicState.notes = toupdate.notes;
            break;
        case DnInhibitMMSUpdate:
            m_dynamicState.inhibitMMSState = toupdate.inhibitMMSState;
            break;
        case DnLastOperatorUpdate:
            m_dynamicState.lastOperator = toupdate.lastOperator;
            break;
        default:
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Receive unknown update type[%d] in Monitor mode datanode [%d]", toupdate.updateType, getEntityKey() );
            break;
        }
        FUNCTION_EXIT;
    }
    
}
