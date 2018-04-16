#ifndef PA_SIM_RECORD_AD_HOC_MSG_SCENARIO_H
#define PA_SIM_RECORD_AD_HOC_MSG_SCENARIO_H
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"

namespace TA_IRS_App
{
    class RecordAdHocMsgScenario: public AbstractScenario
    {
    public:
        typedef enum 
        {
            UNDEF,
			START_RECORD,
			STOP_RECORD,
            CHANGE_TYPE,
            DEL
        }OPERATIONTYPE;
        RecordAdHocMsgScenario(PASimStandardTcpModbusTablesManager* parent):AbstractScenario(parent){ m_operationType = UNDEF;}
        virtual ~RecordAdHocMsgScenario();
        virtual bool isMySpecialScenario(TA_IRS_App::PA_Sim::ModbusQuery* query);
        virtual void reponseReadQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
        virtual void reponseWriteQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
    protected:
    private:
        int m_msgId;
        OPERATIONTYPE m_operationType;
        int m_clientNumber;
        RecordAdHocMsgScenario(const RecordAdHocMsgScenario&);
        RecordAdHocMsgScenario& operator=(const RecordAdHocMsgScenario&);
    };
}
#endif //PA_SIM_RECORD_AD_HOC_MSG_SCENARIO_H