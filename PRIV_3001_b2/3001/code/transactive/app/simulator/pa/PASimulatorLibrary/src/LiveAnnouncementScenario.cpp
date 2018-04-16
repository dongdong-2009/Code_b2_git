#include "app/simulator/pa/PASimulatorLibrary/src/LiveAnnouncementScenario.h"
namespace TA_IRS_App
{
	int LiveAnnouncementScenario::LiveBroadcastReponseTime = 10;

    bool LiveAnnouncementScenario::isMySpecialScenario( TA_IRS_App::PA_Sim::ModbusQuery* query )
    {
		if (MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
		{
			ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);

			if ( (PAS_ADDRESS_OCC_COMMAND_REGISTER_START == p->m_startAddress \
				|| PAS_ADDRESS_STN_COMMAND_REGISTER_START == p->m_startAddress) \
				&& 1 == p->m_registersQuantity )
			{
				switch( p->m_registersValue[0] )
				{
				case 0x14: // Start live broadcast with chime
					m_operationType = Start;
					m_isInCurrentScenario = true;
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Entering LiveAnnouncementScenario with chime...");
					break;
				case 0x13: // Start live broadcast without chime
					m_operationType = Start;
					m_isInCurrentScenario = true;
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Entering LiveAnnouncementScenario without chime...");
					break;
				default:
					m_operationType = Undefined;
					m_isInCurrentScenario = false;
				}
			}
		}
		return m_isInCurrentScenario;
    }
    
    LiveAnnouncementScenario::~LiveAnnouncementScenario()
    {
        
    }
    
