/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/PAManagerGUI.cpp $
  * @author:  Ripple
  * @version: $Revision: #9 $
  *
  * Last modification: $DateTime: 2014/06/02 15:38:46 $
  * Last modified by:  $Author: huangjian $
  *
  */
// PAManagerGUI.cpp : implementation file
//

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_VER

#include "app/pa/PAManager/src/stdafx.h"

#include "app/pa/PAManager/src/PAManagerGUI.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/CachedMapLoader.h"
#include "app/pa/PAManager/src/GraphworxComms.h"
#include "app/pa/PAManager/src/PAmanager.h"
#include "app/pa/PAManager/src/PAManagerDlg.h"
#include "app/pa/PAManager/src/RunParamListener.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// PAManagerGUI

PAManagerGUI::PAManagerGUI() :
AbstractDialogGUI( IDD_MAIN_DLG ),
m_PAManagerDlg(NULL),
m_commandLineInvalid(false)
{
}


void PAManagerGUI::createApplicationObject()
{
    if (!m_PAManagerDlg)
    {
        if (m_commandLineInvalid)
        {
            // Create a dummy dialog, with empty DoModal()
            // implementation to allow rapid shutdown - this
            // is only done as no way to exit GenericGUI quickly+silently
            // when the command line fails
            m_PAManagerDlg = new NullDialog();
        }
        else
        {
            m_PAManagerDlg = new PAManagerDlg(*this);
        }
        setApplicationWnd(*m_PAManagerDlg); 
    }    
}


PAManagerGUI::~PAManagerGUI()
{
    try
    {
        if (m_PAManagerDlg)
        {
            delete m_PAManagerDlg;
            m_PAManagerDlg = NULL;
        }
    }
    catch (...)
    {
         LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
    }
}


unsigned long PAManagerGUI::getApplicationType()
{
    FUNCTION_ENTRY( "getApplicationType" );
    FUNCTION_EXIT;  
    return PA_MANAGER_GUI_APPTYPE;
}


void PAManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
{
    if (m_commandLineInvalid)
    {
        return;
    }
    FUNCTION_ENTRY( "checkEntity" );

	setGuiEntity(*guiEntity);

    // Note that the following can throw InvalidPAConfigurationException
    // which will be handled by the GenericGUI framework

    // This information is set up in checkCommandLine
    unsigned long locationKey = CachedConfig::getInstance()->getAgentLocKey();

    CachedConfig::getInstance()->setEntityKey(guiEntity->getKey());
    CachedConfig::getInstance()->setSessionId(RunParams::getInstance().get(RPARAM_SESSIONID).c_str());

    std::string entityName;
	TA_Base_Core::ILocation::ELocationType eLocationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(locationKey);

	if (TA_Base_Core::ILocation::OCC == eLocationType)
    {
        entityName = MASTER_PA_AGENT_ENTITY_TYPE_NAME;
    }
    else
    {
        entityName = STATION_PA_AGENT_ENTITY_TYPE_NAME;
    }

    std::vector<CorbaName> allNames =
                EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(entityName, locationKey,true);   
	if (1 != allNames.size())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There can be only one CORBA NAME at specified location" );
		TA_THROW(TA_Base_Core::DataException("There can be only one CORBA NAME at specified location",TA_Base_Core::DataException::INVALID_VALUE,"Corba name"));
	}
    CachedConfig::getInstance()->setPAAgentCorbaName( allNames[0] );

    try
    {
        // Perform initial refresh of cached maps
        CachedMapLoader::loadAllCachedMaps();
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        // Error reading data from database
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Database initial read error", ex.what());
        throw;
    }
    catch (const std::exception& se)
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Internal error reading database", se.what());
        throw;
    }
    catch (...)
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Internal error reading database", "Unknown");
        throw;
    }

    FUNCTION_EXIT;
}


