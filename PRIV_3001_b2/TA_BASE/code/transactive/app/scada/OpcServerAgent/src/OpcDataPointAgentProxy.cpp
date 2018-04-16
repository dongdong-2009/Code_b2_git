 /**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcServerAgent/src/OpcDataPointAgentProxy.cpp $
  * @author:  Dhanshri Mokashi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2016/06/27 17:34:18 $
  * Last modified by:  $Author: dhanshri $
  *
  */
// OpcDataPointAgentProxy.cpp: implementation of the OpcDataPointAgentProxy class.
// Fetches data from subscribed agents in form of datapoints via CORBA connection
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <sstream>
#include "time.h"
#include "boost/cast.hpp"
#include "ace/OS.h"
#include "OpcDataPointAgentProxy.h"
#include "OpcServerManager.h"

#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ScadaCommonException.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    const unsigned short BULK_POLL_COMMS_ERROR_RETRY_PERIOD_SECS = 15;
    const unsigned short SET_BAD_QUALITY_RETRY_PERIOD_SECS = 10;
	
    OpcDataPointAgentProxy::OpcDataPointAgentProxy(unsigned long agentKey, std::vector<unsigned long> dpMap, OpcServerManager* pointMgr)
        :
        m_toBeTerminated(false),
        m_isSynchronising(false),
		m_isPrevSynchronising(true),
		m_isAllBadQuality(false),
        m_observerId(0),
        m_agentDPInterface(NULL),
		m_agentKey(agentKey),
        m_agentName(""),
		m_dpList(dpMap),
        m_pollPeriod(500),
        m_retryPeriod(10),
		m_pOpcServerManager(pointMgr)
    {
        m_pollPeriod = m_pOpcServerManager->getPollFrequency();
		m_retryPeriod = m_pOpcServerManager->getRetryperiod();
		start();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Bulk poll period=[%u] ms, retry period=[%u], for agent [%u]", m_pollPeriod, m_retryPeriod, m_agentKey);
    }
    
    OpcDataPointAgentProxy::~OpcDataPointAgentProxy()
    {
        terminateAndWait();

        if (m_agentDPInterface != NULL)
        {
            delete m_agentDPInterface;
			m_agentDPInterface = NULL;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OpcDataPointAgentProxy [%s] destroyed", m_agentName.c_str());
    }
    
    void OpcDataPointAgentProxy::terminate()
    {
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In OpcDataPointAgentProxy::terminate().");
		m_toBeTerminated = true;
		m_conditionLoop.signal();
    }

    void OpcDataPointAgentProxy::startSynchronise()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "startSynchronise() for m_agentKey %u", m_agentKey);

        if ( !m_isSynchronising )
        {
			m_isPrevSynchronising = false;
            m_isSynchronising = true;
        }
    }
    
    void OpcDataPointAgentProxy::stopSynchronise()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopSynchronise() for m_agentKey %u", m_agentKey);

        if ( m_isSynchronising )
        {
			m_isPrevSynchronising= true;
	        m_isSynchronising = false;
        }
    }
    
    bool OpcDataPointAgentProxy::isSynchronised()
    {
        return m_isSynchronising;
    }
    
	void OpcDataPointAgentProxy::run()
	{
		try
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "OpcDataPointAgentProxy::run() starting up...");
			TA_Base_Core::CorbaName corbaName = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(m_agentKey, true);
			m_agentName = corbaName.getObjectName();
			m_agentDPInterface = new DataPointAgentInterface();
			m_agentDPInterface->setCorbaName(m_agentName, m_agentName + TA_Base_Bus::DP_BULK_POLLED_INTERFACE);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "OpcDataPointAgentProxy get corba name for Agent:%u failed ", m_agentKey);
		}

		if (NULL == m_agentDPInterface) 
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "OpcDataPointAgentProxy:[%s] is null, returning.", m_agentName.c_str());
			return;
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "OpcDataPointAgentProxy:[%s] run...", m_agentName.c_str());

		bool logskipstep = true;
		unsigned long maxSkipSteps = BULK_POLL_COMMS_ERROR_RETRY_PERIOD_SECS * 1000 / m_pollPeriod;
		unsigned long skipSteps = 0; //to skip some loops when can't connect to agent
		unsigned long lossCount = 0;

		while( !m_toBeTerminated )
		{
			try
			{
				if (m_isSynchronising)
				{
					if (false == m_isPrevSynchronising)
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "m_isSynchronising %d, m_isPrevSynchronising %d.", 
								m_isSynchronising , m_isPrevSynchronising );
						m_isPrevSynchronising = true;
					}
					if ((skipSteps % maxSkipSteps) == 0)
					{                                    
						TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* dataPointUpdates = NULL;

						if (0 == m_observerId)
						{
							CORBA_CALL_RETURN( m_observerId, (*m_agentDPInterface), regiesterDisplayDataPointClient, () );

							if (m_observerId > 0 && !m_toBeTerminated)
							{
								LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "First time, get all update from agent, m_observerId = %u", m_observerId );
								CORBA_CALL_RETURN( dataPointUpdates, (*m_agentDPInterface), getDisplayDataPointUpdate, ( m_observerId, true ) );
							}
							else
							{
								LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to register: m_observerId %u, m_toBeTerminated %d",
										m_observerId , m_toBeTerminated );
								m_isAllBadQuality = true;
								setAllToBadQuality();// check for testcase
								continue;
							}
						}
						else
						{
							CORBA_CALL_RETURN( dataPointUpdates, (*m_agentDPInterface), getDisplayDataPointUpdate, ( m_observerId, false ) );
							LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
								"Update from agent: m_observerId = %u, m_agentName %s", m_observerId, m_agentName.c_str() );

						}

						skipSteps = 0;
						lossCount = 0;
						processUpdates(dataPointUpdates);
						m_isAllBadQuality = false;
					}
					else
					{
						if (logskipstep)
						{
							logskipstep = false;
							LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "dp skip [%u] agent [%s], synch:1, loss:%lu", 
								skipSteps, m_agentName.c_str(), lossCount);
						}
						++skipSteps;
					}
				}
				else
				{
					if (m_isPrevSynchronising)
					{
						setAllToBadQuality();
						m_isPrevSynchronising = false;

						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "dp skip [%u] agent [%s], synch:0, loss:%lu", 
							skipSteps, m_agentName.c_str(), lossCount);
						skipSteps = 0;
					}
				}
			}
			catch ( const TA_Base_Core::ObjectResolutionException & ex )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
				setAllToBadQuality();
				// only skip poll loop when the agent can't be resolved
				if(lossCount++ < 20)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Resetting skipSteps to 0 as lossCount < 20");
					skipSteps = 0;
				}
				else
				{
					logskipstep = true;
					++skipSteps;
				}
			}
			catch ( const TA_Base_Core::ScadaProxyException & ex )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException" ,ex.what() );
				setAllToBadQuality();
			}
			catch ( const TA_Base_Core::BadParameterException & )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", "Caught Bad Parameter exception" );
				setAllToBadQuality(TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR);
			}
			catch ( const TA_Base_Core::OperationModeException & )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
				// force to rebind named object next time.
				(*m_agentDPInterface).clear();
				setAllToBadQuality(TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR);
			}
			catch ( const CORBA::Exception & ex )
			{
				LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
				// force to rebind named object next time.
				(*m_agentDPInterface).clear();
				setAllToBadQuality(TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR);
			}
			catch ( ... )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception...");
				setAllToBadQuality(TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON);
			}

			m_conditionLoop.timedWait(m_pollPeriod);
		}
		prepareAgentThreadEnd();
	}

	void OpcDataPointAgentProxy::setAllToBadQuality(TA_Base_Bus::EQualityStatus eq)
    {
   		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "OpcDataPointAgentProxy::setAllToBadQuality() = %d, value = %d, observer_id [%u]", m_isAllBadQuality, eq, m_observerId);
		if (false == m_isAllBadQuality)
		{
			TA_Base_Bus::DisplayStateSequence*  pointUpdates = new TA_Base_Bus::DisplayStateSequence();
			pointUpdates->clear();

			for (unsigned int i = 0; i < m_dpList.size() && !m_toBeTerminated; ++i)
			{
				TA_Base_Bus::DataPointDisplayState data;
				//
				//Get EKs from List and put all to Bad quality.
				//
				data.entityKey = m_dpList[i];
				data.alarmAckState = ScadaAlarmNotPresent;//which is 2, ScadaAlarmNotPresent
				data.alarmState = false;
				data.dataPointStatus = (unsigned long)eq;//bad quality indicator.
				//NEED to check this when agent is UP then Down then again UP
				data.value.dpType = DPT_BOOL_TYPE;
				data.value.booleanValue = false;
				data.value.enumValue = 0;
				data.value.floatValue = 0;
				pointUpdates->push_back(data);
			}
			m_pOpcServerManager->submitUpdateData(pointUpdates);
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Finished to reset observer of Agent [%s], observer_id [%u]", m_agentName.c_str(), m_observerId);
		}
		m_isAllBadQuality = true;
        m_observerId = 0;
    }

	void OpcDataPointAgentProxy::processUpdates(TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* dpUpdates)
	{
		unsigned int updateSize = 0;
		if (NULL == dpUpdates)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "dpUpdates is NULL, m_toBeTerminated = %d", m_toBeTerminated );
			return ;
		}

		if (0 != dpUpdates)
		{
			updateSize = dpUpdates->length();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "updateSize = %d" , updateSize);

		if (NULL != dpUpdates && !m_toBeTerminated && updateSize > 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Observer ID %u received %u data point updates from agent %s",
					m_observerId, updateSize, m_agentName.c_str());
			try
			{
				TA_Base_Bus::DisplayStateSequence*  pointUpdates = new TA_Base_Bus::DisplayStateSequence();
				pointUpdates->clear();
				//pointUpdates->resize(updateSize); maybe some dp no need.

				for (unsigned int i = 0; i < updateSize && !m_toBeTerminated; ++i)
				{
					TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState&  dataPointUpdate = (*dpUpdates)[i];

					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "OpcDataPointAgentProxy %s - received an update for entity %u",
						m_agentName.c_str(), dataPointUpdate.entityKey);

					//Note - Need not check if m_dpList has this EK dataPointUpdate.entityKey
					// Convert from CORBA to ScadaType
					TA_Base_Bus::DataPointDisplayState data;
					//
					//convertDpDisplayUpdate(const TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState& displayInfo, TA_Base_Bus::DataPointDisplayState& ret);
					//
					convertDpDisplayUpdate(dataPointUpdate, data);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Updating data.EK %u, data.dataPointStatus %u", data.entityKey , data.dataPointStatus);
					pointUpdates->push_back(data);
				}
				m_pOpcServerManager->submitUpdateData(pointUpdates);
			}
			catch (...) 
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
			}							
		}
		if ( NULL != dpUpdates )
		{
			delete dpUpdates;
			dpUpdates = NULL;
		}
	}

	void OpcDataPointAgentProxy::prepareAgentThreadEnd()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--stopping OpcDataPointAgentProxy thread for Agent [%s]", m_agentName.c_str());

		if (m_observerId > 0) 
		{
			try
			{
				CORBA_CALL( (*m_agentDPInterface), unregiesterDisplayDataPointClient, ( m_observerId ) );
			}
			catch (...) 
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--stopping OpcDataPointAgentProxy thread for Agent [%s] unknown exception", m_agentName.c_str());
			}

			setAllToBadQuality();
		}	
	}
}
