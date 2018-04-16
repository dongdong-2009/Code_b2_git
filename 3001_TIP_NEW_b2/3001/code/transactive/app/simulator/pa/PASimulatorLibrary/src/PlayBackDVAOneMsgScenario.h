#ifndef PA_SIM_PlayBackDVAOneMsgScenario_SCENARIO_H
#define PA_SIM_PlayBackDVAOneMsgScenario_SCENARIO_H
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"

namespace TA_IRS_App
{
    class PlayBackDVAOneMsgScenario: public AbstractScenario
    {
    public:
        typedef enum OPERATIONTYPE
        {
            Undefined,
            Start,
            Stop
        };
        PlayBackDVAOneMsgScenario(PASimStandardTcpModbusTablesManager* parent):AbstractScenario(parent){ m_operationType = Undefined;}
        virtual ~PlayBackDVAOneMsgScenario();
        virtual bool isMySpecialScenario(TA_IRS_App::PA_Sim::ModbusQuery* query);
        virtual void reponseReadQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
        virtual void reponseWriteQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
    protected:
    private:
        OPERATIONTYPE m_operationType;
        int m_clientNumber;
        PlayBackDVAOneMsgScenario(const PlayBackDVAOneMsgScenario&);
        PlayBackDVAOneMsgScenario& operator=(const PlayBackDVAOneMsgScenario&);
    };
}
#endif //PA_SIM_PlayBackDVAOneMsgScenario_SCENARIO_H