void PAManagerGUI::checkCommandLine()
{
    FUNCTION_ENTRY( "checkCommandLine" );

    // On startup - deactivate all the graphworx displays (until startup complete)
    TA_App::GraphworxComms::getInstance().resetGraphworxDisplaysToInactive();

    // Must collect location data from database - to set up initial location parameters    
	TA_IRS_Bus::CachedMaps::getInstance()->refreshLocationKeyToNameMap();

    ////////////////////////////////////////////
    // Perform early setup of location key information, to allow checking of command line
    // Note there are three locations:  The physical location
    // key is hardwired, and will always be the same for a given machine
    // The virtual location key depends on which schematic we were launched from

    // The LocationKey runparam (RPARAM_LOCATIONKEY) doesn't represent
    // either of these (represents non-schematic related location), and hence shouldn't be used.
    // The physical location can be taken from the console
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

    try
    {
        std::auto_ptr<TA_Base_Core::IConsole> console(
                TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId));

		if (0 == console.get())
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Should have thrown exception" );

			FUNCTION_EXIT;
			TA_THROW(TA_Base_Core::TransactiveException( "Console is null" ));
		}

        unsigned long ulMftLocKey = console->getLocation(); 
        CachedConfig::getInstance()->setMftLocKey( ulMftLocKey );
    }
    catch (...)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to retrieve console from sessionID");

        // Unable to determine physical location
        TA_THROW(TA_Base_Core::GenericGUIException("Unable to retrieve console from sessionID", 
            TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID));
    }

    std::string commandLineLocationName = RunParamListener::getInstance().findVirtualLocation();

    unsigned long ulSchematicLocKey = 0;
    unsigned long ulAgentKey = 0;

    try
    {
        // The command line location name (passed in from schematics) identifies what location
        // the schematics is running for (or the 'virtual' as opposed to physical location)
        if (0 == commandLineLocationName.compare("LIN"))
        {
            // Special keyword exists for the 'Overall' schematic, as it is not a proper location
            ulSchematicLocKey = CachedConfig::getInstance()->getOccLocationKey();
        }
        else
        {
            ulSchematicLocKey = TA_IRS_Bus::CachedMaps::getInstance()->getLocationKeyFromName(commandLineLocationName);
        }

        CachedConfig::getInstance()->setSchematicLocKey( ulSchematicLocKey );

        if (CachedConfig::getInstance()->isAtOcc())
        {
            // Whenever we're at the OCC terminal, we leave our physical location as OCC (so we
            // continue to communicate with the OCC agent - NOT the stations agent)
            ulAgentKey = CachedConfig::getInstance()->getMftLocKey();
        }
        else
        {
            ulAgentKey = CachedConfig::getInstance()->getSchematicLocKey();
            // If we're not at the OCC, must be station or depot
            // The station / depot can operate for any other location - except for overall
            // (that is, they can't communicate with the OCC agent)
			TA_Base_Core::ILocation::ELocationType eLocationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(ulSchematicLocKey);
			if ( TA_Base_Core::ILocation::OCC == eLocationType )
            {
                // A station cannot operate on the overall schematic
                PaErrorHandler::displayModalError(PaErrorHandler::STATION_CANNOT_RUN_FOR_OVERALL);
                m_commandLineInvalid = true;
                TA_THROW(TA_Base_Core::GenericGUIException(TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID));
            }
        }

        // We may need to run PA Manager for a foreign station - in which case locationKey
        // won't match physicalLocationKey (locationKey defines station we're operating for)
        // - any restrictions in rights due to a mismatched location key need to be
        // implemented in the PaRightsManager class
        CachedConfig::getInstance()->setAgentLocKey(ulAgentKey);    
    }
    catch (const TA_Base_Core::CachedMappingNotFoundException&)
    {
        std::ostringstream reason;
        reason << "Could not find location key for location specified in command line: " << commandLineLocationName;

        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, reason.str().c_str());

        // Couldn't find the specified location in the database - must be an invalid command
        TA_THROW(TA_Base_Core::GenericGUIException(TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID));
    }

	if (TA_Base_Core::DatabaseKey::isInvalidKey(ulSchematicLocKey))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid location key value" );

		TA_THROW(TA_Base_Core::GenericGUIException("Invalid location key value", TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID));
	}

    
    // The command line should have specified an initial runparam 
    // that can be used to identify this instance
    if (!RunParamListener::getInstance().retrieveInitialRunParamIdentity())
    {
        // This is required since when we throw an exception from here
        // All the core threading functionality is killed off - and
        // cleanupSingletons will have issued if called after this..
        PAManagerApp::cleanupSingletons();
        
        // TD #2593: Removed as per request (so only single error displayed)
        //TA_THROW(TA_Base_Core::GenericGUIException(
        //            TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID));

        // Display our error in a modal dialog (the only feedback user will get before shutdown)
        PaErrorHandler::displayModalError(PaErrorHandler::NO_EXTERNAL_PA_INTERACTION_ALLOWED);

        m_commandLineInvalid = true;
    }

    FUNCTION_EXIT;
}


void PAManagerGUI::entityChanged(const std::vector<std::string>& changes)
{
    FUNCTION_ENTRY( "entityChanged" );
    
    // TODO: Check the changes passed and pass any relevant information on.

    FUNCTION_EXIT;
}


void PAManagerGUI::serverIsDown()
{
    FUNCTION_ENTRY( "serverIsDown" );

    // TODO: Get the GUI to update to indicate this.

    FUNCTION_EXIT;
}


void PAManagerGUI::serverIsUp()
{
    FUNCTION_ENTRY( "serverIsUp" );

    // TODO: Get the GUI to update to indicate this.

    FUNCTION_EXIT;
}


void PAManagerGUI::loadAllUserSettings()
{
    FUNCTION_ENTRY( "loadAllUserSettings" );

    // First check if we should be loading the user settings.
    if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
    {
       setWindowPosition();
       FUNCTION_EXIT;
       return;
    }

    loadPositionSettings();

    FUNCTION_EXIT;
}


void PAManagerGUI::saveAllUserSettings()
{
    // First check if we should be saving the user settings.
    if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
    {
        return;
    }

    savePositionSettings();
}


void PAManagerGUI::prepareForClose()
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

void PAManagerGUI::onInitGenericGUICompleted()
{
	CWaitCursor cur;
	PAManagerDlg * pDlg = dynamic_cast<PAManagerDlg * >(getApplicationWnd());
	if (NULL == pDlg)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "the main dialog is NULL!" );
		return;
	}
	pDlg->LockWindowUpdate();

    try
    {
        pDlg->init();
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        throw;
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unknown exception caught in PAManagerDlg::init" );
        throw;
    }

	pDlg->UnlockWindowUpdate();
	pDlg->UpdateWindow();
	cur.Restore();
}

