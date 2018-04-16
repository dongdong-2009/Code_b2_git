#include "app/simulator/pa/PASimulatorLibrary/src/AcknowledgeFireAlarmScenario.h"
namespace TA_IRS_App
{


    AcknowledgeFireAlarmScenario::~AcknowledgeFireAlarmScenario()
    {

    }

    bool AcknowledgeFireAlarmScenario::isMySpecialScenario( TA_IRS_App::PA_Sim::ModbusQuery* query )
    {
        if (MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
        {
            ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);

            if ((PAS_ADDRESS_OCC_COMMAND_REGISTER_START == p->m_startAddress  ||
                PAS_ADDRESS_STN_COMMAND_REGISTER_START == p->m_startAddress ) && 
                1 == p->m_registersQuantity &&(
                (0x62 == p->m_registersValue[0]) ||
                (0x02 == p->m_registersValue[0] &&
                  0x03 == p->m_registersValue[1])
                )
                )
            {
                m_isInCurrentScenario = true;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Entering AcknowledgeFireAlarmScenario ...");
            }
        }
        return m_isInCurrentScenario;
    }

    void AcknowledgeFireAlarmScenario::reponseReadQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
    {
        if (MODBUS_READ_REGISTER_FUNCTION_CODE == query->m_functionCode)
        {
            ModbusReadQuery* p = dynamic_cast<ModbusReadQuery*>(query);
            if ( PAS_ADDRESS_COMMAND_RESULT_START == p->m_startAddress && 1 == p->m_registersQuantity)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Exiting AcknowledgeFireAlarmScenario...");
                m_isInCurrentScenario = false;
            }
        }
    }

    void AcknowledgeFireAlarmScenario::reponseWriteQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
    {
        if ( MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
        {
            ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
            if (IS_EXE_REG_BE_SET(p))
            {
                setCmdAckReg2Zero();
                setExecutionRegister2Zero();
            }
        }
    }

}