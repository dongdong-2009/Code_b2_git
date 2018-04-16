/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/ModbusServerAgent/src/ModbusPointManager.cpp $
  * @author:	Darren Sampson
  * @version:	$Revision: #19 $
  *
  * Last modification:	$DateTime: 2017/07/07 13:35:34 $
  * Last modified by:	$Author: dhanshri $
  *
  * Description: The class provides a two connections,  one where it fetches updates from Client agents and second where
  *	it responds to the requests from client connections with appropriate data format (converted from Client agent updates).
  * First connection is via m_agentProxies and second via m_ModbusServices.
  */

#include "ModbusPointManager.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/src/ModbusServerPointAccessFactory.h"
#include "bus/modbus_comms/src/ModbusService.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	ModbusPointManager::ModbusPointManager(TA_Base_Core::ModbusServerAgentEntityDataPtr entityData)
		:
		m_bFirstUpdateAllFinished(false),
		m_ThreadPoolManager(NULL)
	{
		m_agentProxies.clear();
		m_Serviceports.clear();
		std::string port = entityData->getServicePort();
		// In future, may need to parse startword and length if not all modbus tables have same configuration.
		std::string startWordList = entityData->getServiceStartWord();
		std::string lengthList = entityData->getServiceWordLength();
	
		// set peersocketTimeout to 2s if parameter is not set between 1000 to 10000 ms
        m_peersocketTimeout = atol(TA_Base_Core::RunParams::getInstance().get("PeerSocketTimeout").c_str());

        if ( (m_peersocketTimeout <= 100) || (m_peersocketTimeout >= 20000) )
        {
            m_peersocketTimeout = 2000;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Ignoring the input value. Setting PeerSocketTimeout to default [%u] ms for all listening ports.", m_peersocketTimeout);
        }
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting PeerSocketTimeout to [%u] ms for all listening ports.", m_peersocketTimeout);
		}
		m_ModbusServices.clear();
		parseAllServicePorts(port);
		parseEPVList(startWordList, 0);
		parseEPVList(lengthList, 1);
		m_points.clear();
		m_ThreadPoolManager = new TA_Base_Core::LFThreadPoolManager();
	}

	ModbusPointManager::~ModbusPointManager()
	{
		
		std::map<unsigned long, ModbusPointAgentProxy*>::iterator it = m_agentProxies.begin();
		while (m_agentProxies.end() != it)
		{
			if( it->second != NULL)
			{
				(it->second)->terminate();
				delete it->second;
			}
			it->second = NULL;
			++it;
		}

		m_agentProxies.clear();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Done m_agentProxies.clear()");

		std::vector<TA_Base_Core::IModbusServerAnaloguePoint*>::iterator itAnaloguePoint = m_AnaloguePoints.begin();
		while (m_AnaloguePoints.end() != itAnaloguePoint)
		{
			delete *itAnaloguePoint;
			*itAnaloguePoint = NULL;
			++itAnaloguePoint;
		}
		m_AnaloguePoints.clear();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Done m_AnaloguePoints.clear()");

		std::map<unsigned short, TA_Base_Bus::ModbusService*>::iterator it2 = m_ModbusServices.begin();
		//Sleep for 0.5 sec so as to close the socket properly in each modbusservice thread.
		//Generally 100ms to close modbusservice thread completely after terminate() invoked.
		//sleep(100);

		while (m_ModbusServices.end() != it2)
		{
			it2->second->terminateAndWait();
			delete it2->second;
			it2->second = NULL;
			++it2;
		}

		m_ModbusServices.clear();

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Done m_ModbusServices.clear()");
		
		m_Serviceports.clear();
		
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Done m_Serviceports.clear().");
		
		std::map<unsigned short, EntityToModbusPointsMap*>::iterator it3 = m_points.begin();
	
		it3 = m_points.begin();
		while (m_points.end() != it3)
		{
			EntityToModbusPointsMap* itChild = it3->second;
			if (NULL != itChild)
			{
				it3->second = NULL;
				if(itChild->size() > 0)
				{
					
					EntityToModbusPointsMap::iterator itChildData = itChild->begin();
					while (itChild->end() != itChildData)
					{
						delete itChildData->second;
						++itChildData;
					}
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Done EntityToModbusPointsMap item in m_points");
				}
				delete itChild;
			}
			++it;
		}
		m_points.clear();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Done m_points.clear()");
		delete m_ThreadPoolManager;

	}

	// Description - Returns the num of valid ports parsed successfully from DB entry
	int ModbusPointManager::parseAllServicePorts(std::string portList)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Current portList %s", portList.c_str());
		int pos = 0, index = 0;
		int value = 0;
		std::string valueSTR = "";
		m_Serviceports.clear();
		ModbusTableStartLength dataMB;
		while("" != portList)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Current portList %s", portList.c_str());
			pos = portList.find ( ",", 0 );
			if ( 0 < pos )
			{
				valueSTR = (portList.substr( 0, pos )).c_str();
				value = atoi(valueSTR.c_str());
				if( value > 0)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Port added in List %s", valueSTR.c_str());
					m_Serviceports.insert( std::pair<std::string, ModbusTableStartLength> (valueSTR, dataMB) );
					index++;
				}
				else 
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Value configured is not greater than 0, portList %s", portList.c_str());
				}
				portList = (portList.substr ( pos + 1, portList.length() )).c_str();
			}
			else if ( portList != "")
			{
				valueSTR = portList.c_str();
				value = atoi(valueSTR.c_str());
				if( value > 0)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Port added in List %s", valueSTR.c_str());
					m_Serviceports.insert( std::pair<std::string, ModbusTableStartLength> (valueSTR, dataMB) );
					index++;
					break;
				}
				else 
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Value configured is not greater than 0, portList %s", portList.c_str());
				}
				portList = "";
			}
		} //while end

		return index;
	}

	void ModbusPointManager::parseEPVList(std::string strList, int paramNumber)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Current List %s", strList.c_str());
		int pos = 0;
		int value = 0;
		std::string valueSTR = "";
		ModbusTableStartLength dataMB;
		std::map<std::string, ModbusTableStartLength>::iterator itPorts = m_Serviceports.begin();

		while("" != strList )
		{
			if(itPorts == m_Serviceports.end())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Check Database configuration for parameter with value %s", strList.c_str());			
				return ;
			}

			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Current strList %s", strList.c_str());
			pos = strList.find ( ",", 0 );
			if ( 0 < pos )
			{
				valueSTR = (strList.substr( 0, pos )).c_str();
				value = atoi(valueSTR.c_str());
				if( value >= 0)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Member added in strList %s", valueSTR.c_str());
					if(paramNumber == 0)
					{
						itPorts->second.m_startWord = value;
					}
					else
					{
						itPorts->second.m_length= value;
					}
				}
				else 
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Value configured is less than 0 so default = 0, strList %s", strList.c_str());
				}
				itPorts++;
				strList = (strList.substr ( pos + 1, strList.length() )).c_str();
			}
			else if ( strList != "")
			{
				valueSTR = strList.c_str();
				value = atoi(valueSTR.c_str());
				if( value >= 0)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Member added in List %s", valueSTR.c_str());
					if(paramNumber == 0)
					{
						itPorts->second.m_startWord = value;
					}
					else
					{
						itPorts->second.m_length= value;
					}
					itPorts++;
					break;
				}
				else 
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Value configured is less than 0 so default = 0, strList %s", strList.c_str());
					itPorts++;
				}
				strList = "";
			}
		} //while end

		return ;
	
	}

	void ModbusPointManager::createAgentProxy()
	{
		std::vector<unsigned long> vAgentKeys;
		TA_Base_Core::ModbusServerPointAccessFactory::getAllSubscribedAgents(vAgentKeys);
		unsigned int i = 0;
		unsigned int nSize = vAgentKeys.size();
		if(nSize < 1)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "AgentKeys.size is unexpected value %d, returning..", nSize);
			return;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Creating ModbusPointAgentProxy list, size= %d", nSize);		
		}

		m_AnaloguePoints.clear();
		TA_Base_Core::ModbusServerPointAccessFactory::getAllAnalogueData(m_AnaloguePoints);
		
		unsigned long agentKey = 0;
		for (i = 0; i < nSize; ++i)
		{
			agentKey = vAgentKeys[i];
			//std::map<unsigned long, ModbusPointAgentProxy*>::const_iterator it = m_agentProxies.find(agentKey);
			//if (m_agentProxies.end() == it)
			{
				m_agentProxies[agentKey] = new ModbusPointAgentProxy(agentKey, this);
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Created ModbusPointAgentProxy for agentKey= %u", agentKey );
			}
			//m_agentProxies[agentKey]->addModbusPoint(pPoint);
		}
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Total ModbusPointAgentProxy %d items.", m_agentProxies.size());

		insertModbusPoint();
	}

	void ModbusPointManager::insertModbusPointInAgent(IModbusServerPoint* pPoint, unsigned short size, std::string entity_address)
	{
		unsigned long  agentKey = pPoint->getAgentKey();

		//Assuming no duplicate entry for pair (EntityKey, AgentKey)
		std::map<unsigned long, ModbusPointAgentProxy*>::iterator itModbusPointAgentProxy = m_agentProxies.find(agentKey);
		if( m_agentProxies.end() == itModbusPointAgentProxy )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "No entry in m_agentProxies for agent key = %u, m_agentProxies.size %d", agentKey,m_agentProxies.size());
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Matching item in m_agentProxies for agent key = %u, m_agentProxies.size = %d", agentKey, m_agentProxies.size());
			ModbusPointAgentProxy *obj = itModbusPointAgentProxy->second;
			obj->addModbusPoint(pPoint, size);
		}
	}		

	void ModbusPointManager::insertModbusPoint()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In ModbusPointManager::insertModbusPoint() for all points..." );
		//EntityToModbusPointsMap::const_iterator it = m_points.find(pPoint->getEntitykey());
		std::vector<TA_Base_Core::IModbusServerPoint*> points;
		TA_Base_Core::IModbusServerPoint* pPoint;

		TA_Base_Core::ModbusServerPointAccessFactory::getAllModbusServerPoint(points);
		unsigned int i = 0;
		unsigned int nSize = points.size();
		if(nSize < 1)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Retrieved size IModbusServerPoint is unexpected value %d, returning..", nSize);
			return;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Creating ModbusServerPoint map, points.size= %d", nSize);
		}

		std::map<unsigned short, EntityToModbusPointsMap*>::iterator it = m_points.begin();
		for (i = 0; i < nSize; ++i)
		{

			pPoint = points[i];

			//unsigned long  agentKey = pPoint->getAgentKey();
			unsigned short port = pPoint->getPort();
			unsigned long entitykey = pPoint->getEntitykey();

			InternalModbusPointData pInfo;

			//pInfo.m_pKey = entitykey;
			convertAddress(pPoint->getAddress(), pInfo.m_address);
			//pInfo.m_port = port;
			
			pInfo.m_entity_address = pPoint->getEntityAddress();
			
			unsigned int pos = 0;
			std::string valueSTR = "", value = pPoint->getLength().c_str();
			if ( value == "TEXT")
			{
				value = pInfo.m_entity_address.c_str();
				//E.g. Scheduled_Arrival_Time:ST02_AITPI01
				pos = value.find ( ":", 0 );
				if (pos != std::string::npos)
				{
					valueSTR = (value.substr( 0, pos )).c_str();
					if (valueSTR == "Train_Service_Number")
					{
						//GZL9 The train service number is like 1003202, need to change it to 32bit
						pInfo.m_size = 32;
					}
					else if (valueSTR == "Train_Direction")
					{
						pInfo.m_size = 16;
					}
					else if (valueSTR == "Train_window")
					{
						pInfo.m_size = 16;
					}					
					else if (valueSTR == "Scheduled_Arrival_Time")
					{
						pInfo.m_size = 32;
					}
					//Add GZL9 new setting.
					else if (valueSTR == "Platform_1st_Arr_Time")
					{
						pInfo.m_size = 32;
					}
					else if (valueSTR == "Stop_Flag")//Should not come here as Datatype = Digital
					{
						pInfo.m_size = 1;
					}
					else
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo," Unsupported Text datapoint %s for entityKey %u", valueSTR.c_str(), entitykey );
					}
					
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo," Unsupported Text datapoint %s for entityKey %u", value.c_str(), entitykey );
				}
			}
			else
			{
				pInfo.m_size = atol(pPoint->getLength().c_str());
			}

			insertModbusPointInAgent(pPoint, pInfo.m_size, pPoint->getEntityAddress());

			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
				"Checking entries in m_points for entityKey = %u , port = %d, "
				"pInfo.m_address = %s, pInfo.m_size = %d, pInfo.m_entity_address = %s."
				, entitykey, port, pPoint->getAddress().c_str(), pInfo.m_size , pInfo.m_entity_address.c_str());


			//Search for matching item <port, DP_List> in m_points
			it = m_points.find(port);
			std::pair<std::map<unsigned short, EntityToModbusPointsMap*>::iterator, bool> ret;	
			std::pair<std::map<unsigned long, InternalModbusPointData*>::iterator, bool> retChild;	
			//std::pair<iterator,bool> 
			//No matching list of entitykeys from Modbus table, listening on this port
			if (m_points.end() == it)
			{
				EntityToModbusPointsMap* pChildItem = new EntityToModbusPointsMap();
				InternalModbusPointData* objInternal = new InternalModbusPointData(pInfo);				
				//childItem[entitykey] = objInternal;
				retChild = pChildItem->insert(std::map<unsigned long, InternalModbusPointData*>::value_type (entitykey, objInternal));
				if (retChild.second == false) 
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"Element exists so not inserted.");
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"Element inserted!");
				}
				ret = m_points.insert( std::map<unsigned short, EntityToModbusPointsMap*>::value_type(port, pChildItem) );
				if (ret.second == false) 
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"Element exists so not inserted.");
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"Element inserted!");
				}
				//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Setting it->first %d, m_points.size() = %d, childItem.size = %d", port, m_points.size(), pChildItem->size());
			}
			//Matching Modbus table for this port, now search for matching entitykey 
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"Found matching parent list with port %d , now search for entitykey in this list...", port); 
				EntityToModbusPointsMap* pChildItem = it->second;
				std::map<unsigned long, InternalModbusPointData*>::iterator itChildEK = pChildItem->find(entitykey);
				//No matching entitykey, so insert new entry
				if ( itChildEK == pChildItem->end())
				{
					InternalModbusPointData* obj = new InternalModbusPointData(pInfo);
					retChild = pChildItem->insert(std::map<unsigned long, InternalModbusPointData*>::value_type(entitykey, obj));
					if (retChild.second == false) 
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"Element exists so not inserted.");
					}
					else
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"Element inserted!");
					}
				
					//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Setting DP_Map.EK %u, Map.size() = %d", entitykey, it->second->size());
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,"Found matching child entitykey %u with port %d.", entitykey, port); 
				}
			}//m_points.end() end-if
			
		} //DB points end-if
		
		//Printing below info for DEBUG

		it = m_points.begin();
		//unsigned int pts_size = m_points.size();
		//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "m_points.size() = %d", pts_size );
		//it = m_points.begin();
		while (it != m_points.end())
		{
			EntityToModbusPointsMap* objMap = it->second;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Size of m_points#port(%d) = %d",it->first, objMap->size());
			//pts_size--;
			it++;
		}
	}

	void ModbusPointManager::convertAddress(const std::string& strAddress, PointAddress& pointAddress)
	{
		//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In ModbusPointManager::convertAddress()");

		unsigned long registerAddress = 0;
		unsigned long bitAddress = 0;

		char * stopString = "\0";
		int base = 10;

		// check if the first 2 character of the address are "0x" or "0X"
		if ( ( "0x" == strAddress.substr ( 0, 2 ) ) ||
			 ( "0X" == strAddress.substr ( 0, 2 ) ) )
		{
			base = 16;
		}
		int pos = 0;
		pos = strAddress.find ( ".", pos );
		if ( 0 < pos )
		{
			registerAddress = strtoul ( (strAddress.substr( 0, pos )).c_str(), &stopString, base );
			bitAddress = strtol ( strAddress.substr ( pos + 1, strAddress.length() ).c_str(), &stopString, base );
		}
		else
		{
			registerAddress = strtoul ( (strAddress.substr( 0, pos )).c_str(), &stopString, base );
		}

		//pointAddress = 0;
		if (0 != registerAddress || 0 != bitAddress
			|| "0.0" == strAddress)
		{
			//pointAddress = new PointAddress();
			pointAddress.m_wordAddress = registerAddress;
			pointAddress.m_bitAddress = bitAddress;
		}
		else if ("" != strAddress)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "address[%s] can not convert to modbus address", strAddress.c_str());
		}
	}

	void ModbusPointManager::submitUpdateData(DataPointUpdateVector* updateData)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In ModbusPointManager::submitUpdateData()");
		{
			//TA_Base_Core::ThreadGuard guard( m_queueLock );
			//m_pointsUpdates.push(updateData);
		}
		ProcessUpdateWorkItem* updateItem = new ProcessUpdateWorkItem(this,updateData);
		m_ThreadPoolManager->queueWorkItem(updateItem);
		//m_queueSemaphore.post();
	}

	void ModbusPointManager::startSynchronise()
	{
		std::map<std::string, ModbusTableStartLength>::const_iterator itPorts = m_Serviceports.begin();
		
		while(itPorts != m_Serviceports.end())
		{
			std::string portNumber = itPorts->first;
			unsigned short portNumeric = atoi(portNumber.c_str());

			unsigned int startWord = itPorts->second.m_startWord;
			unsigned int length = itPorts->second.m_length;

			//starting service only when length >0 and startword >=0
			if (length >= 0 && startWord >= 0)
			{
				std::map<unsigned short, ModbusService*>::const_iterator it = m_ModbusServices.find(portNumeric);
				if (m_ModbusServices.end() == it)
				{
					m_ModbusServices[portNumeric] = new TA_Base_Bus::ModbusService( portNumber , startWord, length, m_peersocketTimeout);
				}
				else
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Duplicate port number %s for ModbusService object. Ignoring..", portNumber.c_str() );
				}
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Not starting ModbusService object for length %d, startWord %d. Ignoring..", length, startWord );
							
			}
			itPorts++;

		}

		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In ModbusPointManager::startSynchronise()");
		std::map<unsigned long, ModbusPointAgentProxy*>::const_iterator it = m_agentProxies.begin();
		while (m_agentProxies.end() != it)
		{
			it->second->startSynchronise();
			++it;
		}
		TA_Base_Core::Thread::sleep(22000);

		std::map<unsigned short, ModbusService*>::const_iterator itSrv = m_ModbusServices.begin();
		while (m_ModbusServices.end() != itSrv)
		{
			itSrv->second->start();
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "ModbusService for portNumber %d started", itSrv->first);
			++itSrv;
		}		

	}

	void ModbusPointManager::stopSynchronise()
	{
		//m_ModbusService->xxx(); close serivice.
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In ModbusPointManager::stopSynchronise()");
		std::map<unsigned long, ModbusPointAgentProxy*>::const_iterator it = m_agentProxies.begin();
		while (m_agentProxies.end() != it)
		{
			it->second->stopSynchronise();
			++it;
		}

		std::map<unsigned short, ModbusService*>::const_iterator itSrv = m_ModbusServices.begin();
		while (m_ModbusServices.end() != itSrv)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Terminating ModbusService for portNumber= %d", itSrv->first);
			itSrv->second->terminateAndWait();
			++itSrv;
		}
		m_ModbusServices.clear();
	}

	void ModbusPointManager::processData(DataPointUpdateVector* updateData)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In ModbusPointManager::processData()");
		std::map<unsigned short, TA_Base_Bus::PointValueVector> values;
		std::map<unsigned short, TA_Base_Bus::PointValueVector>::iterator itValues;

		TA_Base_Bus::PointValueStruct value;
		
		unsigned int i = 0;
		unsigned int nSize = updateData->size();
		std::map<unsigned short, EntityToModbusPointsMap*>::const_iterator it = m_points.begin();

		for (i = 0; i < nSize; ++i)
		{
			DataPointUpdateData& data = updateData->at(i);
			unsigned short portnumber = data.m_port;
			unsigned long ekey = data.m_pKey;

			//std::map<unsigned short, EntityToModbusPointsMap>::const_iterator
			it = m_points.find(portnumber);
		
			if (m_points.end() != it)
			{
				EntityToModbusPointsMap* pObjE = it->second;
				//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Port:it->first %d, Items for this port = %d", it->first, pObjE->size());

				EntityToModbusPointsMap::const_iterator itChild = pObjE->find(ekey);
				if (pObjE->end() == itChild )
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "No matching EK %u in this map item for port %d.", 
						ekey, portnumber);
				}
				else
				{
					InternalModbusPointData* modbusPoint = itChild->second;
					if (0 != modbusPoint->m_size || ((modbusPoint->m_address).m_wordAddress != 0))
					{
						value.m_address.m_bitAddress = modbusPoint->m_address.m_bitAddress;
						value.m_address.m_wordAddress = modbusPoint->m_address.m_wordAddress;
						value.m_value = data.m_value;
						value.m_Dblvalue = data.m_Dblvalue;
						//Set size to 4 bytes for every SIG Text string.
						value.m_size = modbusPoint->m_size;

						itValues = values.find(portnumber);
						if (values.end() == itValues)
						{
							TA_Base_Bus::PointValueVector pointValues;
							values.insert( std::map<unsigned short, TA_Base_Bus::PointValueVector>::value_type(portnumber, pointValues) );
							values[portnumber].reserve(nSize);
						}
						values[portnumber].push_back(value);
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Pushing in as Matching EK %u in this map item for port %d.", 
						ekey, portnumber);					}
					else
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "As size/address invalid  so not Pushing in even it is Matching EK %u in this map item for port %d.", 
							ekey, portnumber);
					
					}
				}
			}
			else
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "No matching map item for port %d, entitykey %u",
						portnumber, ekey);
			}
		}//for ends

		//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Now sending data to service buffers as per port, values buffer size %d.",values.size());

		std::map<std::string, ModbusTableStartLength>::const_iterator itPorts = m_Serviceports.begin();
		while( itPorts != m_Serviceports.end())
		{
			std::string portNumber = itPorts->first;
			unsigned short port = atoi(portNumber.c_str());
			if (m_ModbusServices[port] == NULL)
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Service for port = %d is unavailable", port);
			}
			else
			{
				m_ModbusServices[port]->setData(values[port]);
				//LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "No of items in service buffers as per port = %d.",values[port].size());
			}
			itPorts++;
		}

	}

	void ModbusPointManager::updateDataInitialized()
	{
		TA_Base_Core::ThreadGuard guard( m_checkFirstUpdateLock );
	
		if (!m_bFirstUpdateAllFinished)
		{
			bool bFirstUpdateAllFinished = true;
			std::map<unsigned long, ModbusPointAgentProxy*>::const_iterator itAgentProxy = m_agentProxies.begin();
			while (m_agentProxies.end() != itAgentProxy)
			{
				bFirstUpdateAllFinished = bFirstUpdateAllFinished && itAgentProxy->second->checkIfFullUpdated();
				++itAgentProxy;
			}

			if (bFirstUpdateAllFinished)
			{
				m_bFirstUpdateAllFinished = true;
				std::map<unsigned short, ModbusService*>::const_iterator itModbusService = m_ModbusServices.begin();
				while (m_ModbusServices.end() != itModbusService)
				{
					LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Set ModbusService Data Initialization done for portNumber= %d", itModbusService->first);
					itModbusService->second->setDataInitialized();
					++itModbusService;
				}		
			}	
		}
	}

	ProcessUpdateWorkItem::ProcessUpdateWorkItem(ModbusPointManager* manager, DataPointUpdateVector* updateData)
	:
	m_manager(manager),
	m_datapointUpdates(updateData)
	{
	}

	void ProcessUpdateWorkItem::executeWorkItem()
	{
		m_manager->processData(m_datapointUpdates);
		delete m_datapointUpdates;
	}	
}
