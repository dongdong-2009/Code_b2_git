#include "app/simulator/pa/PASimulatorLibrary/src/BroadcastDvaMessageScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/DVAMsgLibDownloadScenario.h"

bool TA_IRS_App::BroadcastDvaMessageScenario::isMySpecialScenario( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if (MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);

        if ( (PAS_ADDRESS_OCC_COMMAND_REGISTER_START == p->m_startAddress \
			|| PAS_ADDRESS_STN_COMMAND_REGISTER_START == p->m_startAddress) \
            && 1 == p->m_registersQuantity && 0x00 == p->m_registersValue[1] )
        {
            switch( p->m_registersValue[0] )
            {
            case 0x29: // Start DVA bro with chime
                m_operationType = Start;
                m_isInCurrentScenario = true;
                break;
            case 0x09: // Start DVA bro without chime
                m_operationType = Start;
                m_isInCurrentScenario = true;
                break;
            case 0x0A: // Stop DVA bro
                m_operationType = Stop;
                m_isInCurrentScenario = true;
                break;
            default:
                m_operationType = Undefined;
                m_isInCurrentScenario = false;
                break;
            }
        }
    }
    if (m_isInCurrentScenario)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Running BroadcastDvaMessageScenario ...");
    }
    return m_isInCurrentScenario;
}

void TA_IRS_App::BroadcastDvaMessageScenario::reponseReadQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if (MODBUS_READ_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusReadQuery* p = dynamic_cast<ModbusReadQuery*>(query);
        if ( PAS_ADDRESS_COMMAND_RESULT_START == p->m_startAddress && 1 == p->m_registersQuantity)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finishing BroadcastDvaMessageScenario...");

            size_t size = m_statusMatrix.size();
            size_t paZoneMatrixSize = m_paZoneStatusMatrix.size();
            m_msgId = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_OCC_MESSAGE_ID_START);
            int duration = 0;
            if (Start == m_operationType)
            {
                duration = DVAMsgLibDownloadScenario::getDurationFromMsgId(m_msgId);
            }            
            
			// reset status matrix
			for (size_t i = 0; i < size; ++i)
			{
				m_tcpModbusTablesManager->setRegisterValue(*(m_statusMatrix.begin() + i), 0x00);
			}

			ByteVector vecRegPaZoneValue;
			vecRegPaZoneValue.push_back( 0x00 );
			vecRegPaZoneValue.push_back( 0x00 );
            // reset pa zone status matrix
			for (size_t i = 0; i < paZoneMatrixSize; ++i)
			{
				//m_tcpModbusTablesManager->setRegisterValue(*(m_paZoneStatusMatrix.begin() + i), 0x00);

				deliveryUserQuery(Set, *(m_paZoneStatusMatrix.begin() + i), *(m_paZoneStatusMatrix.begin() + i), &vecRegPaZoneValue, 2);
			}

			// update state change counter
			ByteVector vecCounterVal;
			int oriValue = m_tcpModbusTablesManager->getRegisterValue(PAS_POLLING_ZONE_STATE_COUNTER_START);
			++oriValue;
			vecCounterVal.push_back(MSB(oriValue));
			vecCounterVal.push_back(LSB(oriValue));
			deliveryUserQuery(Set, PAS_POLLING_ZONE_STATE_COUNTER_START, PAS_POLLING_ZONE_STATE_COUNTER_START, &vecCounterVal, 6);

			// reset scenario state
            m_isInCurrentScenario = false;
            m_operationType = Undefined;
            m_tcpModbusTablesManager->resetMatrixRoutingTable();
        }
    }
}

