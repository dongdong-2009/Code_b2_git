#if !defined(ECSMASTERMODES_5D0D0C32_443D_4192_B5F5_1343048E4952__INCLUDED_)
#define ECSMASTERMODES_5D0D0C32_443D_4192_B5F5_1343048E4952__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/ECSMasterModes.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is the business class that provides all the the logic and data of the ECS Master Modes
  * user interface. It manipulates the ECS Master Modes user interface through IECSMasterModesView.
  *
  */
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "app/ecs/ecs_manager/src/IStationModeStatusObserver.h"
#include "app/ecs/ecs_manager/src/IProposedMasterModeObserver.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
#include "app/ecs/ecs_manager/src/ProposedMasterModeUpdateSubscriber.h"

#include "bus/ecs/ecs_agent/IDL/src/IMasterEcsAgentCorbaDef.h"
#include "bus/ecs/ecs_agent/IDL/src/IStationEcsAgentCorbaDef.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/naming/src/FlexibleTimeoutNamedObject.h"  
#include "core/synchronisation/src/ReEntrantThreadLockable.h"



#include <memory>
 
namespace TA_IRS_App
{
    class IECSMasterModesView;
    class StationModesDataPointManager;
    //class MasterModesDataPointManager;
    class MappedECSDataCache;
}

