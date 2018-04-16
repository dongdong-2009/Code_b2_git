#include "app/simulator/pa/PASimulatorLibrary/src/BroadcastScheduledDVAMsgScenario.h"

TA_IRS_App::BroadcastScheduledDVAMsgScenario::~BroadcastScheduledDVAMsgScenario()
{
    
}

bool TA_IRS_App::BroadcastScheduledDVAMsgScenario::isMySpecialScenario( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    m_isInCurrentScenario = false;
    if (MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);

        if (( PAS_ADDRESS_OCC_COMMAND_REGISTER_START == p->m_startAddress || 
             PAS_ADDRESS_STN_COMMAND_REGISTER_START == p->m_startAddress )&& 
             1 == p->m_registersQuantity)
        {
            switch( p->m_registersValue[0] )
            {
            case 0x49: // create scheduled without chime
            case 0x69: // create scheduled with chime
                m_operationType = CreateMsg;
                m_isInCurrentScenario = true;
                break;
            case 0x4A: // delete scheduled
                m_operationType = DeleteMsg;
                m_scheduleEventId = p->m_registersValue[1];
                m_isInCurrentScenario = true;
                break;
            default:
                m_operationType = Undefined;
                break;
            }
        }
    }
    if (m_isInCurrentScenario)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Entering BroadcastScheduledDVAMsgScenario ...");
    }
    return m_isInCurrentScenario;
}

void TA_IRS_App::BroadcastScheduledDVAMsgScenario::reponseReadQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if (MODBUS_READ_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusReadQuery* p = dynamic_cast<ModbusReadQuery*>(query);
        if ( PAS_ADDRESS_COMMAND_RESULT_START == p->m_startAddress && 1 == p->m_registersQuantity && m_isInCurrentScenario)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finishing BroadcastScheduledDVAMsgScenario ...");
            m_isInCurrentScenario = false;
            m_operationType = Undefined;
            m_tcpModbusTablesManager->resetMatrixRoutingTable();
        }
    }
}

void TA_IRS_App::BroadcastScheduledDVAMsgScenario::reponseWriteQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if (MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
        if (IS_EXE_REG_BE_SET(p))
        {   
            if ( CreateMsg == m_operationType)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In BroadcastScheduledDVAMsgScenario CreateMsg...");
            }
            else if ( DeleteMsg == m_operationType)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In BroadcastScheduledDVAMsgScenario DeleteMsg...");
            }
            setCmdAckReg2Zero();
            setExecutionRegister2Zero();
        }
    }
}
