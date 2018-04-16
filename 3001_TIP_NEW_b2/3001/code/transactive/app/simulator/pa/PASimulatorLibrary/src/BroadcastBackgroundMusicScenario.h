#ifndef PA_SIM_BROADCAST_BACKGROUND_MUSIC_SCENARIO_H
#define PA_SIM_BROADCAST_BACKGROUND_MUSIC_SCENARIO_H
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"

namespace TA_IRS_App
{
    class BroadcastBackgroundMusicScenario: public AbstractScenario
    {
    public:
        typedef enum OPERATIONTYPE
        {
            Undefined,
                Start,
                Stop
        };
        BroadcastBackgroundMusicScenario(PASimStandardTcpModbusTablesManager* parent):AbstractScenario(parent)
        {
            m_operationType = Undefined;
        }
        virtual ~BroadcastBackgroundMusicScenario();
        virtual bool isMySpecialScenario(TA_IRS_App::PA_Sim::ModbusQuery* query);
        virtual void reponseReadQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
        virtual void reponseWriteQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
    protected:
    private:
        std::vector<int> m_statusMatrix;
        OPERATIONTYPE m_operationType;
        BroadcastBackgroundMusicScenario(const BroadcastBackgroundMusicScenario&);
        BroadcastBackgroundMusicScenario& operator=(const BroadcastBackgroundMusicScenario&);
    };
}
#endif //PA_SIM_BROADCAST_BACKGROUND_MUSIC_SCENARIO_H