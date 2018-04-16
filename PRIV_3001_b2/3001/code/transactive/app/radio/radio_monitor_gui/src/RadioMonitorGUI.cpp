/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/src/RadioMonitorGUI.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Implements required functionality of GenericGUI framework, performs
  *  initialisation of singletons
  *
  */


#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_xVER

#include "app/radio/radio_monitor_gui/src/stdafx.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitor.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorGUI.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorDlg.h"
#include "app/radio/radio_monitor_gui/src/CachedConfig.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "core/utilities/src/RunParams.h"
#include "bus/application_types/src/apptypes.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"

#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioDirectoryEntityData.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/RadioSessionEntityData.h"
#include "core/exceptions/src/DataException.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::SettingsMgr;

namespace TA_IRS_App
{    
    /////////////////////////////////////////////////////////////////////////////
    // RadioMonitorGUI
    RadioMonitorGUI::RadioMonitorGUI()
    : 
    AbstractDialogGUI(IDD_RADIOMONITOR_DIALOG),
    m_radioMonitorDlg(NULL),
    m_commandLineInvalid(false)
    {
    
    }
    
    
    void RadioMonitorGUI::createApplicationObject()
    {
        if (!m_radioMonitorDlg)
        {
            if (m_commandLineInvalid)
            {
                // Create a dummy dialog, with empty DoModal()
                // implementation to allow rapid shutdown - this
                // is only done as no way to exit GenericGUI quickly+silently
                // when the command line fails
                m_radioMonitorDlg = new NullDialog();
            }
            else
            {
                m_radioMonitorDlg = new RadioMonitorDlg(*this);
            }
            setApplicationWnd(*m_radioMonitorDlg); 
        }    
    }
    
    RadioMonitorGUI::~RadioMonitorGUI()
    {
        try
        {
            if (m_radioMonitorDlg)
            {
                delete m_radioMonitorDlg;
                m_radioMonitorDlg = NULL;
            }
        }
        catch (...)
        {
             LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }
    
    unsigned long RadioMonitorGUI::getApplicationType()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getApplicationType" );
        
        LOG( SourceInfo, DebugUtil::FunctionExit, "getApplicationType" );
      
        return PA_MANAGER_GUI_APPTYPE;
    	
    }
    
    void RadioMonitorGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        if (m_commandLineInvalid)
        {
            return;
        }
        LOG( SourceInfo, DebugUtil::FunctionEntry, "checkEntity" );
    
    	setGuiEntity(*guiEntity);
    
        // Note that the following can throw InvalidPAConfigurationException
        // which will be handled by the GenericGUI framework
    
        // This information is set up in checkCommandLine
        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

        try
        {

            std::auto_ptr<TA_Base_Core::IConsole> console(
                    TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId));
        	
            TA_ASSERT (0 != console.get(), "Should have thrown exception");
            
