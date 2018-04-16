 /**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/ModbusServerAgent/src/ModbusPointAgentProxy.cpp $
  * @author:  Ripple
  * @version: $Revision: #18 $
  *
  * Last modification: $DateTime: 2017/07/07 13:35:34 $
  * Last modified by:  $Author: dhanshri $
  *
  */
// ModbusPointAgentProxy.cpp: implementation of the ModbusPointAgentProxy class.
// Fetches data from subscribed agents in form of datapoints via CORBA connection
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <sstream>
#include "time.h"
#include "boost/cast.hpp"
#include "ace/OS.h"
#include "app/scada/ModbusServerAgent/src/ModbusPointAgentProxy.h"
#include "ModbusPointManager.h"

#include "bus/scada/common_library/IDL/src/DataPointCorbaTypes.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/data_access_interface/src/IModbusServerAnaloguePoint.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    const unsigned short BULK_POLL_COMMS_ERROR_RETRY_PERIOD_SECS = 15;
    const unsigned short SET_BAD_QUALITY_RETRY_PERIOD_SECS = 10;
	
	void AnalogueDataPointConfig::printInfo()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Analogue DP Info - Engg (%f,%f), Raw (%f, %f), Precision = %d, EnggConstant = %f."
			,  m_engLowLimit, m_engHighLimit, m_rawLowLimit, m_rawHighLimit, m_precision, m_engConstant);
	}

    ModbusPointAgentProxy::ModbusPointAgentProxy(unsigned long agentKey, ModbusPointManager* pointMgr)
        :
        m_toBeTerminated(false),
        m_isSynchronising(false),
		m_isPrevSynchronising(true),
		m_isAllBadQuality(false),
        m_observerId(0),
        m_agentInterface(NULL),
		m_agentKey(agentKey),
        m_agentName(""),
        m_pollPeriod(600),
        // m_retryPeriod(10),
		m_pointManager(pointMgr),
		m_getAllUpdate(false)
    {
        // set poll period if parameter is set between 100 to 10000 ms
        unsigned long pollPeriod = atol(TA_Base_Core::RunParams::getInstance().get("PollPeriodMs").c_str());

        if ( (pollPeriod >= 100) && (pollPeriod <= 10000) )
        {
            m_pollPeriod = pollPeriod;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Bulk poll period is set to [%u] ms for agent [%u]", m_pollPeriod, m_agentKey);
        }

        // set retry period if parameter is set between m_pollPeriod to 600 seconds
        // unsigned long retryPeriod = atol(TA_Base_Core::RunParams::getInstance().get("RetryPeriodSecond").c_str());

        // if ( (retryPeriod * 1000 >= m_pollPeriod) && (retryPeriod <= 1800) )
        // {
            // m_retryPeriod = retryPeriod;
            // LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retry period is set to [%u] seconds for agent [%u]", m_retryPeriod, m_agentKey);
        // }
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "AgentName %u :: pollPeriod = %u.", m_agentKey, m_pollPeriod);

		m_dataPoints.clear();
		m_ADPs.clear();
		prepareAnalogueConfigEnggToRaw();
		start();
    };
    
    ModbusPointAgentProxy::~ModbusPointAgentProxy()
    {
        terminateAndWait();

        if (m_agentInterface != NULL)
        {
            delete m_agentInterface;
			m_agentInterface = NULL;
        }
		
		std::map<unsigned long, AnalogueDataPointConfig*>::iterator it = m_ADPs.begin();
		while (m_ADPs.end() != it)
		{
			if (it->second != NULL)
			{	delete it->second;
				it->second = NULL;
			}
			++it;
		}

		m_ADPs.clear();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Done m_ADPs.clear()");

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ModbusPointAgentProxy [%s] destroyed", m_agentName.c_str());
    }
    
    void ModbusPointAgentProxy::terminate()
    {
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In ModbusPointAgentProxy::terminate().");
		m_toBeTerminated = true;
		m_conditionLoop.signal();
    }

    void ModbusPointAgentProxy::startSynchronise()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "startSynchronise() for m_agentKey %u", m_agentKey);

        if ( !m_isSynchronising )
        {
			m_isPrevSynchronising = false;
            m_isSynchronising = true;
        }
    }
    
    void ModbusPointAgentProxy::stopSynchronise()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopSynchronise() for m_agentKey %u", m_agentKey);

        if ( m_isSynchronising )
        {
			m_isPrevSynchronising= true;
	        m_isSynchronising = false;
        }
    }
    
    bool ModbusPointAgentProxy::isSynchronised()
    {
        return m_isSynchronising;
    }
    
	void ModbusPointAgentProxy::run()
	{
		try
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "ModbusPointAgentProxy::run() starting up...");
			TA_Base_Core::CorbaName corbaName = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(m_agentKey, true);
			m_agentName = corbaName.getObjectName();
			m_agentInterface = new DataPointAgentInterface();
			m_agentInterface->setCorbaName(m_agentName, m_agentName + TA_Base_Bus::DP_BULK_POLLED_INTERFACE);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "ModbusPointAgentProxy get corba name for Agent:%u failed ", m_agentKey);
		}

		if (NULL == m_agentInterface) 
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DPAgentInterface:[%s] is null, returning.", m_agentName.c_str());
			return;
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DPAgentInterface:[%s] run, datapoints size:%d", m_agentName.c_str(), m_dataPoints.size());

		// bool logskipstep = true;
		// unsigned long maxSkipSteps = BULK_POLL_COMMS_ERROR_RETRY_PERIOD_SECS * 1000 / m_pollPeriod;
		// unsigned long skipSteps = 0; //to skip some loops when can't connect to agent
		// unsigned long lossCount = 0;

		while( !m_toBeTerminated )
		{
			try
			{
				if (m_isSynchronising)
				{
					if (false == m_isPrevSynchronising)
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "m_isSynchronising %d, m_isPrevSynchronising %d,m_dataPoints.size() %d", 
								m_isSynchronising , m_isPrevSynchronising );
						m_isPrevSynchronising = true;
					}
					// if ((skipSteps % maxSkipSteps) == 0)
					{                                    
						TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* dataPointUpdates = NULL;

						if (0 == m_observerId)
						{
							CORBA_CALL_RETURN( m_observerId, (*m_agentInterface), regiesterDisplayDataPointClient, () );

							if (m_observerId > 0 && !m_toBeTerminated)
							{
								LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "first time, get all update from agent, m_observerId = %u", m_observerId );
								CORBA_CALL_RETURN( dataPointUpdates, (*m_agentInterface), getDisplayDataPointUpdate, ( m_observerId, true ) );
							}
							else
							{
								LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to register: m_observerId %u, m_toBeTerminated %d",
										m_observerId , m_toBeTerminated );
								//m_isAllBadQuality = true;
								//setAllToBadQuality();// check for testcase
								continue;
							}
						}
						else
						{
							CORBA_CALL_RETURN( dataPointUpdates, (*m_agentInterface), getDisplayDataPointUpdate, ( m_observerId, false ) );
							LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
								"Update from agent: m_observerId = %u, m_agentName %s", m_observerId, m_agentName.c_str() );

						}

						// skipSteps = 0;
						// lossCount = 0;
						processUpdates(dataPointUpdates);
						//m_isAllBadQuality = false;
					}
					// else
					// {
						// if (logskipstep)
						// {
							// logskipstep = false;
							// LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "dp skip [%u] agent [%s], synch:1, loss:%lu", 
								// skipSteps, m_agentName.c_str(), lossCount);
						// }
						// ++skipSteps;
					// }
				}
				else
				{
					if (m_isPrevSynchronising)
					{
						//setAllToBadQuality();
						m_isPrevSynchronising = false;

						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "agent [%s], synch:0", 
							m_agentName.c_str());
						// skipSteps = 0;
					}
				}
			}
			catch ( const TA_Base_Core::ObjectResolutionException & ex )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
				//setAllToBadQuality();
				// only skip poll loop when the agent can't be resolved
				// if(lossCount++ < 20)
				// {
					// LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Resetting skipSteps to 0 as lossCount < 20");
					// skipSteps = 0;
				// }
				// else
				// {
					// logskipstep = true;
					// ++skipSteps;
				// }
			}
			catch ( const TA_Base_Core::ScadaProxyException & ex )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException" ,ex.what() );
				//setAllToBadQuality();
			}
			catch ( const TA_Base_Core::BadParameterException & )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", "Caught Bad Parameter exception" );
				//setAllToBadQuality();                
				m_observerId = 0;
			}
			catch ( const TA_Base_Core::OperationModeException & )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
				// force to rebind named object next time.
				(*m_agentInterface).clear();
				m_observerId = 0;
				//setAllToBadQuality();
			}
			catch ( const CORBA::Exception & ex )
			{
				LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
				// force to rebind named object next time.
				(*m_agentInterface).clear();
				m_observerId = 0;
				//setAllToBadQuality();
			}
			catch ( ... )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception...");
				//setAllToBadQuality();
			}

			m_conditionLoop.timedWait(m_pollPeriod);
		}

		prepareAgentThreadEnd();
	}

	void ModbusPointAgentProxy::addModbusPoint(IModbusServerPoint* pPoint, unsigned short size)
	{
		unsigned long ekey = pPoint->getEntitykey();
		DataPointSizePortInfo item(size, pPoint->getPort());
		m_dataPoints.insert( std::pair<unsigned long, DataPointSizePortInfo> (ekey, item) );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Added ModbusPoint (%u, %d, %d)", ekey , m_dataPoints[ekey].m_port, m_dataPoints[ekey].m_size );
	}

	void ModbusPointAgentProxy::processUpdates(TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* dpUpdates)
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
		//LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "updateSize = %d, m_dataPoints.size = %d" , updateSize, m_dataPoints.size());

		if (NULL != dpUpdates && !m_toBeTerminated && updateSize > 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Observer ID %u received %u data point updates from agent %s",
					m_observerId, updateSize, m_agentName.c_str());
			try
			{
				DataPointUpdateVector* pointUpdates = new DataPointUpdateVector(updateSize);
				pointUpdates->clear();
				//pointUpdates->resize(updateSize); maybe some dp no need.

				for (unsigned int i = 0; i < updateSize && !m_toBeTerminated; ++i)
				{
					TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState&  dataPointUpdate = (*dpUpdates)[i];

					//LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ModbusPointAgentProxy %s - received an update for entity %u",
					//	m_agentName.c_str(), dataPointUpdate.entityKey);

					std::map<unsigned long, DataPointSizePortInfo>::const_iterator it = m_dataPoints.find(dataPointUpdate.entityKey);
					if (m_dataPoints.end() != it)
					{
						DataPointUpdateData data;
						DataPointSizePortInfo dataInfo = it->second;
						data.m_pKey = it->first;
						convertDpValue(dataPointUpdate, data, dataInfo.m_size);
						data.m_port = dataInfo.m_port;
						LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Updating data.m_pKey %u, data.m_value %u, data.m_port %d, size %u", 
							data.m_pKey , data.m_value, data.m_port, dataInfo.m_size);
						pointUpdates->push_back(data);
					}
				}
				
				m_pointManager->submitUpdateData(pointUpdates);
				//m_pointManager->processData(pointUpdates);
				//delete pointUpdates;

				if (!m_getAllUpdate)
				{
					m_getAllUpdate = true;
				}
				m_pointManager->updateDataInitialized();
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

	void ModbusPointAgentProxy::convertDpValue(const TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState& oriValue, DataPointUpdateData& retValue, unsigned short txt_size)
	{
		
		switch(oriValue.value._d())
		{
		case TA_Base_Bus::ScadaCorbaTypes::AnalogueTagType :
			{
				double  origValue = oriValue.value.floatValue();
				std::map<unsigned long, AnalogueDataPointConfig*>::const_iterator it =  m_ADPs.find(retValue.m_pKey);//<EK, EnggRawInfo>
				if(it != m_ADPs.end())
				{
					double rawTemp =( ( origValue - it->second->m_engLowLimit) / it->second->m_engConstant) +
											it->second->m_rawLowLimit;

					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Updating Analogue DP of EK %u, RAW=%f, Engg=%f", it->first, rawTemp, origValue);
					origValue = rawTemp;
				}
				//EPV param 'ConversionEnabled=0' i.e. We get RawValue so no need to convert to EnggValue.
				retValue.m_value = (unsigned long)origValue;

				break;
			}
		case TA_Base_Bus::ScadaCorbaTypes::BooleanTagType :
			{
				retValue.m_value = 0;
				if (oriValue.value.booleanValue())
				{
					retValue.m_value = 1;
				};
				break;
			}
		case TA_Base_Bus::ScadaCorbaTypes::EnumTagType :
			{
				retValue.m_value = (unsigned long)oriValue.value.enumValue();
				break;
			}
		case TA_Base_Bus::ScadaCorbaTypes::TextTagType :
			{
				TA_Base_Bus::ScadaCorbaTypes::ByteSequence bsNewValue = oriValue.value.stringValue();
				std::ostringstream valueStr;
				bool flagBadString = false;
				for ( unsigned int i = 0; i < bsNewValue.length(); ++i )
				{
					//Only Decimal number acceptable
					if((char)bsNewValue[i] >='0' && (char)bsNewValue[i] <= '9')
					{
					}
					else
					{
						flagBadString=true;
					}
					valueStr << (char) bsNewValue[i];
				}
				unsigned int valueNumeric = 0;
				if((valueStr.str().empty()) || (valueStr.str().c_str() == NULL) || (valueStr.str().c_str() == ""))
				{
					retValue.m_value = -1;
				}
				else if (txt_size == 16)
				{
					//Train_Service_Number or Train_Direction				
					//If string is NULL, then set to -1.
					if( flagBadString == true )
					{
						retValue.m_value = -1;
					}
					else
					{
						retValue.m_value = atoi(valueStr.str().c_str());
					}
				}
				else if (txt_size == 32)
				{
					//the train server number is also 32bit in GZL9
					if (false == flagBadString)
					{
						retValue.m_value = atoi(valueStr.str().c_str());
					}
					else
					{
						//Scheduled_Arrival_Time
						retValue.m_value = convertTimeToEpochValue(valueStr.str().c_str());
					}
				}
				else
				{
					// Don't know 
				}

				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Updating Text data valueSTR = %s, valueNumeric %u, bsNewValue.sz = %u, txt_sz=%d", 
					valueStr.str().c_str(), retValue.m_value, bsNewValue.length(), txt_size);

				break;
			}
		default:
			TA_ASSERT(false, "unknown TA_Base_Bus::ScadaCorbaTypes::ETagType value");
		}
	}

	void ModbusPointAgentProxy::prepareAnalogueConfigEnggToRaw()
	{
		std::vector<TA_Base_Core::IModbusServerAnaloguePoint*>* points = m_pointManager->getAnaloguePoints();
		TA_Base_Core::IModbusServerAnaloguePoint* pPoint;

		//TA_Base_Core::ModbusServerPointAccessFactory::getAllAnalogueData(points);
		unsigned int i = 0;
		unsigned int nSize = points->size();
		if(nSize < 1)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Retrieved size ModbusServerAnaloguePoint is unexpected value %d, returning..", nSize);
			return;
		}
		for (i = 0; i < nSize; ++i)
		{
			pPoint = points->at(i);
			unsigned long EK = pPoint->getKey();//EK
			AnalogueDataPointConfig* childItem = new AnalogueDataPointConfig(pPoint->getEnggConstant(),pPoint->getRawLimitHigh(),pPoint->getRawLimitLow(),
											pPoint->getEngineeringLimitHigh(),pPoint->getEngineeringLimitLow(),pPoint->getPrecision());

			m_ADPs.insert( std::map<unsigned long, AnalogueDataPointConfig*>::value_type(EK, childItem) );
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Creating ModbusServerAnaloguePoint map, Total elements = %d", m_ADPs.size());
		
	}

	//Valid Input value = 14:33:30
	//Valid Input value = 2016-05-16 14:33:30.128
	//Output Value = Epoch value since (Today's Date + Input Value) 
	unsigned int ModbusPointAgentProxy::convertTimeToEpochValue(std::string timeStr)
	{
		int yy,mm,dd, hr, min, ss, ms;
		unsigned int valueNumeric;		
	
		if (0 == timeStr.length())
		{
			return -1;
		}
		
		if (10 < timeStr.length())
		{
			struct tm t1;
			sscanf(timeStr.c_str(), "%4d-%2d-%2d %2d:%2d:%2d.3d", &yy, &mm, &dd, &hr, &min, &ss, &ms);
			
			if( mm > 12 || dd > 31 || hr > 23 || min > 59 || ss > 59 )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Invalid Datetime, setting to -1");
				return  -1;
			}			
			t1.tm_year = yy - 1900;
			t1.tm_mon  = mm - 1;
			t1.tm_mday    = dd;
			
			t1.tm_hour = hr;
			t1.tm_min  = min;
			t1.tm_sec  = ss;
			valueNumeric = mktime(&t1);			
		}
		else
		{
			tm *ltm;
			time_t now = time(0);
			ltm = localtime(&now);
			unsigned int valueNumericNOW ;
			sscanf(timeStr.c_str(), "%2d:%2d:%2d", &hr, &min, &ss);
			if(hr>23||min>59||ss>59)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Invalid Datetime, setting to -1");
				return  -1;
			}
			
			//Get current time to find current date then add time component from value.
			valueNumericNOW = mktime(ltm);
			ltm->tm_hour = hr;
			ltm->tm_min  = min;
			ltm->tm_sec  = ss;
			valueNumeric = mktime(ltm);
			//Corner case check
			if(hr == 23 && min>55)
			{
				if((valueNumericNOW - valueNumeric) > 3600)
				{
					//Something wrong, may be slow, delayed traffic in the network connectivity
					LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,"Time value may not be accurate, due to slow connectivity or date change: Current time=%u , Calculated time=%u", valueNumericNOW, valueNumeric);
					//Put Yesterday's day + Input time
					valueNumeric = valueNumeric - (24*60*60);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,"New Calculated time=%u", valueNumeric);

				}
			}
		}
		
		return valueNumeric;
	}

	void ModbusPointAgentProxy::prepareAgentThreadEnd()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Stopping ModbusPointAgentProxy thread for Agent [%s]", m_agentName.c_str());

		if (m_observerId > 0) 
		{
			try
			{
				CORBA_CALL( (*m_agentInterface), unregiesterDisplayDataPointClient, ( m_observerId ) );
			}
			catch (...) 
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "ModbusPointAgentProxy::run()","Unknown Exception caught during call unregiesterDisplayDataPointClient()");
			}

		}	
	}
}
