#include <CTIME>

#include "app/simulator/pa/PASimulatorLibrary/src/PASimStandardModbusTcpTable.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableQueryProcessor.h"
#include "app/simulator/pa/PASimulatorLibrary/src/CommonScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
using namespace TA_IRS_App::PA_Sim;
TA_IRS_App::AbstractScenario* TA_IRS_App::AbstractScenario::startScenario = NULL;
namespace TA_IRS_App
{

CommonScenario::~CommonScenario()
{
    
}

bool CommonScenario::isMySpecialScenario( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if ( 0x10 == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
        if (IS_EXE_REG_BE_SET(p))
        {
            setExecutionRegister2Zero();
            return true;
        }
    }
    return false;

}

void CommonScenario::reponseReadQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    m_isInCurrentScenario = false;
}

void CommonScenario::reponseWriteQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    m_isInCurrentScenario = false;
}

}
