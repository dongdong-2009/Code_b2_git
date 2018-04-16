#include "app/simulator/pa/PASimulatorLibrary/src/PlayBackDVAOneMsgScenario.h"
namespace TA_IRS_App
{    
    PlayBackDVAOneMsgScenario::~PlayBackDVAOneMsgScenario()
    {
        
    }
    
    bool PlayBackDVAOneMsgScenario::isMySpecialScenario( TA_IRS_App::PA_Sim::ModbusQuery* query )
    {
        if (MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
        {
            ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);

            if ((PAS_ADDRESS_OCC_COMMAND_REGISTER_START == p->m_startAddress  ||
				PAS_ADDRESS_STN_COMMAND_REGISTER_START == p->m_startAddress ) && 
				1 == p->m_registersQuantity 
                && 0x21 == p->m_registersValue[0])
            {
                m_isInCurrentScenario = true;
                m_clientNumber = p->m_registersValue[1];
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Entering PlayBackDVAOneMsgScenario ...");
            }
        }
        return m_isInCurrentScenario;
    }
    
    void PlayBackDVAOneMsgScenario::reponseReadQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
    {
        if (MODBUS_READ_REGISTER_FUNCTION_CODE == query->m_functionCode)
        {
            ModbusReadQuery* p = dynamic_cast<ModbusReadQuery*>(query);
            if ( PAS_ADDRESS_COMMAND_RESULT_START == p->m_startAddress && 1 == p->m_registersQuantity)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Exiting PlayBackDVAOneMsgScenario...");
                if (OCC == m_tcpModbusTablesManager->getLocationType())
                {
                    //m_tcpModbusTablesManager->setRegisterValue(m_clientNumber - 1 + 100, 0x800);
					m_tcpModbusTablesManager->setRegisterValue(m_clientNumber + 100, 0x800);
                } 
                else
                {
                    m_tcpModbusTablesManager->setRegisterValue(PAS_ADDRESS_STN_PLAYBACK_STATUS, 0x800);
                }
                m_isInCurrentScenario = false;
                m_operationType = Undefined;
            }
        }
    }
    
    void PlayBackDVAOneMsgScenario::reponseWriteQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
    {
        if ( MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
        {
            ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
            if (IS_EXE_REG_BE_SET(p))
            {
				unsigned char ucMsgId;
				if (OCC == m_tcpModbusTablesManager->getLocationType())
				{
					ucMsgId = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_OCC_MESSAGE_ID_START);
				}
				else
				{
					ucMsgId = m_tcpModbusTablesManager->getRegisterValue(PAS_ADDRESS_STN_MESSAGE_ID_START);
				}
				if (0 == ucMsgId)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "PlayBackDVAOneMsgScenario:Stop Playback");
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "PlayBackDVAOneMsgScenario:Start Playback");
				}
				setCmdAckReg2Zero();
                setExecutionRegister2Zero();
            }
        }
    }
}