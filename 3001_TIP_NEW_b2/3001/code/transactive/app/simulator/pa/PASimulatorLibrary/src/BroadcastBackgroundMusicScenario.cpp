#include "app/simulator/pa/PASimulatorLibrary/src/BroadcastBackgroundMusicScenario.h"


TA_IRS_App::BroadcastBackgroundMusicScenario::~BroadcastBackgroundMusicScenario()
{
    
}

bool TA_IRS_App::BroadcastBackgroundMusicScenario::isMySpecialScenario( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if (MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
        m_isInCurrentScenario = false;
        if ( (PAS_ADDRESS_OCC_COMMAND_REGISTER_START == p->m_startAddress || 
             PAS_ADDRESS_STN_COMMAND_REGISTER_START == p->m_startAddress)
            && 1 == p->m_registersQuantity)
        {
            switch( p->m_registersValue[0] )
            {
            case 0x05: // Start background music
                m_operationType = Start;
                m_isInCurrentScenario = true;
                break;
            case 0x06: // Stop background music
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
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Entering BroadcastBackgroundMusicScenario ...");
    }
    return m_isInCurrentScenario;
}

void TA_IRS_App::BroadcastBackgroundMusicScenario::reponseReadQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if (MODBUS_READ_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusReadQuery* p = dynamic_cast<ModbusReadQuery*>(query);
        if ( PAS_ADDRESS_COMMAND_RESULT_START == p->m_startAddress && 1 == p->m_registersQuantity)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finishing BroadcastBackgroundMusicScenario...");
            m_isInCurrentScenario = false;
            m_operationType = Undefined;
            m_tcpModbusTablesManager->resetMatrixRoutingTable();
        }
    }
}

void TA_IRS_App::BroadcastBackgroundMusicScenario::reponseWriteQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if ( MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
        if (IS_EXE_REG_BE_SET(p))
        {
            if ( Start == m_operationType)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "in BroadcastBackgroundMusicScenario: start BroadcastBackgroundMusicScenario");
                size_t size;
                unsigned short command;
               
                m_statusMatrix.clear();

                if (OCC == m_tcpModbusTablesManager->getLocationType()) // Mater DVA Broadcast
                {
                    m_tcpModbusTablesManager->getMatrixRoutingTableAddressIsOne(m_statusMatrix);
                    command = PAS_STATUS_OCC_BACKGROUND_MUSIC;
                    size = m_statusMatrix.size();

                    for (size_t i = 0; i < size; ++i)
                    {
                        m_tcpModbusTablesManager->setRegisterValue(*(m_statusMatrix.begin() + i), command);
                    }
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "in BroadcastBackgroundMusicScenario: start BroadcastBackgroundMusicScenario OCC");
                }
                else // Station/DCC DVA Broadcast
                {
                    m_tcpModbusTablesManager->getSpeakerLineTableAddressIsOne(m_statusMatrix);
                    command = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_STN_SOURCE_ID);
                    
                    ByteVector data;
                    for (int i = PAS_ADDRESS_STN_ZONE_STATE_START; i <= PAS_ADDRESS_STN_ZONE_STATE_END; ++i)
                    {
                        data.push_back(LSB(command));
                        data.push_back(MSB(command));
                    }

                    m_tcpModbusTablesManager->setMultiRegistersValues(PAS_ADDRESS_STN_ZONE_STATE_START, PAS_ADDRESS_STN_ZONE_STATE_END, data);

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "in BroadcastBackgroundMusicScenario: start BroadcastBackgroundMusicScenario STN/DEPOT");
                }


                // update Change Counter
                int oriValue = m_tcpModbusTablesManager->getRegisterValue(PAS_POLLING_ZONE_STATE_COUNTER_START);
                ++oriValue;
                m_tcpModbusTablesManager->setRegisterValue(PAS_POLLING_ZONE_STATE_COUNTER_START, oriValue);

                // Set Cmd Ack Reg
                setCmdAckReg2Zero();              
            }
            if ( Stop == m_operationType)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "in BroadcastBackgroundMusicScenario: stop BroadcastBackground Music");         
                
                // reset status matrix
                if (OCC == m_tcpModbusTablesManager->getLocationType())
                {
                    size_t size = m_statusMatrix.size();

                    for (size_t i = 0; i < size; ++i)
                    {
                        m_tcpModbusTablesManager->setRegisterValue(*(m_statusMatrix.begin() + i), 0x00);
                    }
                } 
                else
                {
                    ByteVector data;
                    for (int i = PAS_ADDRESS_STN_ZONE_STATE_START; i <= PAS_ADDRESS_STN_ZONE_STATE_END; ++i)
                    {
                        data.push_back(0x00);
                        data.push_back(0x00);
                    }
                    m_tcpModbusTablesManager->setMultiRegistersValues(PAS_ADDRESS_STN_ZONE_STATE_START, PAS_ADDRESS_STN_ZONE_STATE_END, data);
                }

                setCmdAckReg2Zero();
            }
            setExecutionRegister2Zero(); 
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "in BroadcastBackgroundMusicScenario: set reg 9");
        }
    }
}
