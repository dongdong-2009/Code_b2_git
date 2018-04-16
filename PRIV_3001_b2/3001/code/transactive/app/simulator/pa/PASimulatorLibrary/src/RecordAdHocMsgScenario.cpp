#include "app/simulator/pa/PASimulatorLibrary/src/RecordAdHocMsgScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/DVAMsgLibDownloadScenario.h"
TA_IRS_App::RecordAdHocMsgScenario::~RecordAdHocMsgScenario()
{
    
}

bool TA_IRS_App::RecordAdHocMsgScenario::isMySpecialScenario( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if (MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);

        if (  ( PAS_ADDRESS_OCC_COMMAND_REGISTER_START == p->m_startAddress 
			|| PAS_ADDRESS_STN_COMMAND_REGISTER_START == p->m_startAddress)
			&& 1 == p->m_registersQuantity
			)
        {
            m_isInCurrentScenario = true;
			switch ( p->m_registersValue[0])
			{
			case MSB(PAS_COMMAND_OCC_DVA_RECORDING_START):
				m_operationType = START_RECORD;
                m_clientNumber = p->m_registersValue[1];
				break;
			case MSB(PAS_COMMAND_OCC_DVA_RECORDING_STOP):
				m_operationType = STOP_RECORD;
				break;
			case MSB(PAS_COMMAND_OCC_DVA_RECORDING_CHANGE_TYPE):
				m_operationType = CHANGE_TYPE;
				m_clientNumber = p->m_registersValue[1];
				break;
			case MSB(PAS_COMMAND_OCC_DVA_RECORDING_DELETE):
				m_operationType = DEL;
				break;
			default:
				m_isInCurrentScenario = false;
			}
			if (m_isInCurrentScenario)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Entering RecordAdHocMsgScenario ...");
			}
        }

    }
    return m_isInCurrentScenario;
}

void TA_IRS_App::RecordAdHocMsgScenario::reponseReadQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if (MODBUS_READ_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusReadQuery* p = dynamic_cast<ModbusReadQuery*>(query);
        if ( PAS_ADDRESS_COMMAND_RESULT_START == p->m_startAddress && 1 == p->m_registersQuantity)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Exiting RecordAdHocMsgScenario ...");
            if (OCC == m_tcpModbusTablesManager->getLocationType())
            {
                //m_tcpModbusTablesManager->setRegisterValue(m_clientNumber - 1 + 100, 0x400);
				m_tcpModbusTablesManager->setRegisterValue(m_clientNumber + 100, 0x400);
            } 
            else
            {
                m_tcpModbusTablesManager->setRegisterValue(PAS_ADDRESS_STN_PLAYBACK_STATUS, 0x400);
            }
            m_isInCurrentScenario = false;
            m_operationType = UNDEF;
        }
    }
}

void TA_IRS_App::RecordAdHocMsgScenario::reponseWriteQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if ( MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
        if (IS_EXE_REG_BE_SET(p))
        {
			setCmdAckReg2Zero();

            m_msgId = getMessageID();
			
			switch(m_operationType)
			{
			case START_RECORD:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Running RecordAdHocMsgScenario: Start DVA Record");
				break;
			case STOP_RECORD:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Running RecordAdHocMsgScenario: Stop DVA Record");
				break;
			case DEL:
				TA_IRS_App::DVAMsgLibDownloadScenario::DeleteDvaMessage(m_msgId);
                TA_IRS_App::DVAMsgLibDownloadScenario::updateMessageLibraryVersion(this);
                TA_IRS_App::DVAMsgLibDownloadScenario::updateMessageChangeCounter(this);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Running RecordAdHocMsgScenario: Delete DVA Record");
				break;
			case CHANGE_TYPE:
				TA_IRS_App::DVAMsgLibDownloadScenario::ChangeMessageType(m_msgId);
                TA_IRS_App::DVAMsgLibDownloadScenario::updateMessageLibraryVersion(this);
                TA_IRS_App::DVAMsgLibDownloadScenario::updateMessageChangeCounter(this);
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Running RecordAdHocMsgScenario: Changetype DVA Record");
				break;
			default:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Running RecordAdHocMsgScenario: Undefined operation type");
			}
			setExecutionRegister2Zero();
        }
    }
}