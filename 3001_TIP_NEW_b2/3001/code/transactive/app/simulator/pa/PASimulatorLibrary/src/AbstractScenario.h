#ifndef PA_SIM_ABSTRACT_SCENARIO_H
#define PA_SIM_ABSTRACT_SCENARIO_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h $
  * @author:  huang.jian
  * @version: $Revision: #6 $
  * 
  * Declaration file for AbstractScenario class
  */
#include <CTIME>
#include "app/simulator/pa/PASimulatorLibrary/src/PASimStandardModbusTcpTable.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimModbusFrames.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimStandardTCPModbusTableManager.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableQueryProcessor.h"
#include "core/utilities/src/TAAssert.h"

const int PAS_POLLING_ZONE_STATE_COUNTER_START = 7;
const int PAS_POLLING_ZONE_STATE_COUNTER_END = 7;
const int PAS_POLLING_FAULT_ZONE_COUNTER_START = 8;
const int PAS_POLLING_FAULT_ZONE_COUNTER_END = 8;
const int PAS_POLLING_ZONE_COUNTER_INTERVAL = 200;
const int PAS_POLLING_DVA_LIB_COUNTER_START = 10;
const int PAS_POLLING_DVA_LIB_COUNTER_END = 10;
const int PAS_POLLING_DVA_LIB_COUNTER_INTERVAL = 2000;
const int PAS_POLLING_FIREALARM_START = 642;
const int PAS_POLLING_FIREALARM_END = 642;
const int PAS_POLLING_FIREALARM_INTERVAL = 500;
const int PAS_POLLING_MIN_INTERVAL = 50;


const int PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT = 5000;
const int COUNT_OF_BITS_IN_BYTE = 8;
const int COUNT_OF_BITS_IN_WORD_ = 16;
const int COUNT_OF_BITS_IN_DWORD = 32;

const int PAS_ADDRESS_OCC_MATRIX_TABLE_START = 25000;
const int PAS_ADDRESS_OCC_MATRIX_TABLE_END = 25263;
const int PAS_ADDRESS_STN_MATRIX_TABLE_START = 24715;
const int PAS_ADDRESS_STN_MATRIX_TABLE_END = 24716;

const int PAS_ADDRESS_OCC_PA_STATUS_MATRIX_TABLE_START = 43788;
const int PAS_ADDRESS_OCC_PA_STATUS_MATRIX_TABLE_END = 48011;
const int PAS_ADDRESS_STN_PA_STATUS_MATRIX_TABLE_START = 27796;
const int PAS_ADDRESS_STN_PA_STATUS_MATRIX_TABLE_END = 27827;

const int PAS_ADDRESS_OCC_ROUTING_STATUS_MATRIX_TABLE_START = 456;
const int PAS_ADDRESS_OCC_ROUTING_STATUS_MATRIX_TABLE_END = 4679;
const int PAS_ADDRESS_STN_ROUTING_STATUS_MATRIX_TABLE_START = 27732;
const int PAS_ADDRESS_STN_ROUTING_STATUS_MATRIX_TABLE_END = 27763;

const int PAS_ADDRESS_PRIORITY_START = 24577;
const int PAS_ADDRESS_PRIORITY_END = 24577;
//const int PAS_ADDRESS_OCC_MESSAGE_ID_START = 24583;
//const int PAS_ADDRESS_OCC_MESSAGE_ID_END = 24583;
const int PAS_ADDRESS_OCC_MESSAGE_ID_START = 24900;
const int PAS_ADDRESS_OCC_MESSAGE_ID_END = 24999;
const int PAS_ADDRESS_EXECUTION_REGISTER_START = 9;
const int PAS_ADDRESS_EXECUTION_REGISTER_END = 9;
const int PAS_ADDRESS_OCC_COMMAND_REGISTER_START = 24575;
const int PAS_ADDRESS_OCC_COMMAND_REGISTER_END = 24575;
const int PAS_ADDRESS_STN_COMMAND_REGISTER_START = 24575;
const int PAS_ADDRESS_STN_COMMAND_REGISTER_END = 24575;
const int PAS_ADDRESS_COMMAND_RESULT_START = 24576;
const int PAS_ADDRESS_COMMAND_RESULT_END = 24576;
const int PAS_ADDRESS_OCC_ZONE_STATE_START = 456;
const int PAS_ADDRESS_OCC_ZONE_STATE_END = 4679;
const int PAS_ADDRESS_STN_ZONE_STATE_START = 27732;
const int PAS_ADDRESS_STN_ZONE_STATE_END = 27763;
const int PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_START = 24775;
const int PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_END = 24894;
const int PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_START = 27200;
const int PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_END = 27319;
const int PAS_ADDRESS_OCC_DVA_LIB_VERSION_START = 14;
const int PAS_ADDRESS_OCC_DVA_LIB_VERSION_END = 16;
const int PAS_ADDRESS_STN_DVA_LIB_VERSION_START = 14;
const int PAS_ADDRESS_STN_DVA_LIB_VERSION_END = 16;
const int PAS_ADDRESS_STN_PLAYBACK_STATUS = 257;
const int PAS_ADDRESS_OCC_TRANSACTION_ID = 24697;
const int PAS_ADDRESS_STN_TRANSACTION_ID = 24597;