            unsigned long physicalLocationKey = console->getLocation(); 
            CachedConfig::getInstance().setPhysicalLocationKey(physicalLocationKey);
        }
        catch (...)
        {
            if (stricmp(sessionId.c_str(), "debug") != 0)
            {
                LOG(SourceInfo,     
                    DebugUtil::GenericLog,  
                    DebugUtil::DebugError, 
                    "Unable to retrieve console from sessionID");
        
                // Unable to determine physical location
                TA_THROW(TA_Base_Core::GenericGUIException(TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID));
            }
            else
            {
                // In debug mode, go for a physical location defined by locationkey runparam
               unsigned long physicalLocationKey = atol(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
               CachedConfig::getInstance().setPhysicalLocationKey(physicalLocationKey);
            }
        }
        
        // Attach to agents at the location user has logged in for (contained in RunParams) - this
        // is not to be confused with the physical location
        unsigned long locationKey = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());    

        CachedConfig::getInstance().setEntityKey(guiEntity->getKey());
        CachedConfig::getInstance().setEntityName(guiEntity->getName());
        CachedConfig::getInstance().setEntityTypeKey(guiEntity->getTypeKey());
        CachedConfig::getInstance().setEntityLocationKey(locationKey);
        CachedConfig::getInstance().setEntitySubsystemKey(guiEntity->getSubsystem());
        
        CachedConfig::getInstance().setSessionId(sessionId.c_str());
        
        {
            TA_Base_Core::IEntityDataList entityArray = 
                EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( RadioEntityData::getStaticType(), locationKey );
            TA_ASSERT(entityArray.size() == 1, "There can be only one Entity NAME at specified location");
        
            CachedConfig::getInstance().setLocalRadioAgentEntityName(entityArray[0]->getName());
            CachedConfig::getInstance().setLocalRadioAgentEntityKey(entityArray[0]->getKey());

            deleteAllItemsInArray(entityArray);
        }
            
        {
            /*
            TA_Base_Core::IEntityDataList entityArray = 
                EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( RadioDirectoryEntityData::getStaticType(), locationKey );
                */
            // Directory Agent only exists at OCC
            TA_Base_Core::IEntityDataList entityArray = 
                EntityAccessFactory::getInstance().getEntitiesOfType( RadioDirectoryEntityData::getStaticType() );
            TA_ASSERT(entityArray.size() == 1, "There can be only one Entity NAME at specified location");
        
            CachedConfig::getInstance().setLocalRadioDirectoryAgentEntityName(entityArray[0]->getName());
            deleteAllItemsInArray(entityArray);
        }

        {
            TA_Base_Core::IEntityDataList radioEntityArray = 
                EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( RadioEntityData::getStaticType(), locationKey  );
            TA_ASSERT(radioEntityArray.size() == 1, "There can be only one Entity NAME at specified location");
        
            CachedConfig::getInstance().setLocalRadioEntityName(radioEntityArray[0]->getName());
            deleteAllItemsInArray(radioEntityArray);
        }

        {
            try
            {
                std::string radioSessionEntityName = getRadioSessionEntityName(locationKey, getConsoleKey());
                CachedConfig::getInstance().setLocalRadioSessionAgentEntityName(radioSessionEntityName);
            }
            catch (const TA_Base_Core::DataException&)
            {
                // Occurs when fail to identify console or other MFT specific information
                // just run without session agent..
            }
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "checkEntity" );
    }

    
    void RadioMonitorGUI::checkCommandLine()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "checkCommandLine" );        
        LOG( SourceInfo, DebugUtil::FunctionExit, "checkCommandLine" );
    }
    
    
    void RadioMonitorGUI::entityChanged(const std::vector<std::string>& changes)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "entityChanged" );
        
        // TODO: Check the changes passed and pass any relevant information on.
    
        LOG( SourceInfo, DebugUtil::FunctionExit, "entityChanged" );
    }
    
    void RadioMonitorGUI::serverIsDown()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "serverIsDown" );
    
        // TODO: Get the GUI to update to indicate this.
    
        LOG( SourceInfo, DebugUtil::FunctionExit, "serverIsDown" );
    }
    
    void RadioMonitorGUI::serverIsUp()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "serverIsUp" );
    
        // TODO: Get the GUI to update to indicate this.
    
        LOG( SourceInfo, DebugUtil::FunctionExit, "serverIsUp" );
    }
    
    void RadioMonitorGUI::loadAllUserSettings()
    {
        // First check if we should be loading the user settings.
        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
        {
           setWindowPosition();
           LOG( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );            
           return;
        }
    
        loadPositionSettings();
    }
    
    
    void RadioMonitorGUI::saveAllUserSettings()
    {
        // First check if we should be saving the user settings.
        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
        {
            return;
        }
    
        savePositionSettings();
    }
    
    
    void RadioMonitorGUI::prepareForClose()
    {
    	//TODO: This provides a standard method for each GUI to use to clean itself 
    	//up ready to exit. This will be called whenever the application exits whether 
    	//it is closed by the Control Station or by the user. This should perform 
    	//tasks such as saving user settings, stopping any threads and asking the user 
    	//if they want to save their settings. When this method is completed the GUI 
    	//should be in a safe state to be closed.
    
        try
        {
            saveAllUserSettings();
        }
        catch ( TA_Base_Core::UserSettingsException&)
        {
            // Handle here with appropriate errror message
        }
    
        // TODO: Perform any shutdown tasks here
    }
    
    
    // Code taken from STISManager ---    
    void RadioMonitorGUI::changeFocus(const TA_Base_Core::EFocusType focus)
    {
    	// We only want to handle the change of focus
    	// if the application is not currently hidden,
    	// ie the operator is still making PID selections,
    	// and has not pressed the 'Launch STIS Manager' button
    	
    	if( AbstractGUIApplication::getApplicationWnd() == NULL )
        {
            // Too early
            return;
        }
    
        RadioMonitorDlg* dlg = dynamic_cast<RadioMonitorDlg*>(AbstractGUIApplication::getApplicationWnd());
        TA_ASSERT(dlg, "dlg is not a RadioMonitorDlg");
        if ( dlg->wantToShow() )
        {
    	    AbstractGUIApplication::changeFocus( focus );
    	    
        }
    }


    unsigned long RadioMonitorGUI::getConsoleKey()
    {
     	FUNCTION_ENTRY("getConsoleKey");
    	unsigned long consoleKey;
    
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        TA_ASSERT(!sessionId.empty(), "SessionId parameter is not set");
    	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
    	consoleKey = console->getKey();
    	delete console;
    
    	FUNCTION_EXIT;
    	return consoleKey;
    }

    
    std::string RadioMonitorGUI::getRadioSessionEntityName(unsigned long locationKey,unsigned long consoleKey)
    {
     	FUNCTION_ENTRY("getRadioSessionEntityName");
    	std::vector<TA_Base_Core::IEntityData*> entityList;
    	TA_Base_Core::IRadioSessionEntityData*  radioSessionEntity;
    	std::string   radioSessionEntityName;
    	bool entityFound = false;
    
    	// Should be just one radio entity at each location
    	entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::RadioSessionEntityData::getStaticType(),locationKey);
    	TA_ASSERT(entityList.size()>0,"No radio session entities found for this location.");
    	while (!entityList.empty())
    	{
    		radioSessionEntity = dynamic_cast<TA_Base_Core::IRadioSessionEntityData*>(entityList[0]);
    		TA_ASSERT(radioSessionEntity,"Invalid entity type returned from RasioSession search.");
    		if (radioSessionEntity->getAssociatedConsoleKey()==consoleKey)
    		{
    			if (entityFound)
    			{
    				TA_ASSERT(false,"Multiple radio sessions associated with this console.");
    			}
    			else
    			{
    				radioSessionEntityName = radioSessionEntity->getName();
    				entityFound = true;
    			}
    		}
    
    		entityList.erase(entityList.begin());
    
    		delete radioSessionEntity;
    	}
    	TA_ASSERT(entityFound,"Radio session not found for this console.");
    
    	FUNCTION_EXIT;
    	return radioSessionEntityName;
    }

    void RadioMonitorGUI::onInitGenericGUICompleted()
    {
        //TODO:
    }

} // end namespace TA_IRS_App
