/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/PidSelectionManager.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2017/11/10 17:22:53 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Manages the PID list and PID groups
  */

#include "stdafx.h"
#include "PidSelectionManager.h"
#include "CreateNewGroupDlg.h"
#include "UserMessages.h"

#include "core/utilities/src/TAAssert.h"    // TD9158

#include "app/signs/stis_manager/src/GraphworxComms.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 

#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"

#include "core/synchronisation/src/ThreadGuard.h"

#include "boost/tokenizer.hpp"

#include "core/data_access_interface/tis_agent/src/PidGroupsAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/IPidGroup.h"
#include "core/message/src/MessagePublicationManager.h"

// TD9158 ~ added the following...
#include "core/message/types/ConfigUpdate_MessageTypes.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h" 
#include "LocationsManager.h"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <algorithm>
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace boost::lambda;

namespace TA_IRS_App
{

	volatile int PidSelectionManager::DONTGOFRONT_TAG = 0;
	ACE_Thread_Mutex PidSelectionManager::Tag_mutex;
    PidSelectionManager::PidSelectionManager( PidListCtrl& pidListCtrl,
                                              PidGroupCombo& pidGroupCombo,
                                              CallbackButton& saveButton,
                                              CallbackButton& deleteButton,
                                              IPidSelectionListener& pidSelectionListener)
        : m_pidGroupMap(),
          m_activeGroupName(PidGroupCombo::HMIGroupName),
          m_pidListCtrl(pidListCtrl),
          m_pidGroupCombo(pidGroupCombo),
          m_saveButton(saveButton),
          m_deleteButton(deleteButton),
          m_pidSelectionListener(pidSelectionListener),
          m_canUsePidGroups(false),
		  m_currentLocation( "" ),
		  m_dbOk(false),
		  m_schematicID( 99999 ),
          m_auditMessageSender(NULL),
          m_configUpdateSender(NULL) // TD9158
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"PID Selection Manager constructor");
        // set the default group
        m_activeGroupName = PidGroupCombo::HMIGroupName;

        m_saveButton.EnableWindow(m_canUsePidGroups);
        m_deleteButton.EnableWindow(m_canUsePidGroups);
    }

	void PidSelectionManager::Init()
	{
		//from here to using another fun to init
        // build location map for sorting
        initialiseLocationInfo();
		
        // child controls
        m_pidListCtrl.setPidSelectionManager(this);
        m_pidGroupCombo.setPidSelectionManager(this);
        m_saveButton.setCallback(this);
        m_deleteButton.setCallback(this);
		
        // add the default HMI pid group
        m_pidGroupMap[PidGroupCombo::HMIGroupName];
		
		//read and add the saved groups in the param to the m_pidGroupMap data member
		readAndAddGroups( ); //to change...
		
        // Register for the PID selection/deselection 
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, "Select");	
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, "Deselect");	
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, "SynchroniseSelected");
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, "Display");
		
		// This is redundant unless the STISManager crashes after selections have already been made
		TA_IRS_App::GraphworxComms::getInstance().resetGraphworxDisplaysToInactive();
		
		std::string displayValue = TA_Base_Core::RunParams::getInstance().get("Display");
		if( displayValue != "")
        {
            onRunParamChange( "Display", displayValue);
        }
		
        m_auditMessageSender = 
			TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::TISAudit::Context );
		
		// No longer need to process these as the GUI is initialising
		// since the GUI must let the schematic know it is ready for 
		// selections/deselections via the activation method
		
        m_configUpdateSender = TA_Base_Core::MessagePublicationManager::getInstance().
            getConfigUpdateMessageSender(TA_Base_Core::ConfigUpdate::Context);  // TD9158 ~ added
		
        registerForConfigChanges(); // TD9158 ~ added
	}


    PidSelectionManager::~PidSelectionManager()
    {
        TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);	
        m_pidListCtrl.setPidSelectionManager(NULL);
        m_pidGroupCombo.setPidSelectionManager(NULL);
        m_saveButton.setCallback(NULL);
        m_deleteButton.setCallback(NULL);

        // clear all PID goups
        TA_Base_Core::ThreadGuard guard( m_pidGroupMapLock );
        for (PidGroupMap::iterator iter = m_pidGroupMap.begin();
        iter != m_pidGroupMap.end(); iter++)
        {
	        iter->second.clear();
        }

        m_pidGroupMap.clear();

        m_pidListCtrl.DeleteAllItems();

		TA_IRS_App::GraphworxComms::getInstance().resetGraphworxDisplaysToInactive();
    }


    void PidSelectionManager::setPermissions(bool canModifyPidGroups)
    {
		m_canUsePidGroups = canModifyPidGroups;
		setButtonStates();

		//if the location associated with the sessionId has changed we need to refresh the
		//data
		//return the location associated with the current session
		unsigned long newLocation = getSessionLocation();

		TA_IRS_Core::PidGroupsAccessFactory& mgr = TA_IRS_Core::PidGroupsAccessFactory::getInstance( );
		unsigned long oldLocation = mgr.getLocation();

		if( oldLocation != newLocation )
		{
			mgr.setLocation( newLocation );

			readAndAddGroups( );
		}
    }



    void PidSelectionManager::savePidGroup()
    {
		//if the data read/write was bugger last time try to read it
		if(!m_dbOk)
		{
			readAndAddGroups();

			//and make them hit the button again
			//todo this is a bit of a hack but it is the simple solution to sync
			return;
		}

        CreateNewGroupDlg dialog ( "TIS Manager",
                                   "Enter a name for the PID group",
                                   "",
                                   AfxGetMainWnd());

        int selection = dialog.DoModal();


		if(selection != IDOK) return;

		if( dialog.m_AccessFactoryFailure )
		{
			handlePidReadWriteError( "Unidentified failure occurred while saving PID group" );
			return;
		}
		

        TA_Base_Core::ThreadGuard guard( m_pidGroupMapLock );

		std::string newGroupName = dialog.getName();        

		//these are the pids that will be in the new group
		PidList pids = m_pidGroupMap[ m_activeGroupName ];

		//create a vector of the asset names of these pids:
		std::vector<std::string> pidNames;
		PidList::iterator iter = pids.begin();
		for( ; iter != pids.end(); iter++ )
		{         
			pidNames.push_back( (*iter).assetId );
		}

        TA_IRS_Core::PidGroupsAccessFactory& mgr = TA_IRS_Core::PidGroupsAccessFactory::getInstance( );

		try
		{
			//API REF:
			//void createPidGroup( const std::string& name,
			//					 std::vector<std::string> pidNames );
			mgr.createPidGroup( newGroupName, pidNames );
		}
		catch( TA_Base_Core::DatabaseException& ex )
		{
			std::string error("Failure occurred while saving PID group:\n");
			error += ex.what();

			handlePidReadWriteError( error );
			return;
		}
		catch(...)
		{
			handlePidReadWriteError( "Unidentified failure occurred while saving PID group" );
			return;
		}

		// TES831 Events not logged!
		std::string pidGroupNameStr(newGroupName);
		std::stringstream newPIDNamesStream;


		//create a vector of the asset names of these pids:
		PidList::iterator newiter = pids.begin();
		for( ; newiter != pids.end(); newiter++ )
		{
			newPIDNamesStream << (*newiter).assetId;
		}

		TA_Base_Core::DescriptionParameters desc;
		TA_Base_Core::NameValuePair pidGroupName("name", pidGroupNameStr );
		TA_Base_Core::NameValuePair location( "PID list", newPIDNamesStream.str() );

		desc.push_back(&pidGroupName);
		desc.push_back(&location);

        if (m_auditMessageSender != NULL)
        {
			//Maochun Sun++
			//TD12781
			
			/*
		    m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISPidGroupCreated, getSessionLocation(),
			    desc, 
			    "", // Further description text
			    "",
			    "", // alarm ID - not required
			    "", // incident key - not required
			    ""); // event key - not required 
			*/

			std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
			TA_Base_Core::IEntityData* guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );
			TA_ASSERT ( guiEntity != NULL, "EntityAccessFactory returned a NULL entity and yet did not throw an exception." );

        }
		//TES831 Events not logged!        

		//now refresh the data
		// readAndAddGroups( ); // TD9158 ~ refresh only upon receiving the event

        // TD9158 ~ added notification for other STIS Managers
        submitConfigUpdate(TA_Base_Core::EModificationType::Create);

		//TD16312
		//zhou yuan++
		readAndAddGroups();
		//++zhou yuan
    }


    void PidSelectionManager::deletePidGroup()
    {
        TA_Base_Core::ThreadGuard guard( m_pidGroupMapLock );

		//if the data read/write was bugger last time try to read it
		if(!m_dbOk)
		{
			readAndAddGroups();

			//and make them hit the button again
			return;
		}

		// TD14164 ++
		/*int responce = m_pidGroupCombo.MessageBox( _T("Are you sure you wish to delete the selected group?"),
			 _T("Delete Confirmation"), MB_YESNO );*/
		TA_Base_Bus::TransActiveMessage userMsg;
		CString fieldName = "the selected group";
		userMsg << fieldName;
		UINT responce = userMsg.showMsgBox(IDS_UW_610023);
		// ++ TD14164 

		if(responce != IDYES) return;

		TA_IRS_Core::PidGroupsAccessFactory& mgr = TA_IRS_Core::PidGroupsAccessFactory::getInstance( );

        const std::string deletedPidGroupName(m_activeGroupName);

		try
		{
			//API REF:
			//void deletePidGroup( const std::string& name );
			mgr.deletePidGroup( m_activeGroupName );
		}
		catch( TA_Base_Core::DatabaseException& ex )
		{
			std::string error("Failure occurred while deleting PID group:\n");
			error += ex.what();

			handlePidReadWriteError( error );
			return;
		}
		catch(...)
		{
			handlePidReadWriteError( "Unidentified failure occurred while deleting PID group" );
			return;
		}

		// TES831 Events not logged!
		std::string pidGroupNameStr(m_activeGroupName);

		TA_Base_Core::DescriptionParameters desc;
		TA_Base_Core::NameValuePair pidGroupName("name", pidGroupNameStr );

		desc.push_back(&pidGroupName);

		if (m_auditMessageSender != NULL)
        {
			//Maochun Sun++
			//TD12781

			/*
		    m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISPidGroupCreated, getSessionLocation(),
			    desc, 
			    "", // Further description text
			    "",
			    "", // alarm ID - not required
			    "", // incident key - not required
			    ""); // event key - not required 
			*/

            std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
			TA_Base_Core::IEntityData* guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );
			TA_ASSERT ( guiEntity != NULL, "EntityAccessFactory returned a NULL entity and yet did not throw an exception." );

        }
		//TES831 Events not logged!

		//now refresh the data
		// readAndAddGroups( ); // TD9158 ~ refresh only upon receiving the event

        // TD9158 ~ added notification for other STIS Managers
        submitConfigUpdate(TA_Base_Core::EModificationType::Delete);

		//TD16312
		//zhou yuan++
		readAndAddGroups();
		//++zhou yuan
    }


    void PidSelectionManager::buttonPressed(CallbackButton* button)
    {
        if (button == &m_saveButton)
        {
            savePidGroup();
        }
        else if (button == &m_deleteButton)
        {
            deletePidGroup();
        }
    }


    void PidSelectionManager::pidGroupComboChanged()
    {
        // get the current selection
        m_activeGroupName = m_pidGroupCombo.getCurrentGroupName();

        // refresh the PID list
        refreshPIDList();
		setButtonStates();
    }


    void PidSelectionManager::pidSelectionChanged()
    {
        m_pidSelectionListener.pidSelectionChanged(
            (m_pidListCtrl.GetItemCount() > 0),
            (m_pidListCtrl.GetFirstSelectedItemPosition() != NULL) );
    }

	bool PidSelectionManager::isPidListEmpty()
	{
		return m_pidListCtrl.GetItemCount() > 0;
	}

    void PidSelectionManager::initialiseLocationInfo()
    {
        m_locationInfo.clear();

        std::vector<TA_Base_Core::ILocation*> locations;

        try
        {
            std::vector<TA_Base_Core::ILocation*> locations =
                TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

            // build tha map
            for (std::vector<TA_Base_Core::ILocation*>::iterator iter = locations.begin();
                 iter != locations.end(); iter++)
            {
                m_locationInfo[(*iter)->getName()] = (*iter)->getKey();
            }

        }
        catch(TA_Base_Core::TransactiveException& te)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
        }
		catch(...)
		{

		}

        // clean up
        for (std::vector<TA_Base_Core::ILocation*>::iterator iter = locations.begin(); iter != locations.end(); iter++)
        {
            delete (*iter);
            (*iter) = NULL;
        }
    }


    void PidSelectionManager::onRunParamChange( const std::string& name, const std::string& value )
    {
        // The PID selection map is being updated
		if(name == "Deselect" || name == "Select" || name == "SynchroniseSelected")  // this app only regist four runparam "Select" "Deselect" "SynchroniseSelected" "Display"
		{
			ACE_GUARD(ACE_Thread_Mutex, ace_mon, Tag_mutex);
			++ DONTGOFRONT_TAG;
		}
        TA_Base_Core::ThreadGuard guard( m_pidGroupMapLock );

		std::string runParamReceived("");

		runParamReceived += name;
		runParamReceived += "  ";
		runParamReceived += value;

	    PidList::iterator i;

		// Break up into the header elements and the PID list
		// i.e Station, XPos, 
		std::vector<std::string> valueParts = tokenizeString( value, "," );

		std::string stationID = valueParts[0];
		std::stringstream schematicIDStream( valueParts[1] );
		unsigned int newSchematicID;
		schematicIDStream >> newSchematicID;

		// Check for basic validity of Select/Deselect/Display param, 
		// based on the number of items in the value
		if( ((name.compare("Select") == 0) ||
			(name.compare("Deselect") == 0) ||
			(name.compare("Display") == 0) ) &&
			(valueParts.size() != 3 ))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Invalid %s RunParam given to STIS Manager: %s",
                    name.c_str(),value.c_str());
			return;
		}

		if(name.compare("Display") == 0)
	    {
			if(valueParts[2].compare("SHOW") == 0)
			{
				// If this --display=SHOW is for a different station or from a different schematic
				// remove the existing selections and set the new location
				if( (m_currentLocation != stationID) || (m_schematicID  != newSchematicID) ) 
				{
					deselectAllPIDs();

					m_currentLocation = stationID;
					// Activate the new schematic
					m_schematicID = newSchematicID;

					// Remove all existing PIDs
					m_pidListCtrl.DeleteAllItems();
					
					// Deactivate the current schematic
					TA_IRS_App::GraphworxComms::getInstance().resetGraphworxDisplaysToInactive();

					activateSchematic( m_schematicID );
				}

				// Post a message to the CSTISManagerDlg window
				
				// unsuppress messages
				UserMessages::getInstance().setMessageSuppression(false);
			}
			else
			{
				// removed for consistency with the PA manager:

				// Removed as per Propweb case #3429
				// If we hide window whenever we get a HIDE commmand, when
				// the pamanager is displayed, and user leaves then goes back to
				// the schematic, we DON'T want the PA Manager hiding as the schematic
				// is openened - the PA Manager is hidden on startup by default
				// so the Display=HIDE command is now redundant..

				//ShowWindow(SW_MINIMIZE);
				//m_wantToShow = false;
			}

	    }
		else if(name.compare("Select") == 0)
	    {
			if(( stationID == m_currentLocation ) && ( newSchematicID == m_schematicID ))
			{
				try
				{
					std::string pidEntityName = valueParts[2];
					// tokenize the PID list
					std::vector<std::string> parts = tokenizePIDName( pidEntityName );

					// 1 part means a station
					// 2 parts is not valid
					// 3 parts is a level
					// 4 parts is a PID

					if (parts.size() == 1)
					{
						// '???' or '????'(all) or a station id eg 'DT17'

						if( parts[0].find("???") != std::string::npos ) // select everything
						{
							// deselect all PIDs
							deselectAllPIDs();

							// Resolve and add all PIDs to the list of selected PIDs
							addAllPIDs();
						}
						else // This is a single station - e.g. 'DBG'
						{
							addAllStationPIDs(parts[0]);
						}
					}
					else if (parts.size() == 3)
					{
						// If the first three characters are '???' or '????'
						// then this must be a specific level on ALL stations
						// e.g. ???.TIS.Basement1
						// Add all PIDs at the specific level on all stations
						if (parts[0].find("???") != std::string::npos)
						{
							addLevelAtAllStations(parts[2]);
						}
						else
						{
							// this is a specific level at a specific station
							addLevelAtStation( parts[0], parts[2] );
						}

					}
					else if (parts.size() == 4)
					{
						// specific PID
						addPID( pidEntityName );
					}
					else
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
							"Invalid Select RunParam given to STIS Manager %s",
							value.c_str());
					}
				}
				catch(TA_Base_Core::TransactiveException& te)
				{
					// database or data exception
					// TD14164 ++
					/*std::stringstream error;
					error << "Error while selecting items " << te.what();*/
					// ++ TD14164 

					LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());

					// TD14164 ++
					/*AfxMessageBox(error.str().c_str());*/
					TA_Base_Bus::TransActiveMessage userMsg;
					CString actionName = "selecting";
					userMsg << actionName;
					userMsg << te.what();
					userMsg.showMsgBox(IDS_UE_070125);
					// ++ TD14164 
				}
				catch(...)
				{
				}
			}
	    }
	    else if(name.compare("Deselect") == 0)
 	    {
			if(( stationID == m_currentLocation ) && ( newSchematicID == m_schematicID ))
			{
				try
				{
					// tokenize the PID list
					std::string pidEntityName = valueParts[2];
					std::vector<std::string> parts = tokenizePIDName( pidEntityName );

					// 1 part means a station
					// 2 parts is not valid
					// 3 parts is a level
					// 4 parts is a PID

					if (parts.size() == 1)
					{
						// '???' or '????' (all) or a station id

						if( parts[0].find("???") != std::string::npos ) // deselect everything
						{
							// deselect all PIDs
							deselectAllPIDs();
						}
						else // This is a single station - e.g. 'DBG'
						{
							deselectAllStationPIDs(parts[0]);
						}
					}
					else if (parts.size() == 3)
					{
						// If the first three characters are '????' or "???"
						// then this must be a specific level on ALL stations
						// e.g. ????.TIS.Basement1
						// Deselect all PIDs at the specific level on all stations
						if (parts[0].find("???") != std::string::npos )
						{
							deselectLevelAtAllStations(parts[2]);
						}
						else
						{
							// this is a specific level at a specific station
							deselectLevelAtStation( parts[0], parts[2] );
						}
					}
					else if (parts.size() == 4)
					{
						// specific PID
						deselectPID( pidEntityName );
					}
					else
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
							"Invalid Deselect RunParam given to STIS Manager %s",
							value.c_str());
					}
				}
				catch(TA_Base_Core::TransactiveException& te)
				{
					// database or data exception
					// TD14164 ++
					/*std::stringstream error;
					error << "Error while deselecting items " << te.what();*/
					// ++ TD14164 

					LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());

					/*AfxMessageBox(error.str().c_str());*/
					// TD14164 ++
					TA_Base_Bus::TransActiveMessage userMsg;
					CString actionName = "deselecting";
					userMsg << actionName;
					userMsg << te.what();
					userMsg.showMsgBox(IDS_UE_070125);
					// ++ TD14164 
				}
				catch(...)
				{

				}
			}

	    }
		else if( name.compare("SynchroniseSelected") == 0 )
		{
			if(( stationID == m_currentLocation ) && ( newSchematicID == m_schematicID ))
			{
				/*
				int numberOfPIDs;

				std::stringstream numPIDStream( valueParts[2] );

				numPIDStream >> numberOfPIDs;
				*/
				
				std::vector<std::string> pidList = tokenizeString( valueParts[3], ";" );
				std::for_each(pidList.begin(), pidList.end(), bind(&PidSelectionManager::addPID, this, _1));
			}

		}

	    // Refresh the gui's PID list.  Populate the 'Station' and 'PID' list from the 
	    // list of selected PIDs we have.

        // This is a CORBA thread - updating the PID list from here buggers up the list
	    //refreshPIDList();

        // force a refresh by "re-selecting" the current group
        m_pidGroupCombo.PostMessage(WM_COMMAND, MAKEWPARAM(m_pidGroupCombo.GetDlgCtrlID(),CBN_SELCHANGE),
                                    reinterpret_cast<LPARAM>(m_pidGroupCombo.GetSafeHwnd()));
    }


    void PidSelectionManager::addAllPIDs()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Adding all PIDs to the HMI selection");

        // get all pids
        // search on *.TIS.*
        std::vector<PID> pids = getAllPIDs("%.TIS.%");

        // add the PIDs to the HMI group
        addPIDsToSelection(pids, PidGroupCombo::HMIGroupName);
    }


    void PidSelectionManager::deselectAllPIDs()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Removing all PIDs from the HMI selection");

        // just clear the HMI map
        m_pidGroupMap[PidGroupCombo::HMIGroupName].clear();
    }


    void PidSelectionManager::addAllStationPIDs( const std::string& theStation )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Adding all %s station PIDs to the HMI selection",
                    theStation.c_str());

        // get all pids for the station
        // search on STN.TIS.*
        std::stringstream searchToken;
	    searchToken << theStation << ".TIS.%";

        std::vector<PID> pids = getAllPIDs(searchToken.str().c_str());

        // add the PIDs to the HMI group
        addPIDsToSelection(pids, PidGroupCombo::HMIGroupName);
    }


    void PidSelectionManager::deselectAllStationPIDs( const std::string& theStation )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Removing all %s station PIDs from the HMI selection",
                    theStation.c_str());

        // remove the PIDs from the HMI group

        PidList::iterator findIter = m_pidGroupMap[PidGroupCombo::HMIGroupName].begin();
        while ( findIter != m_pidGroupMap[PidGroupCombo::HMIGroupName].end() )
		{
			if( findIter->station == theStation )
			{
				m_pidGroupMap[PidGroupCombo::HMIGroupName].erase(findIter++);
			}
            else
            {
                findIter++;
            }
		}
    }


    void PidSelectionManager::addLevelAtAllStations( const std::string& levelName )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Adding all %s level PIDs to the HMI selection",
                    levelName.c_str());

        // get all pids for the level
        // search on *.TIS.level.*
        std::stringstream searchToken;
	    searchToken << "%.TIS." << levelName << ".%";

        std::vector<PID> pids = getAllPIDs(searchToken.str().c_str());

        // add the PIDs to the HMI group
        addPIDsToSelection(pids, PidGroupCombo::HMIGroupName);
    }


    void PidSelectionManager::deselectLevelAtAllStations( const std::string& levelName )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Removing all %s level PIDs from the HMI selection",
                    levelName.c_str());

	    // remove the PIDs from the HMI group

        PidList::iterator findIter = m_pidGroupMap[PidGroupCombo::HMIGroupName].begin();
        while ( findIter != m_pidGroupMap[PidGroupCombo::HMIGroupName].end() )
		{
			if( findIter->level == levelName )
			{
				m_pidGroupMap[PidGroupCombo::HMIGroupName].erase(findIter++);
			}
            else
            {
                findIter++;
            }
		}
    }


    void PidSelectionManager::addLevelAtStation( const std::string& station, const std::string& levelName )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Adding all %s station %s level PIDs to the HMI selection",
                    station.c_str(), levelName.c_str());

        // get all pids for the level
        // search on STN.TIS.level.*
        std::stringstream searchToken;
	    searchToken << station << ".TIS." << levelName << ".%";

        std::vector<PID> pids = getAllPIDs(searchToken.str().c_str());

        // add the PIDs to the HMI group
        addPIDsToSelection(pids, PidGroupCombo::HMIGroupName);
    }


    void PidSelectionManager::deselectLevelAtStation( const std::string& station, const std::string& levelName )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Removing all %s station %s level PIDs from the HMI selection",
                    station.c_str(), levelName.c_str());

        PidList::iterator findIter = m_pidGroupMap[PidGroupCombo::HMIGroupName].begin();
        while ( findIter != m_pidGroupMap[PidGroupCombo::HMIGroupName].end() )
		{
			if( (findIter->station == station) &&
                (findIter->level == levelName) )
			{
				m_pidGroupMap[PidGroupCombo::HMIGroupName].erase(findIter++);
			}
            else
            {
                findIter++;
            }
		}
    }


    void PidSelectionManager::addPID( const std::string& pidAssetName )
    {
		FUNCTION_ENTRY("addPID");
	    PID pid;

        // break name on '.'
        std::vector<std::string> parts = tokenizePIDName( pidAssetName );

        // from the start to the first '.' is the station name
        // from the first '.' to the second '.' is TIS
        // from the second '.' to the third '.' is the level
        // from the third '.' to the end of the string is the PID name

        // so a PID will have 4 parts
        if (parts.size() == 4)
        {
            // ignore certain data nodes
            if( ( parts[3] != "ISCS" ) &&
                ( parts[3] != "SEV" ) &&
				( parts[3] != "SEV_OCC") &&
                ( parts[3] != "TIS01" ) && 
                ( parts[3] != "TIS02" ) )
			{
				// this is a valid PID
                pid.assetId = pidAssetName;
                pid.station = parts[0];
                pid.name = parts[3];
                pid.level = parts[2];
				// TD14878 - when a PID is already valid, immediately add it to
				// the PidGroupCombo - lizettejl++
				std::vector<PID> pids;
				pids.push_back(pid);

				// add the PIDs to the HMI group
				addPIDsToSelection(pids, PidGroupCombo::HMIGroupName);
				// ++lizettejl 
				// TD14878
			}
        }
        else
        {
            // invalid PID
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "Invalid PID given for selection %s",
                        pidAssetName.c_str());//TD14337 yezh++
            // no need to add function exit here
			// FUNCTION_EXIT;
            // return;
        }

	    // TD14878 - when a selected PID with 4 parts is invalid, it will still 
		// still be added to the PidGroupCombo with uninitialized data, usually
		// displayed as blank - lizettejl
		//
	    //std::vector<PID> pids;
        //pids.push_back(pid);
		//
        // add the PIDs to the HMI group
        //addPIDsToSelection(pids, PidGroupCombo::HMIGroupName);		
		// ++lizettejl 
		// TD14878
		FUNCTION_EXIT;
    }


    void PidSelectionManager::deselectPID( const std::string& pidAssetName )
    {
        // remove the PID from the HMI group
        PidList::iterator findIter = m_pidGroupMap[PidGroupCombo::HMIGroupName].begin();
        while ( findIter != m_pidGroupMap[PidGroupCombo::HMIGroupName].end() )
		{
			if( findIter->assetId == pidAssetName )
			{
				m_pidGroupMap[PidGroupCombo::HMIGroupName].erase(findIter++);
                break;
			}
            else
            {
                findIter++;
            }
		}
    }

    
    std::vector<PID> PidSelectionManager::getAllPIDs(std::string searchToken)
    {
	    std::vector<TA_Base_Core::IEntityData*> entityData;
        entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(
                        TA_Base_Core::DataNodeEntityData::getStaticType(),
                        searchToken);

        // convert the EntityData to pid names
		std::vector<std::string> entityNames;
	    std::vector<TA_Base_Core::IEntityData*>::iterator i;
	    for ( i = entityData.begin(); i != entityData.end(); i++)
        {
			entityNames.push_back( (*i)->getName() );
	    }

		//convert the vector of pid names to a std::vector<PID>
		std::vector<PID> pidList = entityNamesToPids( entityNames );

	    for ( i = entityData.begin(); i != entityData.end(); i++)
	    {
		    delete (*i);
	    }

        return pidList;
    }

    std::vector<PID> PidSelectionManager::entityNamesToPids(std::vector<std::string> entityNames)
    {
		std::vector<PID> pidList;
		std::vector<std::string>::iterator i;

	    for ( i = entityNames.begin(); i != entityNames.end(); i++)
        {
            // break name on '.'
            std::vector<std::string> parts = tokenizePIDName( *i );
            
            // from the start to the first '.' is the station name
            // from the first '.' to the second '.' is TIS
            // from the second '.' to the third '.' is the level
            // from the third '.' to the end of the string is the PID name

            // so a PID will have 4 parts
            if (parts.size() == 4)
            {
                // ignore certain data nodes
                if( ( parts[3] != "ISCS" ) &&
                    ( parts[3] != "SEV" ) &&
					( parts[3] != "SEV_OCC") &&
                    ( parts[3] != "TIS01" ) && 
                    ( parts[3] != "TIS02" ) )
			    {
				    // this is a valid PID
                    PID pid;
                    pid.assetId = *i ; //the entire entity name
                    pid.station = parts[0];
                    pid.name = parts[3];
                    pid.level = parts[2];

                    pidList.push_back(pid);
			    }
            }
            else
            {
				// invalid PID
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
							"Invalid PID given for selection %s",
							(*i).c_str() );
                continue;
            }

	    }

		return pidList;
	}

    std::vector<std::string> PidSelectionManager::tokenizePIDName(std::string pidName)
    {
		return tokenizeString( pidName, "." );
    }


    void PidSelectionManager::addPIDsToSelection(std::vector<PID> pidList, std::string pidGroup)
    {
        // for each pid
        for ( std::vector<PID>::iterator pidIter = pidList.begin(); pidIter != pidList.end(); pidIter++)
	    {
		    bool found = false;

		    for( PidList::iterator findIter = m_pidGroupMap[pidGroup].begin();
            findIter != m_pidGroupMap[pidGroup].end() ; findIter++ )
		    {
			    if( (findIter->name == pidIter->name ) &&
                    (findIter->station == pidIter->station) )
			    {
				    found = true;
				    break;
			    }
		    }

		    if(!found)
		    {
			    m_pidGroupMap[pidGroup].push_back( *pidIter );
		    }
	    }
    }


    void PidSelectionManager::refreshPIDList()
    {
        TA_Base_Core::ThreadGuard guard( m_pidGroupMapLock );

	    m_pidListCtrl.DeleteAllItems();	

        PidList selectedPids = m_pidGroupMap[m_activeGroupName];
		PidList::iterator itPid = selectedPids.begin();
		LocationsManager& locationManager = LocationsManager::instance();
		std::string displayName;
		TA_Base_Core::ILocation* location = NULL;
	    for( ; itPid != selectedPids.end() ; itPid++ )
	    {  
			displayName = itPid->station;
			location = locationManager.getLocationByName(displayName);
			if(NULL != location)
			{
				displayName = location->getDisplayName();
			}
            int pos = m_pidListCtrl.InsertItem( PidListCtrl::LOCATION, displayName.c_str());
		    m_pidListCtrl.SetItemText( pos, PidListCtrl::PID_NAME, itPid->name.c_str() );

            // set the location key as the item data for sorting
            m_pidListCtrl.SetItemData( pos, m_locationInfo[itPid->station] );
	    }

        pidSelectionChanged();
    }




    void PidSelectionManager::populateDisplayDestination( TA_Base_Core::STISDestinationList& destinationList )
    {
        // a map of levels to PIDs
        typedef std::map< std::string, std::vector<std::string> > PidMap;
        PidMap destinationMap;

        // for each selected PID, add to the map
        {
            TA_Base_Core::ThreadGuard guard( m_pidGroupMapLock );
            for (PidList::iterator pidIter = m_pidGroupMap[m_activeGroupName].begin();
            pidIter != m_pidGroupMap[m_activeGroupName].end(); pidIter++)
            {
                destinationMap[ pidIter->station ].push_back(pidIter->name);
            }
        }

        try
        {
            // if all stations we selected have all pids selected
            // this will remain true
            bool allPidsSoFar = true;

            // now go through the map and see if any stations have all PIDs
            for (PidMap::iterator iter = destinationMap.begin(); iter != destinationMap.end(); iter++)
            {
                // get all pids for the station
                // search on STN.TIS.*
                std::stringstream searchToken;
	            searchToken << iter->first << ".TIS.%";
                std::vector<PID> pids = getAllPIDs(searchToken.str().c_str());

                // if the list has all PIDs for that station
                if ( iter->second.size() == pids.size() )
                {
                    // no pids means the entire station
                    iter->second.clear();

                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                                "Display request for station %s simplified to all pids",
                                iter->first);
                }
                else
                {
                    allPidsSoFar = false;
                }
            }

            // if all stations selected have all pids selected
            if (allPidsSoFar)
            {
                // check whether all stations are selected.
                // then there will be all pids at all stations
                // "AST" is all stations
                bool allStations = false;
                
                std::vector<TA_Base_Core::ILocation*> allLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
                // all stations is all locations minus the occ and the depot
                if ( (allLocations.size() - 2) == destinationMap.size() )
                {
                    //destinationList.length(destinationMap.size());
					destinationList.length(1);
                    destinationList[0].station = "ASTN";
                    allStations = true;
                }

                for (std::vector<TA_Base_Core::ILocation*>::iterator locIter = allLocations.begin();
                locIter != allLocations.end(); locIter++)
                {
                    delete (*locIter);
                }

                // if all stations were selected, then return here
                if (allStations)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Display request simplified to all pids at all stations");
                    return;
                }
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "Caught exception while optimising display request. Will use non optimised version.");
        }

        // make the sequence the right length
        destinationList.length(destinationMap.size());

        // now convert the map to level structures
        // for each destination - check for all level and all pids
        int i = 0;
        for (PidMap::iterator iter = destinationMap.begin(); iter != destinationMap.end(); iter++)
        {
            // first is the station name
            destinationList[i].station = iter->first.c_str();

            // second is a vector of pid names
            int numPids = iter->second.size();
            destinationList[i].pids.length(numPids);

            for (int j = 0; j < numPids; j++)
            {
                destinationList[i].pids[j] = iter->second[j].c_str();
            }

            i++;
        }
    }


    /*

    TES 880 - no longer a different format for clear and display messages


    void PidSelectionManager::populateClearDestination( TA_IRS_Core::STISDestination& destination )
    {
        // get the selected item
		POSITION selection = m_pidListCtrl.GetFirstSelectedItemPosition();
        int nItem = m_pidListCtrl.GetNextSelectedItem(selection);

        TA_ASSERT( nItem > -1, "Clear buttons were enabled when nothing is selected" );

        // from the selected item, get the PID structure
        bool found = false;

        TA_Base_Core::ThreadGuard guard( m_pidGroupMapLock );

        PidList::iterator findIter;
		for( findIter = m_pidGroupMap[m_activeGroupName].begin();
        findIter != m_pidGroupMap[m_activeGroupName].end() ; findIter++ )
		{
			if( (findIter->name.compare(m_pidListCtrl.GetItemText(nItem, 0)) == 0 ) &&
                (findIter->station.compare(m_pidListCtrl.GetItemText(nItem, 1)) == 0) )
			{
				found = true;
				break;
			}
		}

        TA_ASSERT( found, "Selected PID is not in internal PID list" );

		destination.station = findIter->station.c_str();
		destination.pids.length( 1 );
		destination.pids[0] = findIter->name.c_str();
    }
    */

    std::vector<std::string> PidSelectionManager::tokenizeString(std::string theString, const std::string& separatorList )
	{
        std::vector<std::string> parts;
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep( separatorList.c_str() );
        tokenizer tokens(theString, sep);
        
        for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        {
            parts.push_back(*tok_iter);
        }

        // if parts is empty, then this should return the entire string
        if (parts.size() == 0)
        {
            parts.push_back(theString);
        }

        return parts;
	}

	void PidSelectionManager::processShowCommand( const std::string& showCommand )
	{
		std::vector<std::string> commandParts;

		commandParts = tokenizeString( showCommand, "," );

		// TODO - complete

	}


	void PidSelectionManager::activateSchematic( unsigned int schematicID )
	{
		bool schematicUpdated = false;

		// Now we've finished loading, we may activate the graphworx display we're operating for
		try
		{
			schematicUpdated = TA_IRS_App::GraphworxComms::getInstance().activateGraphworxDisplay( schematicID );
		}
		catch (const TA_Base_Core::ValueNotSetException& e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "ValueNotSetException", e.what());      
		}
		catch (...)
		{
			// Silently log any error communicating with the actual schematic, may still run
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "While activating schematic");       
		}

