#ifndef PA_SIM_LIVE_ANNOUNCEMENT_SCENARIO_H
#define PA_SIM_LIVE_ANNOUNCEMENT_SCENARIO_H
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"

namespace TA_IRS_App
{
    class LiveAnnouncementScenario: public AbstractScenario
    {
    public:
        typedef enum OPERATIONTYPE
        {
            Undefined,
                Start,
                Stop
        };
        LiveAnnouncementScenario(PASimStandardTcpModbusTablesManager* parent):AbstractScenario(parent)
        {
            m_operationType = Undefined;
        }
        virtual ~LiveAnnouncementScenario();
        virtual bool isMySpecialScenario(TA_IRS_App::PA_Sim::ModbusQuery* query);
        virtual void reponseReadQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
        virtual void reponseWriteQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
		static void setLiveBroadcastReponseTime(int t);
    protected:
    private:
        std::vector<int> m_statusMatrix;
        std::vector<unsigned long> m_paZoneStatusMatrix;
        OPERATIONTYPE m_operationType;
        unsigned char m_status;
		static int LiveBroadcastReponseTime;
        LiveAnnouncementScenario(const LiveAnnouncementScenario&);
        LiveAnnouncementScenario& operator=(const LiveAnnouncementScenario&);
    };
}
#endif //PA_SIM_LIVE_ANNOUNCEMENT_SCENARIO_H