//const int PAS_ADDRESS_STN_MESSAGE_ID_START = 24578;
//const int PAS_ADDRESS_STN_MESSAGE_ID_END = 24578;
const int PAS_ADDRESS_STN_MESSAGE_ID_START = 24900;
const int PAS_ADDRESS_STN_MESSAGE_ID_END = 24999;

const int PAS_ADDRESS_STN_SOURCE_ID = 24582;

const int PAS_EACH_STATION_WORD_COUNT = 4;
const int PAS_ADDRESS_LENGTH = 16;

const unsigned short PAS_DVA_MSG_LIB_END_ID = 0;
const unsigned short MASK_FIREALARM = 0x0001;

const unsigned short MASK_ZONE_BROADCAST_FAILED = 0xF000;
const unsigned short PAS_ZONE_STATE_FREE = 0;
const unsigned short PAS_ZONE_STATE_FAULT = 0x1000;
const unsigned short PAS_ZONE_STATE_OVERRIDDEN = 0x2000;
const unsigned short PAS_COMMAND_OCC_START_DVA_WITHOUT_CHIME = 0x0900;
const unsigned short PAS_COMMAND_STN_START_DVA_WITHOUT_CHIME = 0x0900;
const unsigned short PAS_COMMAND_OCC_START_DVA_WITH_CHIME = 0x2900;
const unsigned short PAS_COMMAND_STN_START_DVA_WITH_CHIME = 0x2900;
const unsigned short PAS_COMMAND_OCC_STOP_DVA = 0x0A00;
const unsigned short PAS_COMMAND_STN_STOP_DVA = 0x0A00;
const unsigned short PAS_COMMAND_DVA_LIB_NEW_DOWNLOAD = 0x1D00;
const unsigned short PAS_COMMAND_DVA_LIB_NEXT_DOWNLOAD = 0x1E00;
const unsigned short PAS_COMMAND_STN_START_LIVE_BROADCAST_WITH_CHIME_SOURCE_ID = 0x0402;
const unsigned short PAS_COMMAND_STN_START_LIVE_BROADCAST_WITHOUT_CHIME_SOURCE_ID = 0x0402;

const unsigned short MODBUS_WRITE_REGISTER_FUNCTION_CODE = 0x10;
const unsigned short MODBUS_READ_REGISTER_FUNCTION_CODE = 0x04;

const unsigned short PAS_COMMAND_OCC_DVA_RECORDING_START = 0x1900;
const unsigned short PAS_COMMAND_STN_DVA_RECORDING_START = 0x1900;
const unsigned short PAS_COMMAND_OCC_DVA_RECORDING_STOP = 0x1A00;
const unsigned short PAS_COMMAND_STN_DVA_RECORDING_STOP = 0x1A00;
const unsigned short PAS_COMMAND_OCC_DVA_RECORDING_CHANGE_TYPE = 0x1C00;
const unsigned short PAS_COMMAND_STN_DVA_RECORDING_CHANGE_TYPE = 0x1C00;
const unsigned short PAS_COMMAND_OCC_DVA_RECORDING_DELETE = 0x1B00;
const unsigned short PAS_COMMAND_STN_DVA_RECORDING_DELETE = 0x1B00;

const unsigned short PAS_STATUS_OCC_BACKGROUND_MUSIC = 12;
const unsigned short PAS_STATUS_STN_BACKGROUND_MUSIC = 0x0801;

#define MSB(NUM) ((NUM)/256)
#define LSB(NUM) ((NUM)%256)
#define IS_EXE_REG_BE_SET(p) (\
	PAS_ADDRESS_EXECUTION_REGISTER_START == p->m_startAddress \
	&& 1 == p->m_registersQuantity \
	&& 0x00 == p->m_registersValue[0] && 0x01 == p->m_registersValue[1] \
	)

#define IS_WRITE_COMMAND_BE_SENT(p) (\
    PAS_ADDRESS_OCC_COMMAND_REGISTER_START == p->m_startAddress \
    && 1 == p->m_registersQuantity \
    )

