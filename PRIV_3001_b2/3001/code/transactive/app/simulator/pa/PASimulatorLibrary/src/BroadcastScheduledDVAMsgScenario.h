#ifndef PA_SIM_BROADCAST_SCHEDULED_DVA_MSG_SCENARIO_H
#define PA_SIM_BROADCAST_SCHEDULED_DVA_MSG_SCENARIO_H
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"
#include "boost/shared_ptr.hpp"

namespace TA_IRS_App
{
    struct ScheduledDVAMsg
    {
        int m_messageId;
        int m_scheduledEventId;
        std::vector<int> m_statusMatrix;
        int m_priority;
        std::string m_startTime;
        std::string m_endTime;
        std::string m_period;
        unsigned char m_schDay;
        bool m_withChime;
    };


    class BroadcastScheduledDVAMsgScenario: public AbstractScenario
    {
    public:
        typedef enum OPERATIONTYPE
        {
            Undefined,
            CreateMsg,
            DeleteMsg
        };
        BroadcastScheduledDVAMsgScenario(PASimStandardTcpModbusTablesManager* parent):AbstractScenario(parent){ m_operationType = Undefined;}
        virtual ~BroadcastScheduledDVAMsgScenario();
        virtual bool isMySpecialScenario(TA_IRS_App::PA_Sim::ModbusQuery* query);
        virtual void reponseReadQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
        virtual void reponseWriteQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
    protected:
    private:
        std::vector< boost::shared_ptr<ScheduledDVAMsg> > m_scheduleVec;
        boost::shared_ptr<ScheduledDVAMsg> m_tmpScheduleSmartPtr;
        OPERATIONTYPE m_operationType;
        int m_scheduleEventId;
        BroadcastScheduledDVAMsgScenario(const BroadcastScheduledDVAMsgScenario&);
        BroadcastScheduledDVAMsgScenario& operator=(const BroadcastScheduledDVAMsgScenario&);
    };
}
#endif //PA_SIM_BROADCAST_SCHEDULED_DVA_MSG_SCENARIO_H