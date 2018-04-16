#ifndef HistoryViewerGUI_CPP
#define HistoryViewerGUI_CPP

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/history_viewer/src/HistoryViewerGUI.cpp $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the main control class.
  *
  */

#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/HistoryViewerGUI.h"
#include "app/history/history_viewer/src/HistoryViewerDlg.h"
#include "app/history/history_viewer/src/globals.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/DataException.h"

#include "core/data_access_interface/entity_access/src/HistoryViewerData.h"

#include "bus/application_types/src/AppTypes.h"

#include "bus/user_settings/src/SettingsMgr.h"

using TA_Core::DebugUtil;
using TA_Core::RunParams;
using TA_Core::UserSettingsException;
using TA_Bus::SettingsMgr;
using TA_Core::GenericGUIException;
using TA_Core::HistoryViewerData;
using TA_Core::DataException;


namespace TA_App
{

    //
    // HistoryViewerGUI
    //
    HistoryViewerGUI::HistoryViewerGUI()
    {
    }


    //
    // ~HistoryViewerGUI
    //
    HistoryViewerGUI::~HistoryViewerGUI()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "~HistoryViewerGUI" );

        try
        {
            CWnd* application = getApplicationWnd();
            if (application != NULL)
            {
                delete application;
            }
        }
        catch( ... )
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
                "An error occurred upon shutdown");
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "~HistoryViewerGUI" );
    }


    //
    // createApplicationObject
    //
    void HistoryViewerGUI::createApplicationObject()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "createApplicationObject" );

        //m_application = new CHistoryViewerDlg(this);
        setApplicationWnd(*(new CHistoryViewerDlg(this)));

        LOG ( SourceInfo, DebugUtil::FunctionExit, "createApplicationObject" );
    }


    //
    // loadAllUserSettings
    //
    void HistoryViewerGUI::loadAllUserSettings()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "loadAllUserSettings" );

        TA_Bus::SettingsMgr* userSettings = getUserSetting();
        if (userSettings == NULL)
        {
            setUserSetting(*(new TA_Bus::SettingsMgr(getSessionId(), getApplicationType())));
        }

        // Restore any other settings here
        dynamic_cast<CHistoryViewerDlg*>(getApplicationWnd())->loadUserSettings(*getUserSetting());

        if (RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
        {
            setWindowPosition();
            LOG( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );            
            return;
        }
         
        loadPositionSettings();

        LOG ( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );
    }


    //
    // saveAllUserSettings
    //
    void HistoryViewerGUI::saveAllUserSettings()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "saveAllUserSettings" );
 
        TA_Bus::SettingsMgr* userSettings = getUserSetting();
        if (userSettings == NULL)
        {
            setUserSetting(*(new SettingsMgr(getSessionId(), getApplicationType())));
        }

        // Save any other settings here
        std::map<std::string, std::string> userSettingsMap;
        CWnd* application = getApplicationWnd();
        dynamic_cast<CHistoryViewerDlg*>(application)->saveUserSettings(userSettingsMap);

        std::map<std::string, std::string>::iterator it = userSettingsMap.begin();
        for(; it != userSettingsMap.end(); ++it)
        {
            userSettings->saveSetting(it->first, it->second);
        }

        if (RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
        {
            return;
        }
        
        savePositionSettings();

        LOG( SourceInfo, DebugUtil::FunctionExit, "saveAllUserSettings" );
    }


    //
    // getApplicationType
    //
    unsigned long HistoryViewerGUI::getApplicationType()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getApplicationType" );

        LOG( SourceInfo, DebugUtil::FunctionExit, "getApplicationType" );

        return TREND_VIEWER_GUI_APPTYPE;
    }

    
    //
    // checkEntity
    //
    void HistoryViewerGUI::checkEntity(TA_Core::IEntityData* guiEntity)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "checkEntity" );
        
        char buffer[500];

        //
        // Retrieve entity key and save as run param
        //
        unsigned long entitykey = guiEntity->getKey();
        sprintf(buffer, "%lu", entitykey);
        RunParams::getInstance().set(RPARAM_ENTITY_KEY, buffer);

        HistoryViewerData* historyViewerData = dynamic_cast<HistoryViewerData*> (guiEntity);
        
        //
        // Graph Refresh Interval
        //
        int graphRefreshRate = 0;
        try
        {
            graphRefreshRate = historyViewerData->getGraphRefreshInterval();
        }
        catch(DataException& ex)
        {
            //
            // There was a problem retrieving the value, set to default
            //
            graphRefreshRate = CFG_GRAPH_REFRESH_RATE_DEFAULT;

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "An exception was caught "
                "while retrieving GraphRefreshInterval.\nDetails: %s\nGraph Refresh Interval set "
                "to default value of: %d", ex.what(), graphRefreshRate);
        }

        if(graphRefreshRate < CFG_GRAPH_REFRESH_RATE_MIN)
        {
            sprintf(buffer, "Invalid configuration parameter: GraphRefreshInterval. "
                "Minimum allowed: %d", CFG_GRAPH_REFRESH_RATE_MIN);

            TA_THROW( GenericGUIException(buffer, GenericGUIException::ENTITY_CONFIGURATION_INVALID ));
        }
        else
        { 
            //
            // Set as a Run Param
            //
            sprintf(buffer, "%d", graphRefreshRate);
            RunParams::getInstance().set(RPARAM_GRAPH_REFRESH_INTERVAL, buffer);

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Setting GraphRefreshInterval to: %d", graphRefreshRate);
        }

        //
        // Max Trends Displayed
        //
        int maxTrendsDisplayed = 0;
        try
        {
            maxTrendsDisplayed = historyViewerData->getMaxTrendsDisplayed();
        }
        catch(DataException& ex)
        {
            //
            // There was a problem retrieving the value, set to default
            //
            maxTrendsDisplayed = CFG_MAX_TRENDS_DISPLAYED_DEFAULT;

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "An exception was caught "
                "while retrieving GraphRefreshInterval.\nDetails: %s\nGraph Refresh Interval set "
                "to default value of: %d", ex.what(), maxTrendsDisplayed);
        }

        if(maxTrendsDisplayed <= 0)
        {
            TA_THROW( GenericGUIException("Invalid configuration parameter: MaxTrendsDisplayed. "
                "Must be greater than 0", GenericGUIException::ENTITY_CONFIGURATION_INVALID ));
        }
        else if(maxTrendsDisplayed > CFG_MAX_TRENDS_DISPLAYED_MAX)
        {
            sprintf(buffer, "Invalid configuration parameter: MaxTrendsDisplayed. "
                "Maximum allowed: %d", CFG_MAX_TRENDS_DISPLAYED_MAX);

            TA_THROW( GenericGUIException(buffer, GenericGUIException::ENTITY_CONFIGURATION_INVALID ));
        }
        else
        { 
            //
            // Set as a Run Param
            //
            sprintf(buffer, "%d", maxTrendsDisplayed);
            RunParams::getInstance().set(RPARAM_MAX_TRENDS_DISPLAYED, buffer);

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Setting MaxTrendsDisplayed to: %d", maxTrendsDisplayed);
        }

        setGuiEntity(*guiEntity);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "checkEntity" );
    }


    //
    // checkCommandLine
    //
    void HistoryViewerGUI::checkCommandLine()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "checkCommandLine" );

        //
        // DataPoints - optional
        //
        std::string dataPoints = RunParams::getInstance().get("DataPoints");
        if(!dataPoints.empty())
        {
            if(dataPoints.find_first_not_of("0123456789,") != -1)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: DataPoints", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }
        }

        //
        // GraphType - optional
        //
        std::string graphType = RunParams::getInstance().get("GraphType");
        if(!graphType.empty())
        {
            if(graphType.find_first_not_of("0123456789") != -1)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: GraphType", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }
        }

        //
        // StartTime - optional
        //
        std::string startTime = RunParams::getInstance().get("StartTime");
        if(!startTime.empty())
        {
            if(startTime.find_first_not_of("0123456789/,:") != -1)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: StartTime", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }

            int firstSlash = startTime.find_first_of("/");
            int secondSlash = startTime.find_first_of("/", firstSlash+1);
            if(secondSlash != firstSlash+2 || secondSlash != firstSlash+3)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: StartTime", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }

            int comma = startTime.find_first_of(",");
            if(comma != secondSlash+5)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: StartTime", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }

            int colon = startTime.find_first_of(":");
            if(colon != comma+3 || colon != comma+2)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: StartTime", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }

            if(startTime.size() != static_cast<unsigned int>(colon)+3)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: StartTime", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "checkCommandLine" );
    }


    //
    // entityChanged
    //
    void HistoryViewerGUI::entityChanged(const std::vector<std::string>& changes)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "entityChanged" );

        // 
        // Only care about changes to the GraphRefreshInterval parameter
        //
        std::vector<std::string>::const_iterator it = changes.begin();
        for(; it != changes.end(); ++it)
        {
            if(*it == RPARAM_GRAPH_REFRESH_INTERVAL)
            {
                dynamic_cast <CHistoryViewerDlg*> (getApplicationWnd())->serverDown();
            }
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "entityChanged" );
    }


    //
    // serverIsDown
    //
    void HistoryViewerGUI::serverIsDown()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "serverIsDown" );

        // 
        // Get the GUI to update to indicate this.
        //
        dynamic_cast <CHistoryViewerDlg*> (getApplicationWnd())->serverDown();

        LOG ( SourceInfo, DebugUtil::FunctionExit, "serverIsDown" );
    }


    //
    // serverIsUp
    //
    void HistoryViewerGUI::serverIsUp()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "serverIsUp" );

        // 
        // Get the GUI to update to indicate this.
        //
        dynamic_cast <CHistoryViewerDlg*> (getApplicationWnd())->serverUp();
        LOG ( SourceInfo, DebugUtil::FunctionExit, "serverIsUp" );
    }


    //
    // prepareForClose
    //
   void HistoryViewerGUI::prepareForClose()
   {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "prepareForClose" );

        try
        {
            saveAllUserSettings();
        }
        catch ( UserSettingsException& )
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                "Could not save user settings.");
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "prepareForClose" );
    }


}   // end namespace TA_App

#endif // HistoryViewerGUI_CPP