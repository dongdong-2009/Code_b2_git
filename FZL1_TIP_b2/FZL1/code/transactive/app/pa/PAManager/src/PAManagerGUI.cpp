/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// PAManagerGUI.cpp : implementation file
//

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_VER

#include "app/pa/PAManager/src/stdafx.h"


#include "bus/application_types/src/apptypes.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "app/pa/PAManager/src/PaAgentInterface.h"
#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/RunParamListener.h"
#include "app/pa/PAManager/src/PAManagerGUI.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/CachedMapLoader.h"
#include "app/pa/PAManager/src/SchematicDecideModel.h"
#include "app/pa/PAManager/src/VariableConvertor.h"
#include "app/pa/PAManager/src/PAManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// PAManagerGUI

PAManagerGUI::PAManagerGUI() : AbstractDialogGUI( IDD_DLG_PA_MAIN_WND ),
m_pPAManagerDlg(NULL),
m_commandLineInvalid(false)
{
}


void PAManagerGUI::createApplicationObject()
{
    if (!m_pPAManagerDlg)
    {
        if (m_commandLineInvalid)
        {
            // Create a dummy dialog, with empty DoModal()
            // implementation to allow rapid shutdown - this
            // is only done as no way to exit GenericGUI quickly+silently
            // when the command line fails
            m_pPAManagerDlg = new NullDialog();
        }
        else
        {
            m_pPAManagerDlg = new PAManagerDlg(*this);
        }
        setApplicationWnd(*m_pPAManagerDlg); 
    }
}


PAManagerGUI::~PAManagerGUI()
{
    try
    {
        if (NULL != m_pPAManagerDlg)
        {
            delete m_pPAManagerDlg;
            m_pPAManagerDlg = NULL;
        }

        PaAgentInterface::removeCorbaObject();
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

    if ( NULL == guiEntity )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
    }
    else
    {
        CachedConfig::getInstance()->setEntityKey(guiEntity->getKey());
        CachedConfig::getInstance()->setEntityName(guiEntity->getName());
        CachedConfig::getInstance()->setEntitySubsystemKey(guiEntity->getSubsystem());
        CachedConfig::getInstance()->setEntityTypeKey(guiEntity->getTypeKey());
    }

	unsigned long locationKey = CachedConfig::getInstance()->getPhysicalLocationKey();
	SchematicDecideModel::setSchematicLocationKey( locationKey );
	SchematicDecideModel::setSubsystemKey( CachedConfig::getInstance()->getEntitySubsystemKey() );

    PaDataManager::getInstance().updateRunParams();
    std::string strPaAgentCorbaName = "";

    try
    {
        unsigned long locationKey = CachedConfig::getInstance()->getEntityLocationKey();
        std::string entityName = "";
        if (locationKey == TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey())
        {
            entityName = MASTER_PA_AGENT_ENTITY_TYPE_NAME;
        }
        else
        {
            entityName = STATION_PA_AGENT_ENTITY_TYPE_NAME;
        }

        TA_Base_Core::IEntityDataList entityArray = 
            TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( entityName, locationKey );
        TA_ASSERT(entityArray.size() == 1, "There can be only one Entity NAME at specified location");
    
        strPaAgentCorbaName = entityArray[0]->getName();
        TA_IRS_Bus::deleteAllItemsInArray(entityArray);
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
    }

    PaAgentInterface::createCorbaObject( strPaAgentCorbaName );

    CachedMapLoader::loadAllCachedMaps( CachedConfig::getInstance()->getEntityLocationKey(), CachedConfig::getInstance()->isAtOcc() );

    FUNCTION_EXIT;
}


void PAManagerGUI::checkCommandLine()
{
    FUNCTION_ENTRY( "checkCommandLine" );

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

    std::auto_ptr<TA_Base_Core::IConsole> console(NULL);

    try
    {
        console.reset( TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId) );

        TA_ASSERT (NULL != console.get(), "Should have thrown exception");
    }
    catch (...)
    {
        LOG(SourceInfo,     
            TA_Base_Core::DebugUtil::GenericLog,  
            TA_Base_Core::DebugUtil::DebugError, 
            "Unable to retrieve console from sessionID");

        m_commandLineInvalid = true;
        // Unable to determine physical location
        TA_THROW(TA_Base_Core::GenericGUIException(TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID));
    }


    try
    {
        CachedConfig::getInstance()->setPhysicalLocationKey( console->getLocation() );
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Unable to retrieve location from console");
        m_commandLineInvalid = true;
        // Unable to determine physical location
        TA_THROW(TA_Base_Core::GenericGUIException(TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID));
    }

    try
    {
        CachedConfig::getInstance()->setPABackEntityKey( console->getPABackEntityKey() );
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Unable to retrieve back entity key from console");
    }

    unsigned long locationKey = CachedConfig::getInstance()->getPhysicalLocationKey();

    // Update the location key to SchematicDecideModel
    //SchematicDecideModel::setSchematicLocationKey( locationKey );
    //SchematicDecideModel::setSubsystemKey( CachedConfig::getInstance()->getEntitySubsystemKey() );

    CachedConfig::getInstance()->setEntityLocationKey(locationKey);

    // Determine special location flags:
    if (locationKey == TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey())
    {
        CachedConfig::getInstance()->setLocationType(CachedConfig::LOCATION_TYPE_OCC);
    }
    else
    {
        // Currently, we do not need to identify whether the start location is deport or not , so we comments the following codes.
        //if (locationKey == TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey())
        //{
        //    CachedConfig::getInstance()->setLocationType(CachedConfig::LOCATION_TYPE_DEPOT);
        //    TA_IRS_Bus::CachedMaps::getInstance()->setOccDepotFlags(false, true);
        //}
        //else
        {
            // Station
            CachedConfig::getInstance()->setLocationType(CachedConfig::LOCATION_TYPE_STATION);
        }   
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
    if ( !TA_Base_Core::RunParams::getInstance().isSet(RPARAM_USERPREFERENCES) )
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
    if ( !TA_Base_Core::RunParams::getInstance().isSet(RPARAM_USERPREFERENCES) )
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
	PAManagerDlg * pDlg = dynamic_cast<PAManagerDlg *>(getApplicationWnd());
	TA_ASSERT(pDlg != NULL, "the main dialog is NULL!");
	pDlg->LockWindowUpdate();
	pDlg->init();
	pDlg->UnlockWindowUpdate();
	pDlg->UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
// PAManagerGUI message handlers
