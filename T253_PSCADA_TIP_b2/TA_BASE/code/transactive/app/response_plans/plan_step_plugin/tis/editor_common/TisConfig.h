#pragma once

#include <string>
#include <vector>
#include <set>
#include "app/response_plans/plan_step_plugin/tis/common/TisCommonDef.h"
#include "core/data_access_interface/tis_agent/src/IPredefinedMessageLibrary.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
//#include "app/response_plans/plan_mft_step_plugin/plan_step_common/src/PlanStepCommonDef.h"

namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// TisConfigAccess

class TisConfig
{
    // Types
public:
    // Extend TA_Base_Core::PredefinedMessage with a copy ctor and an assignment
    // operator so that it can be used with STL containers.
    struct PredefinedTisMessage : public TA_Base_Core::PredefinedMessage
    {
		static const unsigned short TIS_UNDEFINED_MESSAGE_ID;
        PredefinedTisMessage()
        {
            messageTag = TIS_UNDEFINED_MESSAGE_ID;
            librarySection = 0;
            description = "";
            priority = 0;
            message = "";
            englishOnly = true;
            repeatInterval = 0;
        }

        PredefinedTisMessage(const TA_Base_Core::PredefinedMessage& predefinedMessage)
        {
            *this = predefinedMessage;
        }

        PredefinedTisMessage& operator=(const TA_Base_Core::PredefinedMessage& predefinedMessage)
        {
            if(this != &predefinedMessage)
            {
                messageTag = predefinedMessage.messageTag;
                librarySection = predefinedMessage.librarySection;
                description = predefinedMessage.description;
                priority = predefinedMessage.priority;
                message = predefinedMessage.message;
                englishOnly = predefinedMessage.englishOnly;
                repeatInterval = predefinedMessage.repeatInterval;
            }

            return *this;
        }
    };

    typedef std::vector<PredefinedTisMessage> PredefinedTisMessageList;
    typedef std::set<std::string> TisStationNameSet;
    typedef std::set<std::string> TisStationLevelSet;
    typedef std::set<std::string> TisPidSet;

    // Operations
public:
    static std::string getSTisMessageText(const TISMessage& message);
    static std::string getTTisMessageText(const TISMessage& message);
    static PredefinedTisMessageList getSTisMessages(unsigned long libraryVersion);
    static PredefinedTisMessageList getTTisMessages(unsigned long libraryVersion);
    static TisStationNameSet getSTisStations();
    static TisStationLevelSet getSTisLevels(const std::string& stationName = "");
    static TisPidSet getSTisPids(const std::string& stationName = "");

private:
    static std::string getTisMessageText(const TISMessage& message, const std::string& libraryType);
    static void getTisMessages(unsigned long libraryVersion, const std::string& libraryType, PredefinedTisMessageList& messages);
};
}