using namespace TA_IRS_App::PA_Sim;
namespace TA_IRS_App
{
class AbstractScenario
{
public:
    typedef void (AbstractScenario::* PMF)(TA_IRS_App::PA_Sim::ModbusQuery* );
    static AbstractScenario* startScenario;
	AbstractScenario(){}
    AbstractScenario(PASimStandardTcpModbusTablesManager* parent)
		:m_tcpModbusTablesManager(parent),
		m_isInCurrentScenario(false) 
	{ }
    virtual ~AbstractScenario(){}
    AbstractScenario& setSuccessor(AbstractScenario* p) { TA_ASSERT(NULL != m_successor,""); m_successor = p; return *p;}
    virtual bool isMySpecialScenario(TA_IRS_App::PA_Sim::ModbusQuery* query) = 0;
    virtual void reponseReadQuery(TA_IRS_App::PA_Sim::ModbusQuery* query) = 0; // must set isInCurrentScenario to false if the last step has been finished
    virtual void reponseWriteQuery(TA_IRS_App::PA_Sim::ModbusQuery* query) = 0; // must set isInCurrentScenario to false if the last step has been finished

    void processQuery(PMF pf, TA_IRS_App::PA_Sim::ModbusQuery* query)
    {
        if ( m_isInCurrentScenario ) // Alread in current scenario, process directly
        {
            (this->*pf)(query);
        }
        else if (AbstractScenario::startScenario == this) // no special scenario match the query
        {
            // do nothing
        }
        else if ( isMySpecialScenario(query) )
        {
            // do nothing
            (this->*pf)(query);
        }
        else
        {
            if ( NULL == AbstractScenario::startScenario)
            {
                AbstractScenario::startScenario = this;
            }
            m_tcpModbusTablesManager->setScenario(m_successor);
            m_successor->processQuery(pf, query); 
        }
        AbstractScenario::startScenario = NULL;
    }

    void deliveryUserQuery(MessageType type, int startAddress, int endAddress, ByteVector* data, int interval)
    {
        switch (type)
        {
        case Set:
            {
                UserQuery* q = new UserQuery(Set, m_tcpModbusTablesManager->getQueryProcessor()->getTableOutputFormat());
                q->m_startAddress = startAddress;
                q->m_endAddress = endAddress;
                size_t size = (*data).size();
                q->m_data.resize(size);
                std::copy((*data).begin(), 
                    (*data).end(),
                    q->m_data.begin());
                q->setType(Set);
                time_t t;
                time(&t);
                t+=interval;
                q->m_timeStamp = t;
                m_tcpModbusTablesManager->getQueryProcessor()->addUserQuery(q);
                break;
            }
		case Inc:
			{
				UserQuery* q = new UserQuery(Inc, m_tcpModbusTablesManager->getQueryProcessor()->getTableOutputFormat());
				q->m_startAddress = startAddress;
				q->m_endAddress = endAddress;
				q->setType(Inc);
				time_t t;
				time(&t);
				t+=interval;
				q->m_timeStamp = t;
				m_tcpModbusTablesManager->getQueryProcessor()->addUserQuery(q);
				break;
			}
        default:
            ;
        }
    }
    
	void setExecutionRegister2Zero()
    {
        ByteVector dataExe;
        dataExe.push_back(0x00);
        dataExe.push_back(0x00);
        deliveryUserQuery(Set, PAS_ADDRESS_EXECUTION_REGISTER_START, PAS_ADDRESS_EXECUTION_REGISTER_START, &dataExe, 1);
    }
    void setCmdAckReg2Zero(int val = 0)
    {
        if (0 == val)
        {
            ByteVector dataAck;
            dataAck.push_back(0x00);
            dataAck.push_back(0x00);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "set ack reg to zero");
            deliveryUserQuery(Set, PAS_ADDRESS_COMMAND_RESULT_START, PAS_ADDRESS_COMMAND_RESULT_END, &dataAck, 0);
        }
        else
        {
            ByteVector dataAck;
            dataAck.push_back(MSB(val));
            dataAck.push_back(LSB(val));
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "set ack reg to %d", val);
            deliveryUserQuery(Set, PAS_ADDRESS_COMMAND_RESULT_START, PAS_ADDRESS_COMMAND_RESULT_END, &dataAck, 0);
        }
    }

	int getMessageID()
	{
		if (OCC == m_tcpModbusTablesManager->getLocationType())
		{
			return m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_OCC_MESSAGE_ID_START);
		} 
		else
		{
			return m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_STN_MESSAGE_ID_START);
		}
	}
    PASimStandardTcpModbusTablesManager* getTablesManager()
    {
        return m_tcpModbusTablesManager;
    }
protected:
    PASimStandardTcpModbusTablesManager* m_tcpModbusTablesManager;
    AbstractScenario* m_successor;
    bool m_isInCurrentScenario;
private:
    AbstractScenario(const AbstractScenario&);
    AbstractScenario& operator=(const AbstractScenario&);
};
}
#endif //PA_SIM_ABSTRACT_SCENARIO_H