void TA_IRS_App::BroadcastDvaMessageScenario::reponseWriteQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if ( MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
        if (IS_WRITE_COMMAND_BE_SENT(p))
        {
            if ( Start == m_operationType)
            {
                m_tcpModbusTablesManager->setRegisterValue(PAS_ADDRESS_EXECUTION_REGISTER_START, 1);

                size_t size; 
                int duration;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "in BroadcastDvaMessageScenario: start BroadcastDvaMessage");

                m_statusMatrix.clear();
                m_paZoneStatusMatrix.clear();
                          
                if (OCC == m_tcpModbusTablesManager->getLocationType()) // Mater DVA Broadcast
                {
                    m_msgId = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_OCC_MESSAGE_ID_START);
                    duration = DVAMsgLibDownloadScenario::getDurationFromMsgId(m_msgId);
                    m_tcpModbusTablesManager->getMatrixRoutingTableAddressIsOne(m_statusMatrix);
                    m_msgId+=100;

                    // update PA Zone status
                    m_tcpModbusTablesManager->getPAZoneStatusTableAddressIsOne(m_paZoneStatusMatrix);
                    size_t paZoneStatusMatrixsize = m_paZoneStatusMatrix.size();

                    unsigned long transactionValue;
                    transactionValue = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_OCC_TRANSACTION_ID);
                    for (size_t i = 0; i < paZoneStatusMatrixsize; ++i)
                    {
                        m_tcpModbusTablesManager->setRegisterValue(*(m_paZoneStatusMatrix.begin() + i), transactionValue + 12288);
                    }
                }
                else // Station/DCC DVA Broadcast
                {
                    m_msgId = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_STN_MESSAGE_ID_START);
                    duration = DVAMsgLibDownloadScenario::getDurationFromMsgId(m_msgId);
                    m_tcpModbusTablesManager->getSpeakerLineTableAddressIsOne(m_statusMatrix);
                    m_msgId+=0x0C00;

                    // update PA Zone status
                    m_tcpModbusTablesManager->getPAZoneStatusTableAddressIsOne(m_paZoneStatusMatrix);
                    size_t paZoneStatusMatrixsize = m_paZoneStatusMatrix.size();

                    unsigned long transactionValue;
                    transactionValue = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_STN_TRANSACTION_ID);
                    for (size_t i = 0; i < paZoneStatusMatrixsize; ++i)
                    {
                        m_tcpModbusTablesManager->setRegisterValue(*(m_paZoneStatusMatrix.begin() + i), transactionValue + 12288);
                    }
                }
                size = m_statusMatrix.size();
                
                if ( -1 == duration)
                {
                    // Set Cmd Ack Reg
                    setCmdAckReg2Zero(0x2900);
                }
                else
                {
                    for (size_t i = 0; i < size; ++i)
                    {
						m_tcpModbusTablesManager->setRegisterValue(*(m_statusMatrix.begin() + i), m_msgId);
					}

					// update Change Counter
					int oriValue = m_tcpModbusTablesManager->getRegisterValue(PAS_POLLING_ZONE_STATE_COUNTER_START);
					++oriValue;
					m_tcpModbusTablesManager->setRegisterValue(PAS_POLLING_ZONE_STATE_COUNTER_START, oriValue);
					// update state change counter
					//ByteVector vecCounterVal;
					//int oriValue = m_tcpModbusTablesManager->getRegisterValue(PAS_POLLING_ZONE_STATE_COUNTER_START);
					//++oriValue;
					//vecCounterVal.push_back(MSB(oriValue));
					//vecCounterVal.push_back(LSB(oriValue));
					//deliveryUserQuery(Set, PAS_POLLING_ZONE_STATE_COUNTER_START, PAS_POLLING_ZONE_STATE_COUNTER_START, &vecCounterVal, 4);

                    // Set Cmd Ack Reg
                    setCmdAckReg2Zero();
                }                    
            }
            if ( Stop == m_operationType)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "in BroadcastDvaMessageScenario: stop BroadcastDvaMessage");   
                setCmdAckReg2Zero();

                // reset matrix table and throw User Query
                m_tcpModbusTablesManager->resetMatrixRoutingTable();
            }
            setExecutionRegister2Zero(); 
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "in BroadcastDvaMessageScenario: set reg 9");
        }
    }
}

TA_IRS_App::BroadcastDvaMessageScenario::~BroadcastDvaMessageScenario()
{
    
}

