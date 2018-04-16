#if !defined(ECSStationModes_FAAD6103_A862_41b5_AB9E_3A4607A21535__INCLUDED_)
#define ECSStationModes_FAAD6103_A862_41b5_AB9E_3A4607A21535__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/ECSStationModes.h $
  * @author:  Robert Young
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/06/06 09:00:06 $
  * Last modified by:  $Author: qi.huang $
  *
  * This is the business class that provides all the the logic and data of the ECS Station Modes
  * user interface. It manipulates the ECS Station Modes user interface through IECSStationModesView.
  *
  */
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include <memory>

#include "app/ecs/ecs_manager/src/IProposedStationModeObserver.h"
#include "app/ecs/ecs_manager/src/IStationModeStatusObserver.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
#include "app/ecs/ecs_manager/src/ProposedStationModeUpdateSubscriber.h"

#include "bus/ecs/ecs_agent/IDL/src/IStationEcsAgentCorbaDef.h"

#include "bus/security/rights_library/src/RightsLibrary.h"	// CL-17274
#include "bus/security/access_control/actions/src/AccessControlledActions.h"  // CL-17274
#include "core/naming/src/FlexibleTimeoutNamedObject.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

//
// forward declarations 
//
namespace TA_IRS_App
{
    class IECSStationModesView;
    class MappedECSDataCache;
    class StationModesDataPointManager;
}


namespace TA_IRS_App
{
    class ECSStationModes : public IStationModeStatusObserver, public IProposedStationModeObserver
    {

    public:

        /** 
          * Constructor
          *
          * @param view -   the IStationModesView that will display the data of, 
          *                 and forward user events back to, this object
          */
        ECSStationModes( IECSStationModesView& view );


        /** 
          * Destructor
          */
        virtual ~ECSStationModes();

		/***************************************************
		IStationModeStatusObserver Interfaces Implementation
		****************************************************/
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

		/** 
          * stationVisualLockUpdated
          *
          * Implementation of IStationModeStatusObserver. Callback to notify of change in
          * Station Visual Lock value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param locationkey  - key of related location (station)
          * @param isValid      - true if value is confirmed value from agent, false otherwise.
          * @param value        - New Station Visual Lock value.
          */
		virtual void stationVisualLockUpdated( unsigned long locationKey, bool isValid, int value );

		
		/** 
          * stationAutoModeStatusUpdated
          *
          * Implementation of IStationModeStatusObserver. Callback to notify of change in
          * Station Auto Mode Status value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param locationkey  - key of related location (station)
          * @param isValid      - true if value is confirmed value from agent, false otherwise.
          * @param value        - New Station Auto Mode Status value.
          */
		virtual void stationAutoModeStatusUpdated( unsigned long locationKey, bool isValid, int value );

		virtual void stationAutoModeControlStateUpdated( unsigned long locationKey, TA_Base_Bus::EDataPointControlState controlState);


		/***************************************************
		IProposedStationModeObserver Interfaces Implementation
		****************************************************/
        /** 
          * proposedStationModeUpdated
          *
          * Implementation of IProposedStationModeObserver. Callback to notify of change in
          * Proposed Station Mode value (including invalidation when communications with related
          * Agent is lost)
          *
          * @param locationkey  - key of related location (station)
          * @param isValid      - true if value is confirmed value from agent, false otherwise.
          * @param value        - New Proposed Station Mode value.
          */
		virtual void proposedStationModeUpdated( bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* stationModes );

		virtual void overridingProposedStationModesUpdated( bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* stationModes);

		virtual void stationModeExecutionUpdated(bool isValid, TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* stationModeExecutionInfo);

		/***************
		Self Definition
		***************/
        /** 
          * initialise
          *
          * Loads user interface object to initial state.
          */
        void initialise();


        /** 
          * locationSelectionChanged
          *
          * Callback for user interface object to indicate when location (Station) selection has changed.
          *
          * @param locationIndex - Index of newly selected location (index relative to vector passed to user 
          *                        interface using setLocations).
          */
        void locationSelectionChanged( int locationIndex );


        /** 
          * stationModeSelectionChanged
          *
          * Callback for user interface object to indicate when Station Mode selection has changed.
          *
          * @param stationModeSelectionIndex - Index of newly selected Station Mode (relative to vector
          *                                    passed to user interface using setStationModeDetails) (-1 
          *                                    indicates no selection).
          */
        void stationModeSelectionChanged( int stationModeSelectionIndex );


        /** 
          * applyStationMode
          *
          * Apply selected Station Mode.
          *
          * PreCondition: 0 >= stationModeSelection < (size of Station Modes vector passed to view using 
          *               setStationModeDetails).
          *
          * @param stationModeSelectionIndex - Index of current selected Station Mode (relative to vector
          *                                    passed to view using setStationModeDetails).
          */
        void applyStationMode( int stationModeSelectionIndex );


        /** 
          * prepareForClose
          *
          * Callback for user interface object to indicate when application is about to shutdown. 
          */
        void prepareForClose();
		
