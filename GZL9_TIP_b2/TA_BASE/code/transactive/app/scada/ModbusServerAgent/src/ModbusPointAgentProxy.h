/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/ModbusServerAgent/src/ModbusPointAgentProxy.h $
  * @author:  Ripple
  * @version: $Revision: #9 $
  *
  * Last modification: $DateTime: 2017/03/15 10:33:41 $
  * Last modified by:  $Author: dhanshri $
  *
  */
// ModbusPointAgentProxy.h: interface for the ModbusPointAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MODBUS_POINT_AGENT_PROXY_H
#define MODBUS_POINT_AGENT_PROXY_H

#include <string>
#include <sys/timeb.h>

#include "bus/scada/common_library/IDL/src/IDataPointAgentCorbaDef.h"
#include "core/synchronisation/src/Condition.h"
#include "core/naming/src/NamedObject.h"
#include "core/threads/src/Thread.h"
#include "core/threads/src/LFThreadPoolManager.h"
#include "core/data_access_interface/src/IModbusServerPoint.h"
#include "bus/modbus_comms/src/ModbusService.h"

using namespace TA_Base_Bus;
namespace TA_Base_App
{
	class ModbusPointManager;

	typedef TA_Base_Core::NamedObject< TA_Base_Bus::IDataPointAgentCorbaDef,
		TA_Base_Bus::IDataPointAgentCorbaDef_ptr,
		TA_Base_Bus::IDataPointAgentCorbaDef_var >   DataPointAgentInterface;

	class DataPointUpdateData
	{
	public:
		unsigned long	m_pKey;//entitykey
		unsigned int	m_value;
		double m_Dblvalue;
		unsigned short	m_port;
		//bool	m_bGoodValue;
	};
	
	class DataPointSizePortInfo
	{
	public:
		DataPointSizePortInfo():m_size(0),m_port(0){}
		DataPointSizePortInfo(unsigned short sz, unsigned short port):m_size(sz), m_port(port){ }

		unsigned short m_size;		//must be one of (1, 16, 32). other values will be invalid.
		unsigned short	m_port;
		//Below member especially to handle text DP related to SIG agent, it contains the prefix.
		//It is required to send Hex bytes to downstream
		//E.g. If actual address is Scheduled_Arrival_Time:ST02_AITPI01, this value = Scheduled_Arrival_Time
		//E.g. Train_Service_Number:T1405-11, then value = Train_Service_Number
		//E.g. Train_Direction:T1405-11, then value = Train_Direction
		//std::string m_entity_address;
	};

	class AnalogueDataPointConfig
	{
	public:
		AnalogueDataPointConfig(){
		//m_pKey = 0;
		m_engConstant = 0;
		m_rawHighLimit = 0;
		m_rawLowLimit = 0;
		m_engHighLimit = 0;
		m_engLowLimit = 0;
		m_precision = 0;
		}

		AnalogueDataPointConfig(double			engConstant,
		double			rawHighLimit,		double			rawLowLimit,		
		double			engHighLimit,		double			engLowLimit,		int				precision)
		:m_engConstant(engConstant), m_rawHighLimit(rawHighLimit),m_rawLowLimit(rawLowLimit),
		m_engHighLimit(engHighLimit),m_engLowLimit(engLowLimit),m_precision(precision)
		{
			//Aligned with DpValue::DEFAULT_PRECISION 
			if(m_precision < 0)
			{
				m_precision = 8;
			}
		}

		void printInfo();

		//unsigned long	m_pKey;
		double			m_engConstant;
		double			m_rawHighLimit;
		double			m_rawLowLimit;
		double			m_engHighLimit;
		double			m_engLowLimit;
		int				m_precision;
		
	};


	typedef std::vector<DataPointUpdateData> DataPointUpdateVector;

    class ModbusPointAgentProxy : public TA_Base_Core::Thread
    {
    public:
	    ModbusPointAgentProxy(unsigned long agentKey, ModbusPointManager* pointMgr);

	    virtual ~ModbusPointAgentProxy();

        virtual void run();
        
        virtual void terminate();
        
        void startSynchronise();
        void stopSynchronise();

        bool isSynchronised();

		void processUpdates(TA_Base_Bus::DataPointCorbaTypes::DisplayStateSequence* dpUpdates);

		void addModbusPoint(IModbusServerPoint* pPoint, unsigned short size);
		void insertFromPoolModbusPoint(IModbusServerPoint* pPoint, unsigned short size);
		void convertDpValue(const TA_Base_Bus::DataPointCorbaTypes::DataPointDisplayState& oriValue, DataPointUpdateData& retValue, unsigned short txt_size = 0);
		void prepareAnalogueConfigEnggToRaw();
		unsigned int convertTimeToEpochValue(std::string timeStr);
		bool checkIfFullUpdated() {return m_getAllUpdate;}

	private:
		void prepareAgentThreadEnd();
        volatile bool	m_toBeTerminated;
        bool	m_isSynchronising;
		bool	m_isPrevSynchronising;
		bool	m_isAllBadQuality;
        unsigned long	m_observerId;
        DataPointAgentInterface *	m_agentInterface;
		TA_Base_Core::ReEntrantThreadLockable		m_attributeMapLock;
		
		unsigned long   m_agentKey;
        std::string		m_agentName;

        unsigned long m_pollPeriod; // in millisecond

        unsigned long m_retryPeriod; // in second

        bool m_connected;

        TA_Base_Core::Condition m_conditionLoop;
		std::map<unsigned long, DataPointSizePortInfo>	m_dataPoints; //<EK, port>
		std::map<unsigned long, AnalogueDataPointConfig*> m_ADPs;//<EK, EnggRawInfo>
		ModbusPointManager* m_pointManager;
		volatile bool 		m_getAllUpdate;
		TA_Base_Core::LFThreadPoolManager*			m_ThreadPoolManager;

	};

	class AddModbusPtWorkItem : public TA_Base_Core::IThreadPoolExecuteItem
	{
	public:
		AddModbusPtWorkItem(ModbusPointAgentProxy* agent, IModbusServerPoint* addItem, unsigned short sz);
		void executeWorkItem();
	private:
		ModbusPointAgentProxy*		m_agentProxy;
		IModbusServerPoint*			m_addItem;
		unsigned short				m_size;
	};	
}
#endif
