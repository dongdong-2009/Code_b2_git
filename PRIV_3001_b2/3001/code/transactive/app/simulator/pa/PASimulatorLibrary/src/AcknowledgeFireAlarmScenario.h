#ifndef PA_SIM_ACK_FIRE_ALARM_SCENARIO_H
#define PA_SIM_ACK_FIRE_ALARM_SCENARIO_H
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"

namespace TA_IRS_App
{
    class AcknowledgeFireAlarmScenario: public AbstractScenario
    {
    public:
        AcknowledgeFireAlarmScenario(PASimStandardTcpModbusTablesManager* parent):AbstractScenario(parent){}
        virtual ~AcknowledgeFireAlarmScenario();
        virtual bool isMySpecialScenario(TA_IRS_App::PA_Sim::ModbusQuery* query);
        virtual void reponseReadQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
        virtual void reponseWriteQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
    protected:
    private:
        AcknowledgeFireAlarmScenario(const AcknowledgeFireAlarmScenario&);
        AcknowledgeFireAlarmScenario& operator=(const AcknowledgeFireAlarmScenario&);
    };
}
#endif //PA_SIM_ACK_FIRE_ALARM_SCENARIO_H