    void LiveAnnouncementScenario::reponseReadQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
    {
        if (MODBUS_READ_REGISTER_FUNCTION_CODE == query->m_functionCode)
        {
            ModbusReadQuery* p = dynamic_cast<ModbusReadQuery*>(query);
            if ( PAS_ADDRESS_COMMAND_RESULT_START == p->m_startAddress && 1 == p->m_registersQuantity)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finishing LiveAnnouncementScenario ...");
                m_isInCurrentScenario = false;
                m_operationType = Undefined;
                m_tcpModbusTablesManager->resetMatrixRoutingTable();
            }
        }
    }
    
    void LiveAnnouncementScenario::reponseWriteQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
    {
        if ( MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
        {
            ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);

            if (IS_WRITE_COMMAND_BE_SENT(p))
            {
                m_tcpModbusTablesManager->setRegisterValue(PAS_ADDRESS_EXECUTION_REGISTER_START, 1);

				size_t size; 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "in LiveAnnouncementScenario: receive write command ");

				m_statusMatrix.clear();
                m_paZoneStatusMatrix.clear();

				if (OCC == m_tcpModbusTablesManager->getLocationType()) // Mater DVA Broadcast
				{
					// get matrix table
					m_tcpModbusTablesManager->getMatrixRoutingTableAddressIsOne(m_statusMatrix);
					size = m_statusMatrix.size();

					// set matrix table to client number
					int cmdValue;
					cmdValue = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_OCC_COMMAND_REGISTER_START);
					for (size_t i = 0; i < size; ++i)
					{
						m_tcpModbusTablesManager->setRegisterValue(*(m_statusMatrix.begin() + i), LSB(cmdValue));
					}

                    // update PA Zone status
                    m_tcpModbusTablesManager->getPAZoneStatusTableAddressIsOne(m_paZoneStatusMatrix);
                    size = m_paZoneStatusMatrix.size();

                    unsigned long transactionValue;
                    transactionValue = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_OCC_TRANSACTION_ID);
                    for (size_t i = 0; i < size; ++i)
                    {
                        m_tcpModbusTablesManager->setRegisterValue(*(m_paZoneStatusMatrix.begin() + i), transactionValue + 12288);
                    }

					// update state change counter
					ByteVector vecCounterVal;
					int oriValue = m_tcpModbusTablesManager->getRegisterValue(PAS_POLLING_ZONE_STATE_COUNTER_START);
					++oriValue;
					vecCounterVal.push_back(MSB(oriValue));
					vecCounterVal.push_back(LSB(oriValue));
					deliveryUserQuery(Set, PAS_POLLING_ZONE_STATE_COUNTER_START, PAS_POLLING_ZONE_STATE_COUNTER_START, &vecCounterVal, 3);


					// deliver reset matrix update query
					ByteVector vecRegValue;
					vecRegValue.push_back( 0x00 );
					vecRegValue.push_back( 0x00 );

					for(int i = 0; i < size; ++i)
					{
						deliveryUserQuery(Set, *(m_statusMatrix.begin() + i), *(m_statusMatrix.begin() + i), &vecRegValue, LiveBroadcastReponseTime);
					}
					

					//// deliver pa zone matrix update query
					for(int i = 0; i < size; ++i)
					{
						deliveryUserQuery(Set, *(m_paZoneStatusMatrix.begin() + i), *(m_paZoneStatusMatrix.begin() + i), &vecRegValue, LiveBroadcastReponseTime);
					}


					// deliver status matrix counter update query
					deliveryUserQuery(Inc, PAS_POLLING_ZONE_STATE_COUNTER_START, PAS_POLLING_ZONE_STATE_COUNTER_START, 0, LiveBroadcastReponseTime + 5);

				}
				else // Station/DCC DVA Broadcast
				{
					m_tcpModbusTablesManager->getMatrixRoutingTableAddressIsOne(m_statusMatrix);
					size = m_statusMatrix.size();

					// set matrix table to client number
					for (size_t i = 0; i < size; ++i)
					{
						m_tcpModbusTablesManager->setRegisterValue(*(m_statusMatrix.begin() + i), PAS_COMMAND_STN_START_LIVE_BROADCAST_WITH_CHIME_SOURCE_ID);
					}

                    // update PA Zone status
                    m_tcpModbusTablesManager->getPAZoneStatusTableAddressIsOne(m_paZoneStatusMatrix);
                    size = m_paZoneStatusMatrix.size();

                    unsigned long transactionValue;
                    transactionValue = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_STN_TRANSACTION_ID);
                    for (size_t i = 0; i < size; ++i)
                    {
                        m_tcpModbusTablesManager->setRegisterValue(*(m_paZoneStatusMatrix.begin() + i), transactionValue + 12288);
                    }

					// update state change counter
					ByteVector vecCounterVal;
					int oriValue = m_tcpModbusTablesManager->getRegisterValue(PAS_POLLING_ZONE_STATE_COUNTER_START);
					++oriValue;
					vecCounterVal.push_back(MSB(oriValue));
					vecCounterVal.push_back(LSB(oriValue));
					deliveryUserQuery(Set, PAS_POLLING_ZONE_STATE_COUNTER_START, PAS_POLLING_ZONE_STATE_COUNTER_START, &vecCounterVal, 3);


					// deliver reset matrix update query
					ByteVector vecRegValue;
					vecRegValue.push_back( 0x00 );
					vecRegValue.push_back( 0x00 );
					for(int i = 0; i < size; ++i)
					{
						deliveryUserQuery(Set, *(m_statusMatrix.begin() + i), *(m_statusMatrix.begin() + i), &vecRegValue, LiveBroadcastReponseTime);

					}
					
                    //// deliver pa zone matrix update query
                    for(int i = 0; i < size; ++i)
                    {
                        deliveryUserQuery(Set, *(m_paZoneStatusMatrix.begin() + i), *(m_paZoneStatusMatrix.begin() + i), &vecRegValue, LiveBroadcastReponseTime);
                    }

					// deliver status matrix counter update query
					deliveryUserQuery(Inc, PAS_POLLING_ZONE_STATE_COUNTER_START, PAS_POLLING_ZONE_STATE_COUNTER_START, 0 , LiveBroadcastReponseTime + 5);
				}


				// Set Cmd Ack Reg
				setCmdAckReg2Zero();
            }

			setExecutionRegister2Zero(); 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "in LiveAnnouncementScenario: set reg 9");
        }
    }

	void LiveAnnouncementScenario::setLiveBroadcastReponseTime( int t )
	{
		LiveBroadcastReponseTime = t;
	}

}