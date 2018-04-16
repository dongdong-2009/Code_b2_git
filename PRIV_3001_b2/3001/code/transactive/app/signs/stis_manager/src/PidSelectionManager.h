/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/stis_manager/src/PidSelectionManager.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Manages the PID list and PID groups
  */

#if !defined(PIDSELECTIONMANAGER_H)
#define PIDSELECTIONMANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPidSelectionListener.h"
#include "PidListCtrl.h"
#include "PidGroupCombo.h"
#include "CallbackButton.h"

#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

//TES831 Events not logged!
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/TISAudit_MessageTypes.h"
//TES831 Events not logged!

#include "core/configuration_updates/src/IOnlineUpdatable.h"        // TD9158
#include "ace/Synch.h"
#include "ace/Guard_T.h"
#include <vector>
#include <list>
#include <map>
#include <string>


using TA_Base_Bus::ColourCombo;
using TA_Base_Bus::ListCtrlSelNoFocus;
using TA_Base_Bus::TISAgentAccessFactory;

namespace TA_Core
{
	class IPidGroup;
}

// TD9158 ~ added forward declaration
namespace TA_Base_Core
{
    class ConfigUpdateMessageSender;
}

namespace TA_IRS_App
{
    struct PID
    {
	    // Location of the PID
	    std::string station;

	    // Asset name 
	    std::string assetId;

	    // Short name eg PDP03
	    std::string name;

	    // Used for deleting from selection only
	    std::string level;
    };


