#ifndef PA_SIM_COMMON_SCENARIO_H
#define PA_SIM_COMMON_SCENARIO_H
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"

namespace TA_IRS_App
{
class CommonScenario: public AbstractScenario
{
public:
    CommonScenario(PASimStandardTcpModbusTablesManager* parent):AbstractScenario(parent){}
    virtual ~CommonScenario();
    virtual bool isMySpecialScenario(TA_IRS_App::PA_Sim::ModbusQuery* query);
    virtual void reponseReadQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
    virtual void reponseWriteQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
protected:
private:
    CommonScenario(const CommonScenario&);
    CommonScenario& operator=(const CommonScenario&);
};
}
#endif //PA_SIM_COMMON_SCENARIO_H