		 // 7196-Start
		  /** 
          * refreshHealthPollInterval
          *
          * Notify the Station Mode DataPoint Manager that the Data has been updated
          */
		void refreshHealthPollInterval();
		// 7196-End

        //TD13744
        std::string getExecutionStateString(TA_IRS_Bus::EEcsExecutionState executionState);

        std::string getExecutionErrorString(TA_IRS_Bus::EEcsExecutionError executionError);
            
    private:
        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        ECSStationModes( const ECSStationModes& );


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        ECSStationModes& operator=( const ECSStationModes& );

        
        /** 
          * determineUserLocation
          *
          * Helper to determine current user's location.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException
          */
        void determineUserLocation();


        /** 
          * loadLocations
          *
          * Helper to retrieve all locations from cache (Stations) and set the 
          * view accordingly. Will also enable the view's the location selection
          * control as appropriate for user's location (enabled only if in OCC)
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
          * loadDetailsForLocation
          *
          * Helper to retrieve Station Mode Details of currently selected location from cache 
          * and subsequently load view with them.
          */
        void loadDetailsForLocation();


        /** 
          * loadValuesForLocation
          *
          * Helper to retrieve current Station Mode DataPoint values and set the view accordingly.
          */
        void loadValuesForLocation();

		void createStationECSAgents();

		//zn++ CL-17285 ITEM1 
		//void checkModeInProgress();
		//++zn

		//zn++ - for beautiful code
		/** 
          * isCentralLocation
          *
          * Helper to judge whether user launch the application in central location, 
		  *	so that can switch the station name combox
          */
		bool isCentralLocation();
		//++zn

    private:

		// The reference to the Station ECS Agent.
		typedef TA_Base_Core::FlexibleTimeoutNamedObject<TA_IRS_Bus::IStationEcsAgentCorbaDef,
			                 TA_IRS_Bus::IStationEcsAgentCorbaDef_ptr,
				             TA_IRS_Bus::IStationEcsAgentCorbaDef_var> StationECSAgentNamedObj;

		// marvin++
		typedef TA_Base_Core::FlexibleTimeoutNamedObject<TA_IRS_Bus::IMasterEcsAgentCorbaDef,
			                 TA_IRS_Bus::IMasterEcsAgentCorbaDef_ptr,
				             TA_IRS_Bus::IMasterEcsAgentCorbaDef_var> MasterECSAgentNamedObj;
		// ++marvin



		bool isProposedModeInOverridingMap(int proposedMode);


        IECSStationModesView& m_ecsStationModesView;    // Reference to the IStationModesView that will display the data of, and forward
                                                        // user events back to, this object.

		std::map<unsigned long, StationECSAgentNamedObj*> m_stationECSAgents;

		std::map<unsigned long, bool> m_stationLockStatusMap;

        std::auto_ptr< StationModesDataPointManager > m_pStationModesDataPointManager;  // Object to manage the Datapoints related to Station modes.

        std::auto_ptr< MappedECSDataCache > m_pMappedECSDataCache;  // Object to retrieve and cache the StationMode database data.

        LocationKeys m_locationKeys; // Ordered (inbound) container of location (station) keys.

        unsigned long m_currentUsersLocationKey; // Key of current user's location.

        LocationNames m_locationNames; // Ordered (inbound) container of location (station) names.

        int m_currentLocationSelectionIndex; // Index (relative to m_locationKeys and m_locationNames) of currently selected location (Station).

        int m_currentStationModeSelectionIndex; // Index (relative to m_currentStationModeDetails) of currently selected Station Mode.

        StationModeDetails m_currentStationModeDetails; // Container of Station Modes loaded for currently selected location.

        TA_Base_Core::ReEntrantThreadLockable m_updateLock; // IThreadlockable to ensure updates from multiple threads are processed synchronously.

		TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes m_currentOverridingStationModes;
                    
		// marvin++
		typedef  std::vector<int> CurrentProposedMode;

		typedef CurrentProposedMode::iterator CurrentProposedModeIt;

        // liuyu++ #134
        typedef  std::map<int, unsigned long> CongestionModePairLocationKey;
        CongestionModePairLocationKey m_CongestionModePair;
        // ++liuyu #134

		CurrentProposedMode m_currentProposed;

		TA_IRS_App::ProposedStationModeUpdateSubscriber* m_proposedStationModeUpdateSubscriber;

		std::auto_ptr<MasterECSAgentNamedObj> m_masterECSAgent;

		void ECSStationModes::filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType type,
			TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& proposedModes,
			TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& filteredModes);
		// ++marvin
 
		std::string m_sessionId;
		bool checkPermission();			//CL17274
		TA_Base_Bus::RightsLibrary*		m_rightsMgr;	//CL17274

		//zn++ - for beautiful code
		bool m_isCentralLocation;
		//++zn

		//zn++ - CL-17285 ITEM1
		int m_curStationModeIndexApplying;
		//++zn
    };

} // namespace TA_IRS_App

#endif // !defined(ECSStationModes_FAAD6103_A862_41b5_AB9E_3A4607A21535__INCLUDED_)
