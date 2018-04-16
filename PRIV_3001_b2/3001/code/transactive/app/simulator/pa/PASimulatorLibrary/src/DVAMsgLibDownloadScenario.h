#ifndef PA_SIM_DVA_MSG_LIB_DOWNLOAD_SCENARIO_H
#define PA_SIM_DVA_MSG_LIB_DOWNLOAD_SCENARIO_H
#include "app/simulator/pa/PASimulatorLibrary/src/AbstractScenario.h"

namespace TA_IRS_App
{
    struct DvaMsg 
    {
        int index;
        int msgId;
        int msgBank;
        int duration;
        std::string msgName;
    };
    class DVAMsgLibDownloadScenario: public AbstractScenario
    {
    public:
        static std::vector<DvaMsg> DvaMessageVec;
        DVAMsgLibDownloadScenario(PASimStandardTcpModbusTablesManager* parent);
        virtual ~DVAMsgLibDownloadScenario();
        virtual bool isMySpecialScenario(TA_IRS_App::PA_Sim::ModbusQuery* query);
        virtual void reponseReadQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
        virtual void reponseWriteQuery(TA_IRS_App::PA_Sim::ModbusQuery* query); // must set isInCurrentScenario to false if the last step has been finished
        static void ChangeMessageType(int msgId);
        static void DeleteDvaMessage(int msgId);
        static int getDurationFromMsgId(int msgId);
        static void updateMessageChangeCounter(TA_IRS_App::AbstractScenario* trigger);
        static void updateMessageLibraryVersion(TA_IRS_App::AbstractScenario* trigger);
        static int MassageChangeCounter;
        static ByteVector LibVersionNumber;
    protected:
    private:
        time_t m_lastModDvaMsgLibTime;
        int m_msgIndex;
        /*std::vector<DvaMsg> m_dvaMsgVec;*/
        void checkAndUpdateDVAMsgLib();
        void updateRegistersAccordingToDvaMsgLib(int index);

        DVAMsgLibDownloadScenario(const DVAMsgLibDownloadScenario&);
        DVAMsgLibDownloadScenario& operator=(const DVAMsgLibDownloadScenario&);
    };
}
#endif //PA_SIM_DVA_MSG_LIB_DOWNLOAD_SCENARIO_H