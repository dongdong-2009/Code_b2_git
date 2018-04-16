/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/ModbusServerAgent/src/ModbusPointManager.h $
  * @author:	Darren Sampson
  * @version:	$Revision: #13 $
  *
  * Last modification:	$DateTime: 2017/07/06 11:21:55 $
  * Last modified by:	$Author: dhanshri $
  *
  * Description:
  *
  *
  */

#ifndef MODBUS_POINT_MANAGER_H
#define MODBUS_POINT_MANAGER_H

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/LFThreadPoolManager.h"

#include "core/data_access_interface/src/ModbusServerPointAccessFactory.h"
#include "core/data_access_interface/src/IModbusServerPoint.h"
#include "core/data_access_interface/entity_access/src/ModbusServerAgentEntityData.h"
#include "bus/modbus_comms/src/ModbusService.h"
#include "core/data_access_interface/src/IModbusServerAnaloguePoint.h"

#include "ModbusPointAgentProxy.h"
#include <queue>

namespace TA_Base_App
{
	class ModbusPointData
	{
	public:
		unsigned long	m_pKey; //entitykey
		PointAddress*	m_address;
		unsigned long	m_size;		//must be one of (1, 16, 32). other values will be invalid.
		unsigned int	m_port;
		/*PointAddress*	m_qualityAddress;
		PointAddress*	m_subQualityAddress;
		std::string		m_subQualityStrAddress;*/
	};

	class InternalModbusPointData
	{
	public:

		InternalModbusPointData(){}
		InternalModbusPointData(unsigned long	sz,unsigned long wordAddr, unsigned long bitAddr ):m_size(sz)
		{
			m_address.m_wordAddress = wordAddr;
			m_address.m_bitAddress = bitAddr;
		}
		//unsigned long	m_pKey; //entitykey
		PointAddress	m_address;
		std::string		m_entity_address;
		short m_size;		//must be one of (1, 16, 32). other values will be invalid.
	};

	class SubQualityRecord
	{
	public:
		std::map<unsigned long, bool> m_points;
		PointAddress m_address;
		int	m_goodCount;
	};

	class ModbusTableStartLength
	{
	public:
		ModbusTableStartLength():m_startWord(0), m_length(0){}
		ModbusTableStartLength(unsigned int startWord, unsigned int len):m_startWord(startWord), m_length(len){}
		unsigned int m_startWord;
		unsigned int m_length;
	
	};

	typedef std::vector<InternalModbusPointData> ModbusPointDataVector;
	typedef std::map<unsigned long, InternalModbusPointData*> EntityToModbusPointsMap;

	class ModbusPointManager
	{
	public:
		ModbusPointManager(TA_Base_Core::ModbusServerAgentEntityDataPtr entityData);
		virtual ~ModbusPointManager();
		void startSynchronise();
        void stopSynchronise();

		void createAgentProxy();
		void submitUpdateData(DataPointUpdateVector* updateData);
		void updateDPsize(unsigned long entitykey, unsigned long size);
		int parseAllServicePorts(std::string portList);
		void parseEPVList(std::string startWordList, int paramNumber=0);
		void processData(DataPointUpdateVector* updateData);

		void updateDataInitialized();
		std::vector<TA_Base_Core::IModbusServerAnaloguePoint*>* getAnaloguePoints() {return &m_AnaloguePoints;};

	private:
		void insertModbusPoint();
		void insertModbusPointInAgent(IModbusServerPoint* pPoint, unsigned short size, std::string entity_address);
		void convertAddress(const std::string& strAddress, PointAddress& pointAddress);

		std::map<unsigned short, TA_Base_Bus::ModbusService*>	 m_ModbusServices;// Map <server_port, servObj>
		std::map<std::string, ModbusTableStartLength> m_Serviceports;
		std::map<unsigned short, EntityToModbusPointsMap*> m_points; //<port, DP>

		//agentkey to ModbusPointAgentProxy.
		std::map<unsigned long, ModbusPointAgentProxy*>	m_agentProxies;

		unsigned long m_peersocketTimeout; 
		TA_Base_Core::ReEntrantThreadLockable		m_checkFirstUpdateLock;
		bool 	m_bFirstUpdateAllFinished;
		std::vector<TA_Base_Core::IModbusServerAnaloguePoint*> m_AnaloguePoints;
		TA_Base_Core::LFThreadPoolManager*			m_ThreadPoolManager;
		
	};

	class ProcessUpdateWorkItem : public TA_Base_Core::IThreadPoolExecuteItem
	{
	public:
		ProcessUpdateWorkItem(ModbusPointManager* manager, DataPointUpdateVector* updateData);
		void executeWorkItem();
	private:
		ModbusPointManager*				m_manager;
		DataPointUpdateVector*			m_datapointUpdates;
	};	
}
#endif
