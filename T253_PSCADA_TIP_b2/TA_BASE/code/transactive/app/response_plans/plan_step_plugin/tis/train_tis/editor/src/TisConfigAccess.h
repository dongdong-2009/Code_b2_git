/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TisConfigAccess.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * This is a utility class. It's used to retrieve from the database
  * various TIS related configuration data items.
  *
  */

#if !defined(AFX_TISCONFIGACCESS_H__7ED31E52_E6DD_4577_8BAD_A6A06BF9C28B__INCLUDED_)
#define AFX_TISCONFIGACCESS_H__7ED31E52_E6DD_4577_8BAD_A6A06BF9C28B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <set>
#include "app/response_plans/plan_step_plugin/tis/common/TisCommonDef.h"
#include "core/data_access_interface/tis_agent/src/IPredefinedMessageLibrary.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TisConfigAccess

	const unsigned short TIS_UNDEFINED_MESSAGE_ID = 0xFFFF;
    class TisConfigAccess
    {
    // Types
    public:
        // Extend TA_Base_Core::PredefinedMessage with a copy ctor and an assignment
        // operator so that it can be used with STL containers.
        struct PredefinedTisMessage : public TA_Base_Core::PredefinedMessage
        {
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
                if (this != &predefinedMessage)
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
        static PredefinedTisMessageList getSTisMessages(unsigned short libraryVersion);
        static PredefinedTisMessageList getTTisMessages(unsigned short libraryVersion);
        static TisStationNameSet getSTisStations();
        static TisStationLevelSet getSTisLevels(const std::string& stationName = "");
        static TisPidSet getSTisPids(const std::string& stationName = "");

    private:
        static std::string getTisMessageText(const TISMessage& message, const std::string& libraryType);
        static void getTisMessages(unsigned short libraryVersion, const std::string& libraryType, PredefinedTisMessageList& messages);
    };
}
#endif // !defined(AFX_TISCONFIGACCESS_H__7ED31E52_E6DD_4577_8BAD_A6A06BF9C28B__INCLUDED_)