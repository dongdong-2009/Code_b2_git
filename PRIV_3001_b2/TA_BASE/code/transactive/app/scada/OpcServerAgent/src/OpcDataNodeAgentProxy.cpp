/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcBridgeLib/src/OpcDataNodeAgentProxy.cpp $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/07/12 15:49:47 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// OpcDataNodeAgentProxy.cpp: implementation of the OpcDataNodeAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)


#include <sstream>
#include "time.h"
#include "boost/cast.hpp"
#include "ace/OS.h"
#include "OpcDataNodeAgentProxy.h"
#include "OpcServerManager.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ScadaCommonException.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"

#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    const unsigned short BULK_POLL_COMMS_ERROR_RETRY_PERIOD_SECS = 15;
    const unsigned short SET_BAD_QUALITY_RETRY_PERIOD_SECS = 10;

    OpcDataNodeAgentProxy::OpcDataNodeAgentProxy(unsigned long agentKey, std::vector<unsigned long> dnMap, OpcServerManager* proxyManager)
        :
        m_toBeTerminated(false),
        m_isSynchronising(false),
		m_isPrevSynchronising(true),
        m_observerId(0),
        m_agentDNInterface(NULL),
		m_agentKey(agentKey),
		m_agentName(""),
		m_dnList(dnMap),
		m_isAllBadQuality(false),
        m_pOpcServerManager(proxyManager),
        m_pollPeriod(500),
        m_retryPeriod(10),
        m_connected(true),
        m_timeLastConnected(time(NULL))
    {
        m_pollPeriod = m_pOpcServerManager->getPollFrequency();
		m_retryPeriod = m_pOpcServerManager->getRetryperiod();
		start();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Bulk poll period=[%u] ms, retry period=[%u], for agent [%u]", m_pollPeriod, m_retryPeriod, m_agentKey);
    }

    OpcDataNodeAgentProxy::~OpcDataNodeAgentProxy()
    {
        terminateAndWait();
        
        if (m_agentDNInterface != NULL)
        {
            delete m_agentDNInterface;
			m_agentDNInterface = NULL;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MarkB--OpcDataNodeAgentProxy [%s] destroyed", m_agentName.c_str());
    }
    
    void OpcDataNodeAgentProxy::terminate()
    {
        m_toBeTerminated = true;
		m_conditionLoop.signal();
    }
    
    void OpcDataNodeAgentProxy::startSynchronise()
    {
        if ( !m_isSynchronising )
        {
            m_isSynchronising = true;
			m_isPrevSynchronising = false;
        }
    }
    
    void OpcDataNodeAgentProxy::stopSynchronise()
    {
        if ( m_isSynchronising )
        {
            m_isSynchronising = false;
			m_isPrevSynchronising = true;
        }
    }
    
    bool OpcDataNodeAgentProxy::isSynchronised()
    {
        return m_isSynchronising;
    }
    
	void OpcDataNodeAgentProxy::run()
	{
		try
		{
			TA_Base_Core::CorbaName corbaName = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(m_agentKey, true);
			m_agentName = corbaName.getObjectName();
			m_agentDNInterface = new DataNodeAgentInterface();
			m_agentDNInterface->setCorbaName(m_agentName, m_agentName + TA_Base_Bus::DN_BULK_POLLED_INTERFACE);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--OpcDataNodeAgentProxy get corba name for Agent:%u failed ", m_agentKey);
		}

		if (NULL == m_agentDNInterface) return;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--DNAgent:[%s] run", 
			m_agentName.c_str());

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
					if ((skipSteps % maxSkipSteps) == 0)
					{                                        
	                    TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* dataNodeUpdates = NULL;

						if (0 == m_observerId)
	                    {
							CORBA_CALL_RETURN( m_observerId, (*m_agentDNInterface), regiesterDisplayDataNodeClient, () );

	                        if (m_observerId > 0 && !m_toBeTerminated)
	                        {
	                            //first time, get all update from agent
	                            CORBA_CALL_RETURN( dataNodeUpdates, (*m_agentDNInterface), getDisplayDataNodeUpdate, ( m_observerId, true ) );
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
							CORBA_CALL_RETURN( dataNodeUpdates, (*m_agentDNInterface), getDisplayDataNodeUpdate, ( m_observerId, false ) );			
	                    }

	                    skipSteps = 0; //reset skipSteps when connected to agent
	                    m_timeLastConnected = time(NULL);
	                    m_connected = true;
						lossCount = 0;
	                    processUpdates(dataNodeUpdates);
						m_isAllBadQuality = false;
	                }
	                else
	                {
						if (logskipstep)
						{
							logskipstep = false;
							LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "dn skip [%u] agent [%s], synch:1, loss:%lu", 
								skipSteps, m_agentName.c_str(), lossCount);
						}
						++skipSteps;
	                }
                }
				else
				{
					if (m_isPrevSynchronising)
					{
						m_isPrevSynchronising = false;

	                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "dn skip [%u] agent [%s], synch:0, loss:%lu", 
							skipSteps, m_agentName.c_str(), lossCount);
						skipSteps = 0;
					}					
				}
            }
            catch ( const TA_Base_Core::ObjectResolutionException & ex )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );			
                setAllToBadQuality (  );
                // only skip poll loop when the agent can't be resolved
				if(lossCount++ < 20)
				{
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
				setAllToBadQuality (  );
            }
            catch ( const TA_Base_Core::BadParameterException & )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", "Caught Bad Parameter exception" );
                // set all data point to bad quality status and restart the polling cycle
                // by requesting the agent for new observer ID
				setAllToBadQuality (TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR  );
            }
            catch ( const TA_Base_Core::OperationModeException & )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
                // force to rebind named object next time.
                (*m_agentDNInterface).clear();
                // set all data point to bad quality status and restart the polling cycle
                // by requesting the agent for new observer ID
                setAllToBadQuality (  TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR);
            }
            catch ( const CORBA::Exception & ex )
            {
                LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
                // force to rebind named object next time.
                (*m_agentDNInterface).clear();     
                setAllToBadQuality ( TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception...");
				setAllToBadQuality ( TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON );
            }

			m_conditionLoop.timedWait(m_pollPeriod);
        }

		prepareAgentThreadEnd();
	}

    void OpcDataNodeAgentProxy::setAllToBadQuality(TA_Base_Bus::EQualityStatus eq)
    {
   		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "OpcDataNodeAgentProxy::setAllToBadQuality() = %d, value = %d, observer_id [%u]", m_isAllBadQuality, eq, m_observerId);
		if (false == m_isAllBadQuality)
		{
			TA_Base_Bus::DataNodeDisplayStateVector*  pointUpdates = new TA_Base_Bus::DataNodeDisplayStateVector();
			pointUpdates->clear();

			for (unsigned int i = 0; i < m_dnList.size() && !m_toBeTerminated; ++i)
			{
				TA_Base_Bus::DataNodeDisplayState data;
				//
				//Get EKs from List and put all to Bad quality.
				//
				data.entityKey = m_dnList[i];
				data.ONITP = "";

				data.alarmState.value = false;
				data.alarmState.quality = eq;

				data.alarmAckState.value = (unsigned long)eq;
				data.alarmAckState.quality = eq;

				data.quality.value = (unsigned long)eq;
				data.quality.quality = eq;

				pointUpdates->push_back(data);
			}
			m_pOpcServerManager->submitUpdateData(pointUpdates);
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Finished to reset observer of Agent [%s], observer_id [%u]", m_agentName.c_str(), m_observerId);
		}
		m_isAllBadQuality = true;
        m_observerId = 0;
     }
	
	void OpcDataNodeAgentProxy::processUpdates(TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* dnUpdates)
	{
		unsigned int updateSize = 0;
		if (NULL == dnUpdates)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "dnUpdates is NULL, m_toBeTerminated = %d", m_toBeTerminated );
			return ;
		}

		if (0 != dnUpdates)
		{
			updateSize = dnUpdates->length();
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "updateSize = %d" , updateSize);

		if (NULL != dnUpdates && !m_toBeTerminated && updateSize > 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Observer ID %u received %u data node updates from agent %s",
					m_observerId, updateSize, m_agentName.c_str());
			try
			{
				TA_Base_Bus::DataNodeDisplayStateVector*  pointUpdates = new TA_Base_Bus::DataNodeDisplayStateVector();
				pointUpdates->clear();
				//pointUpdates->resize(updateSize); maybe some dp no need.

				for (unsigned int i = 0; i < updateSize && !m_toBeTerminated; ++i)
				{
					TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState&  dataNodeUpdate = (*dnUpdates)[i];

					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "OpcDataNodeAgentProxy %s - received an update for entity %u",
						m_agentName.c_str(), dataNodeUpdate.entityKey);

					// Note - Need not check if m_dpList has this EK dataNodeUpdate.entityKey
					// Convert from CORBA to ScadaType
					TA_Base_Bus::DataNodeDisplayState data;
					//
					//convertDnDisplayUpdate(const TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState& displayInfo, TA_Base_Bus::DataNodeDisplayState& ret);
					//
					convertDnDisplayUpdate(dataNodeUpdate, data);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Updating data.EK %u, data.quality.value %d", data.entityKey , data.quality.value);
					pointUpdates->push_back(data);
				}
				m_pOpcServerManager->submitUpdateData(pointUpdates);
			}
			catch (...) 
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
			}							
		}
		if ( NULL != dnUpdates )
		{
			delete dnUpdates;
			dnUpdates = NULL;
		}
	}

	void OpcDataNodeAgentProxy::prepareAgentThreadEnd()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "MarkB--stopping OpcDataNodeAgentProxy thread for Agent [%s]", m_agentName.c_str());

		if (m_observerId > 0) 
		{
			try
			{
				CORBA_CALL( (*m_agentDNInterface), unregiesterDisplayDataNodeClient, ( m_observerId ) );
			}
			catch (...) 
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "MarkB--stopping OpcDataNodeAgentProxy thread for Agent [%s] unknown exception", m_agentName.c_str());
			}

			setAllToBadQuality();
		}	
	}
}