    class PidSelectionManager : public TA_Base_Core::RunParamUser,
                                public IButtonListener,
                                public TA_Base_Core::IOnlineUpdatable   // TD9158
    {

    public:

		 
        /** 
          * PidSelectionManager
          *
          * Constructor which takes gui elements
          *
          * @param pidListCtrl
          * @param pidGroupCombo
          * @param saveButton
          * @param deleteButton
          *
          */
	    PidSelectionManager( PidListCtrl& pidListCtrl,
                             PidGroupCombo& pidGroupCombo,
                             CallbackButton& saveButton,
                             CallbackButton& deleteButton,
                             IPidSelectionListener& pidSelectionListener);


	    ~PidSelectionManager();

		void Init();
        /** 
          * setPermissions
          *
          * Set whether the user can modify PID groups.
          *
          * @param canModifyPidGroups
          */
        void setPermissions(bool canModifyPidGroups);


        /** 
          * populateDisplayDestination
          *
          * Build a destination list based on the current selection.
          *
          * @param destinationList the corba sequence to populate
          */
        void populateDisplayDestination( TA_Base_Core::STISDestinationList& destinationList );

        /** 
          * populateClearDestination
          *
          * Build a destination structure based on the currently selected list item.
          *
          * TES 880 - no longer a different format for clear and display messages
          *
          * @param destination  the corba structure to populate
          */
		//void populateClearDestination( TA_IRS_Core::STISDestination& destination );




        /** 
          * onRunParamChange
          *
          * Called for a runparam change. selection and deselection.
          *
          */
	    void onRunParamChange( const std::string& name, const std::string& value );

        /* These are passed on by the custom controls - this isnt a window, commands are just
           delegated to it so the functionality can be segregated from all the other display page
           functionality */


        /** 
          * buttonPressed
          *
          * Gets called when a button is pressed.
          *
          * @param button   the button that was pressed
          */
        virtual void buttonPressed(CallbackButton* button);


        /** 
          * pidGroupComboChanged
          *
          * Called when another entry is selected from the pid
          * group combo
          *
          */
        void pidGroupComboChanged();


        /** 
          * pidSelectionChanged
          *
          * Called when a PID is selected or deselected.
          * Or when the number of pids in the list changes.
          */
        void pidSelectionChanged();

		// Extracts the station from the header included in a run param value
		// e.g passing --select=DBG,1281,ALL will return DBG
		std::string getLocationFromRunParam( const std::string& runParamValue );

		// Extracts the x position from the header included in a run param value
		// e.g passing --select=DBG,1281,ALL will return 1281
		std::string getPositionFromRunParam( const std::string& runParamValue );

		void processShowCommand( const std::string& showCommand );


		bool isPidListEmpty();
		static volatile int DONTGOFRONT_TAG;
		static ACE_Thread_Mutex Tag_mutex;

    private:

        
        /** 
          * savePidGroup
          *
          * Called when the save group button is pressed
          *
          */
        void savePidGroup();


        /** 
          * deletePidGroup
          *
          * Called when the delete group button is pressed
          *
          */
        void deletePidGroup();


        /** 
          * initialiseLocationInfo
          *
          * Initialises location keys for sorting.
          */
        void initialiseLocationInfo();
		
        /** 
          * addAllPIDs
          *
          * Add all Pids from all stations
          */
        void addAllPIDs();

        /** 
          * deselectAllPIDs
          *
          * Deselects all PIDs
          */
        void deselectAllPIDs();

        /** 
          * addAllStationPIDs
          *
          * Add all Pids from the given station
          *
          * @param theStation   the 3 character station name eg DBG
          */
		void addAllStationPIDs( const std::string& theStation );

        /** 
          * deselectAllStationPIDs
          *
          * remove all Pids from the given station
          *
          * @param the station name eg DBG
          */
        void deselectAllStationPIDs( const std::string& theStation );

        /** 
          * addLevelAtAllStations
          *
          * Add a level from all stations
          *
          * @param levelName the name of the level eg Platform1
          */
		void addLevelAtAllStations( const std::string& levelName );

        /** 
          * deselectLevelAtAllStations
          *
          * Deselect a level across all stations
          *
          * @param levelName the name of the level eg Platform1
          */
        void deselectLevelAtAllStations( const std::string& levelName );

        /** 
          * addLevelAtStation
          *
          * Add a level at a station
          *
          * @param station  The station name eg DBG
          * @param levelName The level eg Platform2
          */
		void addLevelAtStation( const std::string& station, const std::string& levelName );

        /** 
          * deselectLevelAtStation
          *
          * Remove a level at a station
          *
          * @param station  The station name eg DBG
          * @param levelName The level eg Platform2
          */
        void deselectLevelAtStation( const std::string& station, const std::string& levelName );

        /** 
          * addPID
          *
          * Add a single PID
          *
          * @param pidAssetName the PID name eg DBG.TIS.Basement.LED01
          */
        void addPID( const std::string& pidAssetName );

        /** 
          * deselectPID
          *
          * Remove a single PID from the selection list
          *
          * @param pidAssetName The datanode name for the PID
          */
        void deselectPID( const std::string& pidAssetName );

        /** 
          * getAllPIDs
          *
          * Gets all pids matching the search string.
          *
          * @param searchToken  an sql type search string eg DBG.TIS.%
          *
          * @return The PIDs (not levels) matching that token
          */
        std::vector<PID> getAllPIDs(std::string searchToken);

        /** 
          * addPIDsToSelection
          *
          * Adds the pids to the selected pid list. ensures there are
          * no duplicates.
          *
          * @param pidList  A vector of PIDs
          * @param pidGroup The PID group to add the selection to
          */
        void addPIDsToSelection(std::vector<PID> pidList, std::string pidGroup);

        /** 
          * tokenizePIDName
          *
          * splits a pid or level name on the '.' characters.
          *
          * @param pidName  The full asset name eg DBG.TIS.Platform.PDP01 or DBG.TIS.Platform
          *
          * @return a vector of the strings between the '.' characters
          */
        std::vector<std::string> tokenizePIDName(std::string pidName);


		std::vector<std::string> tokenizeString(std::string theString, const std::string& separatorList );

        /** 
          * refreshPIDList
          *
          * This will apply the active PID selection group to
          * the PID list.
          */
		void refreshPIDList();

        /** 
          * activateSchematic
          *
          * Activates the current schematic
          * 
          */
		void activateSchematic( unsigned int schematicID );		

        // TD9158 ~ added method declaration
	    /**
	      * registerForConfigChanges
	      * 
	      * This registers the application for config updates.
	      * 
	      * @return void 
	      */
        void registerForConfigChanges();

        // TD9158 ~ added method declaration
	    /**
	      * unregisterFromConfigChanges
	      * 
	      * This unregisters the application from registered config updates.
	      * 
	      * @return void 
	      */
        void unregisterFromConfigChanges();

        // TD9158 ~ added method declaration
	    /**
	      * submitConfigUpdate
	      * 
	      * Called whenever there is a change being made by this class to the database
          *  - Note: Call AFTER the changes has been made
          *
          * This will post out ConfigUpdate messages to everyone listening out for
          *  configEditor updates (with relevant change information)
	      * 
	      * @return void 
	      * @param : TA_Base_Core::EModificationType modificationType
	      */
        void submitConfigUpdate(TA_Base_Core::EModificationType modificationType);

        //
        // From IOnlineUpdatable
        //

        // TD9158 ~ added section for IOnlineUpdatable and method processUpdate
        /**
         * processUpdate
         *
         * When there is a configuration update of the type and key matching
         * one registered by this class, this method will be invoked
         * to process the update accordingly.
         *
         * @return  void 
         * @param   updateEvent - This event contains all the 
         *                        information about the update
         */
        void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


		// List of PIDs selected on the PID selection schematics
        // list is used so iterators arent invalidated on item removal
        typedef std::list<PID> PidList;
        typedef std::map< std::string, PidList > PidGroupMap;
		typedef std::pair< std::string, PidList > PidGroupMapPair;
        PidGroupMap m_pidGroupMap;

		// read the groups ( for your location ) from the db
		// then remove all groups from the m_pidGroupMap data member, less the group
		//that is associated with the schematic, then and add the new groups (from the db )
		// to m_pidGroupMap and update the combo box.
		//
		// 
		//This method does not throw on read failure, instead reportPidReadError()
		// is called
		//
		void readAndAddGroups( );

		//used to store the success of the last db interaction
		bool m_dbOk;

		//tell the user that we could not read the saved PIDs
		//and update the controls accordingly
		void handlePidReadWriteError( const std::string& error );

		//display the db error to the user
		void displayDbError( const std::string& error );

		//read the groups that you are entitled to see given the location
		//currently associated with your session id
		//throws TA_Base_Core::DatabaseException
		PidGroupMap readGroupsFromDataBase();

		//return the location associated with the current session
		unsigned long getSessionLocation();

		//convert the vector of pid names to a std::vector<PID>
		std::vector<PID> entityNamesToPids(std::vector<std::string> entityNames);

		//set the Button States in the gui based on selections and the m_canUsePidGroups
		// value
		void setButtonStates();

        // This is the lock that protects the PID group (corba and MFC threads use it)
        TA_Base_Core::ReEntrantThreadLockable m_pidGroupMapLock;

        // this is the group that is active in the PID group combo - the group in use
        std::string m_activeGroupName;

        // the GUI controls
        PidListCtrl& m_pidListCtrl;
        PidGroupCombo& m_pidGroupCombo;
        CallbackButton& m_saveButton;
        CallbackButton& m_deleteButton;
        IPidSelectionListener& m_pidSelectionListener;

        // access control
        bool m_canUsePidGroups;

        // map of location names to keys
        std::map<std::string, unsigned long> m_locationInfo;

		// Station ID of the schematic that is currently active
		std::string m_currentLocation;

		// Schematic XPos we're currently communicating with - use as an ID
		unsigned int m_schematicID;

		//TES831 Events not logged!
        TA_Base_Core::AuditMessageSender*     m_auditMessageSender;

        TA_Base_Core::ConfigUpdateMessageSender* m_configUpdateSender;  // TD9158
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} // TA_IRS_App

#endif // !defined(PIDSELECTIONMANAGER_H)