namespace TA_IRS_App
{
    class ECSMasterModes : public IStationModeStatusObserver, 
		                   public IProposedMasterModeObserver
    {

    public:

        /** 
          * Constructor
          *
          * Construct instance of this class.
          *
          * @param view - Reference to user interface this object is to control.
          */
        ECSMasterModes( IECSMasterModesView& view );


        /** 
          * Destructor
          */
        virtual ~ECSMasterModes();


        /** 
          * prepareForClose
          *
          * Callback for user interface object to indicate when application is about to shutdown. 
          */
        void prepareForClose();


        /** 
          * initialise
          *
          * Loads user interface object to initial state.
          */
        void initialise();


        /** 
          * masterModeSelectionChanged
          *
          * Callback for user interface object to indicate when master mode selection has changed.
          *
          * @param masterModeIndex - integer index corresponding to selected Master Mode 
          *                          (-1 specifies no current selection)
          */
        void masterModeSelectionChanged( int masterModeIndex );


        /** 
          * applySelectedMasterMode
          *
          * Apply the selected Master Mode.
          *
          * @param masterModeIndex - integer index corresponding to selected Master Mode.
          */
        void applySelectedMasterMode( int masterModeIndex );


        /** 
          * stationModeStatusUpdated
          *
          * Implementation of IStationModeStatusObserver. Callback to notify of change in
          * Station Mode Status value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param locationkey  - key of related location (station)
          * @param isValid      - true if value is confirmed value from agent, false otherwise.
          * @param value        - New Station Mode Status value.
          */
        virtual void stationModeStatusUpdated( unsigned long locationKey, bool isValid, int value );


        /** 
          * stationModeInProgressUpdated
          *
          * Implementation of IStationModeStatusObserver. Callback to notify of change in
          * Station Mode In Progress value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param locationkey  - key of related location (station)
          * @param isValid      - true if value is confirmed value from agent, false otherwise.
          * @param value        - New Station Mode In Progress value.
          */
        virtual void stationModeInProgressUpdated( unsigned long locationKey, bool isValid, int value );

		virtual void stationVisualLockUpdated( unsigned long locationKey, bool isValid, int value );

		virtual void stationAutoModeStatusUpdated( unsigned long locationKey, bool isValid, int value );

		virtual void stationAutoModeControlStateUpdated( unsigned long locationKey, TA_Base_Bus::EDataPointControlState controlState);

		virtual void proposedMasterModesUpdated(bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* masterModes);

		virtual void masterModeExecutionUpdated(bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData* masterModeExecutionInfo);

		std::string getExecutionStateString(TA_IRS_Bus::EEcsExecutionState executionState);

        std::string getExecutionErrorString(TA_IRS_Bus::EEcsExecutionError executionError);

        /** 
          * proposedMasterMode1Updated
          *
          * Implementation of IProposedMasterModeObserver. Callback to notify of change in
          * Proposed Master Mode 1 value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param isValid  - true if value is confirmed value from agent, false otherwise.
          * @param value    - New Proposed Master Mode 1 value.
          */
        //virtual void proposedMasterMode1Updated( bool isValid, int value );


        /** 
          * proposedMasterMode2Updated
          *
          * Implementation of IProposedMasterModeObserver. Callback to notify of change in
          * Proposed Master Mode 2 value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param isValid  - true if value is confirmed value from agent, false otherwise.
          * @param value    - New Proposed Master Mode 2 value.
          */
        //virtual void proposedMasterMode2Updated( bool isValid, int value );

		bool checkPermission();


    private:

        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ECSMasterModes( const ECSMasterModes& );


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ECSMasterModes& operator=( const ECSMasterModes& );

    private:

		// The reference to the Master ECS Agent.
		typedef TA_Base_Core::FlexibleTimeoutNamedObject<TA_IRS_Bus::IMasterEcsAgentCorbaDef,
			                 TA_IRS_Bus::IMasterEcsAgentCorbaDef_ptr,
				             TA_IRS_Bus::IMasterEcsAgentCorbaDef_var> MasterECSAgentNamedObj;

		// The reference to the Station ECS Agent.
		typedef TA_Base_Core::FlexibleTimeoutNamedObject<TA_IRS_Bus::IStationEcsAgentCorbaDef,
			                 TA_IRS_Bus::IStationEcsAgentCorbaDef_ptr,
				             TA_IRS_Bus::IStationEcsAgentCorbaDef_var> StationECSAgentNamedObj;

        typedef enum EDirectionSettingIndex
        {
            SELECTED_EVACUATION = 1,
            SELECTED_AIRFLOW    = 9,
            CURRENT_EVACUATION  = 16,
            CURRENT_AIRFLOW     = 24
        };
        typedef enum EDirectionMapping
        {
            UP =    0,
            DOWN =  1,
            LEFT =  2,
            RIGHT = 3,
            UNKNOWN = 4
        };
        // Array of possible values mapped to their Enumerated Type value
        struct DirectionTypeValuePair
        {
            const char*  directionString;
            EDirectionMapping directionMapping;
        };
        static const DirectionTypeValuePair DIRECTION_MAPPING_ARRAY[];

        /** 
          * getBitSetIntegerFromDirection
          *
          * Static helper to convert EDirectionSettingIndex and direction String to int with
          * appropriate bits set.
          *
          * @param directionSettingIndex    - EDirectionSettingIndex to be converted.
          * @param directionString          - Direction string to be converted.
          *
          * @return int with appropriate bits set for the parameters passed in.
          */
        static int getBitSetIntegerFromDirection( EDirectionSettingIndex directionSettingIndex, const std::string& directionString );


        /** 
          * loadLocations
          *
          * Helper to retrieve all locations from cache (Stations) and set the 
          * view accordingly.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
        void loadLocations();


        /** 
          * determineZoneToLoadFor
          *
          * Helper used when Dialog-Type is EMERGENCY_MASTER_MODES_FOR_ZONE to determine the 
          * zone the dialog should load for. Uses the command line parameter TA_ECSManagerCommonDefs::ZONE_DIRECTION_RPARAM
          * if set, else uses Zone of currently proposed Master Mode.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
        void determineZoneToLoadFor();

        
        /** 
          * loadMasterModeDetails
          *
          * Helper to retrieve MasterMode Details from cache and load view
          * with them.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
        void loadMasterModeDetails();


        /** 
          * loadDataPointValues
          *
          * Helper to retrieve initial StationMode Status and Proposed Master Mode Values
          * and set view accordingly.
          */
        void loadDataPointValues();

		void createStationECSAgents();

    private:
        //
        // Reference to the IMasterModesView that will display the data of, and forward
        // user events back to, this object.
        //
        IECSMasterModesView& m_ecsMasterModesView;

        //
        // Object to manage the Datapoints related to Station modes
        //
        std::auto_ptr< StationModesDataPointManager >   m_pStationModesDataPointManager;

        //
        // Object to manage the Datapoints related to Station modes
        //
        //std::auto_ptr< MasterModesDataPointManager >    m_pMasterModesDataPointManager;

        //
        // Object to retrieve and cache the StationMode database data.
        //
        std::auto_ptr< MappedECSDataCache >             m_pMappedECSDataCache;

        TA_ECSManagerCommonDefs::LocationKeys           m_locationKeys;
        TA_ECSManagerCommonDefs::LocationNames          m_locationNames;

        TA_ECSManagerCommonDefs::MasterModeDetails      m_currentMasterModeDetails;

		// marvin++
		TA_IRS_App::ProposedMasterModeUpdateSubscriber* m_proposedMasterModeUpdateSubscriber;
		// ++marvin

        int                                             m_currentlySelectedMasterModeIndex;

        unsigned long                                   m_currentMasterMode;
        unsigned long                                   m_currentMasterModeDirectionKey;
        std::string                                     m_currentMasterModeDisplayDirection;
		std::string										m_sessionId;

        unsigned long                                   m_zoneID;

        TA_Base_Core::ReEntrantThreadLockable           m_updateLock;

		std::auto_ptr<MasterECSAgentNamedObj> m_masterECSAgent;
		unsigned long						  m_masterECSAgentEntityKey;
		unsigned long						  m_masterECSAgentLocation;
		unsigned long						  m_masterECSAgentSubsystem;
		std::map<unsigned long, StationECSAgentNamedObj*> m_stationECSAgents;

        //ToDo - change m_currentProposedMasterModes to m_currentProposedModes
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes m_currentProposedMasterModes;
		TA_Base_Bus::RightsLibrary*						   m_rightsMgr;
    };
} // namespace TA_IRS_App

#endif // !defined(ECSMASTERMODES_5D0D0C32_443D_4192_B5F5_1343048E4952__INCLUDED_)
