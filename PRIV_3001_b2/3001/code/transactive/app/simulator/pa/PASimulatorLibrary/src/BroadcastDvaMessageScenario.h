#ifndef PA_SIM_BROADCAST_DVA_MESSAGE_SCENARIO_H
#define PA_SIM_BROADCAST_DVA_MESSAGE_SCENARIO_H
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"

namespace TA_IRS_App
{
    class BroadcastDvaMessageScenario: public AbstractScenario
    {
    public:
        typedef enum OPERATIONTYPE
        {
            Undefined,
                Start,
                Stop
        };
        BroadcastDvaMessageScenario(PASimStandardTcpModbusTablesManager* parent):AbstractScenario(parent)
        {
            m_operationType = Undefined;
        }
        virtual ~BroadcastDvaMessageScenario();
        virtual bool isMySpecialScenario(TA_IRS_App::PA_Sim::ModbusQuery* query);
        virtual void reponseReadQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
        virtual void reponseWriteQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
    protected:
    private:
        int m_msgId;
        std::vector<int> m_statusMatrix;
        std::vector<unsigned long> m_paZoneStatusMatrix;
        OPERATIONTYPE m_operationType;
        BroadcastDvaMessageScenario(const BroadcastDvaMessageScenario&);
        BroadcastDvaMessageScenario& operator=(const BroadcastDvaMessageScenario&);
    };
}
#endif //PA_SIM_BROADCAST_DVA_MESSAGE_SCENARIO_H