/*    
	#ifdef _DEBUG
		// Perform an override if a debug session
		if (CachedConfig::getInstance()->getSessionId().compare("debug") == 0)
		{
			schematicUpdated = true;
		}
	#endif

		if (!schematicUpdated)
		{
			//PaErrorHandler::displayModalError(PaErrorHandler::ERROR_SCHEMATIC_COMMS);
			//PostQuitMessage(0);    
		}
*/

	}

	//read the groups that you are entitled to see given the location
	//currently associated with your session id
	//throws TA_Base_Core::DatabaseException
	PidSelectionManager::PidGroupMap PidSelectionManager::readGroupsFromDataBase()
	{
		TA_IRS_Core::PidGroupsAccessFactory& mgr = TA_IRS_Core::PidGroupsAccessFactory::getInstance( );

		//you must tell the PidGroupsAccessFactory what location you are getting PidGroups for
		mgr.setLocation( getSessionLocation() );

		//read the groups from the db as required - may throw DatabaseException
		std::vector<TA_IRS_Core::IPidGroup*> groupData = mgr.getPidGroups();

		//for each IPidGroup retrieved convert it to a PidGroupMapPair and add to the PidGroupMap
		PidGroupMap daMap;
		std::vector<TA_IRS_Core::IPidGroup*>::iterator iter = groupData.begin();
		for( ; iter != groupData.end(); iter++)
		{
			std::string groupName = (*iter)->getName();
			TA_ASSERT(!groupName.empty() , "failed !groupName.empty()");

			//get vector of PIDs in group via their entity names
			std::vector<PID> v = entityNamesToPids( (*iter)->getPidNames() );

			//convert the vector of PIDs to a list of PIDs
			PidList pidList(v.begin(), v.end());

			//insert the group into the map
			daMap.insert( PidGroupMapPair( groupName, pidList ) );
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "%d groups have been read from the database.", daMap.size());

		
		return daMap;
	}

	void PidSelectionManager::readAndAddGroups( )
	{
		//read the saved groups from the db and add them to m_pidGroupMap
		try
		{
			//read the groups from the db
			PidGroupMap savedGroups = readGroupsFromDataBase();

			//copy the schematic group into the the groups just read
			PidGroupMap::iterator iterToSchematicGroup = m_pidGroupMap.find(PidGroupCombo::HMIGroupName);

			//if we found the schematic group add it to the groups retrieved
			if( iterToSchematicGroup != m_pidGroupMap.end() )
			{
				savedGroups.insert( *iterToSchematicGroup );
			}

			//now swap new data for old
			m_pidGroupMap.swap(savedGroups);

			//update the combo box:
			PidGroupMap::iterator iter = m_pidGroupMap.begin();
			std::vector<std::string> groupNames;
			groupNames.push_back(PidGroupCombo::HMIGroupName);
			for( ; iter != m_pidGroupMap.end(); iter++)
			{
				std::string groupName = iter->first;
				if(PidGroupCombo::HMIGroupName != groupName)
				{
					groupNames.push_back( groupName );
				}
			}			
			m_pidGroupCombo.updateGroupNames( groupNames );

			setButtonStates();

			m_dbOk = true;

		}
		catch( TA_Base_Core::DatabaseException& ex )
		{
			std::string error("Failure occurred while reading saved PID groups:\n");
			error += ex.what();

			handlePidReadWriteError( error );
			return;
		}
		catch(...)
		{
			handlePidReadWriteError( "Unidentified failure occurred while reading saved PID groups" );
			return;
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "m_pidGroupMap now contains %d groups", m_pidGroupMap.size() );
                   
	}

	void PidSelectionManager::handlePidReadWriteError( const std::string& error )
	{
		m_dbOk = false;

		setButtonStates();

		//remove all groups accept the schematic related group from the local data
			//read the groups from the db
			PidGroupMap justOne;

			//copy the schematic group into justOne
			PidGroupMap::iterator iterToSchematicGroup = m_pidGroupMap.find(PidGroupCombo::HMIGroupName);

			if( iterToSchematicGroup != m_pidGroupMap.end() )
			{
				justOne.insert( *iterToSchematicGroup );
			}

			//now swap new data for old
			m_pidGroupMap.swap(justOne);

			//update the combo box:
			PidGroupMap::iterator iter = m_pidGroupMap.begin();
			std::vector<std::string> groupNames;
			for( ; iter != m_pidGroupMap.end(); iter++)
			{
				std::string groupName = iter->first;
				groupNames.push_back( groupName );

			}			
			m_pidGroupCombo.updateGroupNames( groupNames );

			displayDbError( error );

			setButtonStates();

			m_dbOk = true;
	}

	void PidSelectionManager::displayDbError( const std::string& error )
	{
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << error.c_str();
		CString errMsg = userMsg.constructMessage(IDS_UE_020071);
		/*m_pidGroupCombo.MessageBox( error.c_str(), _T("Database failure"));*/
		m_pidGroupCombo.MessageBox(errMsg, _T("Database failure"));
		// ++ TD14164 
	}

	unsigned long PidSelectionManager::getSessionLocation()
	{
		//todo verify that this updates on override and is the logical (vs.) 
		//physical location

        std::stringstream locationKeyStream;
        locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
        unsigned long locationKey = 0;
        locationKeyStream >> locationKey;
		return locationKey;

	}

	void PidSelectionManager::setButtonStates()
	{

        // enable/disable the save button
        m_saveButton.EnableWindow(m_canUsePidGroups );


		//if we can delete groups and we don't have the schematic group
		//selected then enable the delete button - else disable
		if( m_canUsePidGroups && m_activeGroupName != PidGroupCombo::HMIGroupName )
		{
			m_deleteButton.EnableWindow( true );
		}
		else
		{
			m_deleteButton.EnableWindow( false );
		}	
    }
   
    // TD9158 ~ added method definition
    void PidSelectionManager::processUpdate(
        const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("processUpdate");
        
        TA_ASSERT(updateEvent.getType() == TA_Base_Core::STIS_PID_GROUP,
                  _T("Invalid config update message."));

        const unsigned long sessionLocationKey = updateEvent.getKey();

        TA_ASSERT(sessionLocationKey > 0, _T("Invalid session location key."));

        // refresh our pid group list
        if (sessionLocationKey == getSessionLocation())            
        {
            TA_Base_Core::ThreadGuard guard(m_pidGroupMapLock);

            // force a  reload of data from database
            TA_IRS_Core::PidGroupsAccessFactory& mgr = 
                TA_IRS_Core::PidGroupsAccessFactory::getInstance();
            mgr.invalidate();

            // refresh current map and combo box
            readAndAddGroups();
           
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                        _T("Pid group successfully refreshed."));
        }

        FUNCTION_EXIT;    
    }

    // TD9158 ~ added method definition
    void PidSelectionManager::registerForConfigChanges()
    {
        FUNCTION_ENTRY("registerForConfigChanges");
        
        try
        {
            TA_Base_Core::OnlineUpdateListener::getInstance().
                registerAllInterests(TA_Base_Core::STIS_PID_GROUP, *this);
        }
        catch(...)
        {   
            LOG_EXCEPTION_CATCH(SourceInfo, "...", 
                "Failed to register to PID Group config update notification.");
        }

        FUNCTION_EXIT;    
    }

    // TD9158 ~ added method definition
    void PidSelectionManager::unregisterFromConfigChanges()
    {
        FUNCTION_ENTRY("unregisterFromConfigChanges");

        try
        {
            TA_Base_Core::OnlineUpdateListener::getInstance().
                deregisterAllInterests(TA_Base_Core::STIS_PID_GROUP, *this);
        }
        catch(...)
        {   
            LOG_EXCEPTION_CATCH(SourceInfo, "...", 
                "Failed to unregister from PID Group config update notification.");
        }

        FUNCTION_EXIT;    
    }

    // TD9158 ~ added method definition
    void PidSelectionManager::submitConfigUpdate(TA_Base_Core::EModificationType modificationType)
    {
        FUNCTION_ENTRY("submitConfigUpdate");

        TA_ASSERT(NULL != m_configUpdateSender, "m_configUpdateSender is NULL");

        // no need to specify changes as receiver will reload directly from db
        std::vector<std::string> changes;        

        try
        {     
            m_configUpdateSender->sendConfigUpdateMessage(
                TA_Base_Core::ConfigUpdate::ConfigPaZoneGroup /*ConfigStisPidGroup*/,  // Message Type
                getSessionLocation(),           // Key of changed item
                modificationType,               // EModificationType (upd/del)
                changes,                        // Desc of changes (col names)
                NULL);                          // FilterableData
            
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                "Config update for PID Group change was successfully sent.");
        }
        catch(...)
        {   
            LOG_EXCEPTION_CATCH(SourceInfo, "...", 
                "Failed to submit the config update for PID Group change.");
        }   

        FUNCTION_EXIT;
    }

} // TA_IRS_App
