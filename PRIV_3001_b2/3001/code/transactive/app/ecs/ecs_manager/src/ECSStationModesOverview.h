#if !defined(ECSSTATIONMODESOVERVIEW_4E0A6164_7475_4889_B919_306A91528839__INCLUDED_)
#define ECSSTATIONMODESOVERVIEW_4E0A6164_7475_4889_B919_306A91528839__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/ECSStationModesOverview.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This is the business class that provides all the the logic and data of the ECS Station Modes 
  * Overview user interface. It manipulates the ECS Station Modes Overview user interface through 
  * IECSStationModesOverviewView.
  *
  */

#pragma warning(disable:4503)


#include "app/ecs/ecs_manager/src/IProposedStationModeObserver.h"
#include "app/ecs/ecs_manager/src/IStationModeStatusObserver.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
#include "app/ecs/ecs_manager/src/ProposedStationModeUpdateSubscriber.h"

#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/naming/src/FlexibleTimeoutNamedObject.h"

//
// forward declarations
//
namespace TA_IRS_App
{
    class IECSStationModesOverviewView;
    class MappedECSDataCache;
    class StationModesDataPointManager;
}


namespace TA_IRS_App
{
    class ECSStationModesOverview : public IStationModeStatusObserver, 
		                            public IProposedStationModeObserver
    {

    public:

        /** 
          * Constructor
          *
          * @param view - Reference to user interface this object is to control.
          */
        ECSStationModesOverview( IECSStationModesOverviewView& view );


        /** 
          * Destructor
          */
        virtual ~ECSStationModesOverview();


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

		virtual void stationModeExecutionUpdated(bool isValid, TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* stationModeExecutionInfo);
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
        //virtual void proposedStationModeUpdated( unsigned long locationKey, bool isValid, int value );
		virtual void proposedStationModeUpdated(bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* proposedModes);


		virtual void overridingProposedStationModesUpdated( bool isValid, TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes* stationModes);

       
        /** 
          * initialise
          *
          * Loads user interface object to initial state.
          */
        void initialise();


        /** 
          * prepareForClose
          *
          * Callback for user interface object to indicate when application is about to shutdown. 
          */
        void prepareForClose();


        /** 
          * proposedStationModeSelected
          *
          * Callback for user interface object to indicate when selection of proposed Station Mode
          * has changed.
          *
          * @param locationIndex            - Index of selected proposed Station Mode's location (relative 
          *                                   to container of locations passed to user interface through setLocations).
          * @param EProposedStationModeType - Enum corresponding to type of selected proposed Station Mode.
          */
        void proposedStationModeSelected( int locationIndex, EProposedStationModeType proposedStationModeType );


		/** 
          * autoModeSelected
          *
          * Callback for user interface object to indicate when selection of automode
          * has changed.
          *
          */
		void ECSStationModesOverview::autoModeSelected();


        /** 
          * applySelectedProposedStationMode
          *
          * Apply the selected proposed Station Mode.
          *
          * @param locationIndex            - Index of selected proposed Station Mode's location (relative 
          *                                   to container of locations passed to user interface through setLocations).
          * @param EProposedStationModeType - Enum corresponding to type of selected proposed Station Mode.
          */
        void applySelectedProposedStationMode( int locationIndex, EProposedStationModeType proposedStationModeType );


        /** 
          * applyAllProposedCongestionStationModes
          *
          * Apply all currently proposed Station Modes for congestion.
          */
        void applyAllProposedCongestionStationModes();

		//Prasanna
		void SetAutoModeStatusForLocation(int locationIndex,ECurrentStationAutoModeType currentStationAutoModeType);
		//Prasanna

		// 7196 -Start
		void refreshHealthPollInterval();

		// 7196 -End

		// wenching++
		// bug 134 (TD)
		bool checkCongestionPair(unsigned long locationKey, int proposedVal, unsigned long& pairLocationKey);
		// ++wenching
		// bug 134 (TD)

		void confirmApplyProposedStationMode(std::map<int, TA_IRS_App::EProposedStationModeType> allProposedIndexAndType);

	
		std::string getExecutionStateString(TA_IRS_Bus::EEcsExecutionState executionState);

        std::string getExecutionErrorString(TA_IRS_Bus::EEcsExecutionError executionError);

		
   private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ECSStationModesOverview( const ECSStationModesOverview& );


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ECSStationModesOverview& operator=( const ECSStationModesOverview& );

    private:

        // Typedefs to be used by this class only.
        typedef std::vector< int > StationModeDataPointValues;
        typedef std::pair< int,  EProposedStationModeType > LocationAndProposedType;
        typedef std::pair< int, int > LocationAndMode;
        typedef std::vector< LocationAndMode >LocationAndModeVector;


        /** 
          * loadStationModeDataPointValues
          *
          * Helper to retrieve initial Station Mode datapoint values and apply
          * to user interface.
          */
        void loadStationModeDataPointValues();


        /** 
          * getProposedStationModeType
          *
          * Helper to map proposed Station Mode to its type.
          *
          * @param locationKey         - Location key of proposed Station Mode.
          * @param proposedStationMode - Proposed Station Mode (actual mode value)
          *
          * @return EProposedStationModeType corresponding to Mode value and Location key passed in.
          */
        EProposedStationModeType getProposedStationModeType( unsigned long locationKey,
            int proposedStationMode );


        /** 
          * enableApplyStationModeAsAppropriate
          *
          * Helper to enable apply/disable Apply Station Mode control based on validity of
          * current proposed Station Mode selection.
          */
        void enableApplyStationModeAsAppropriate();

        /** 
          * enableApplyAllCongestionAsAppropriate
          *
          * Helper to enable apply/disable Apply All Station Modes for Congestion control based 
          * on whether there are currently any proposed Station Modes for Congestion.
          */
        void enableApplyAllCongestionAsAppropriate();


        /** 
          * applyModes
          *
          * Helper to apply container of proposed Station Modes.
          *
          * @param locationAndModeVector - Reference to container of proposed Station
          *                                Modes to apply.
          */
        void applyModes( const LocationAndModeVector& locationAndModeVector );

	
		void createStationECSAgents();

		//zn++ CL-17444 ITEM1
		bool isStationInOverrideOfType(int index, EProposedStationModeType type);
		//++zn

    private:

		// The reference to the Station ECS Agent.
		typedef TA_Base_Core::FlexibleTimeoutNamedObject<TA_IRS_Bus::IStationEcsAgentCorbaDef,
			                 TA_IRS_Bus::IStationEcsAgentCorbaDef_ptr,
				             TA_IRS_Bus::IStationEcsAgentCorbaDef_var> StationECSAgentNamedObj;

		// The reference to the Master ECS Agent.
		typedef TA_Base_Core::FlexibleTimeoutNamedObject<TA_IRS_Bus::IMasterEcsAgentCorbaDef,
			                 TA_IRS_Bus::IMasterEcsAgentCorbaDef_ptr,
				             TA_IRS_Bus::IMasterEcsAgentCorbaDef_var> MasterECSAgentNamedObj;

		std::map<unsigned long, StationECSAgentNamedObj*> m_stationECSAgents;

        IECSStationModesOverviewView& m_ecsStationModesOverviewView;    // Reference to the IStationModesOverviewView that will display the data of, and forward
                                                                        // user events back to, this object.

        std::auto_ptr< StationModesDataPointManager > m_pStationModesDataPointManager;  // Object to manage the Datapoints related to Station modes

        std::auto_ptr< MappedECSDataCache > m_pMappedECSDataCache; // Object to retrieve and cache the StationMode database data.

        LocationAndProposedType m_currentSelection; // Current proposed Station Mode selection.

        LocationKeys m_locationKeys; // Container of location (Station) keys inbound order.

        LocationNames m_locationNames; // Container of location (Station) names inbound order.

        StationModeDataPointValues m_currentProposedStationModeValues; // The last received values of Proposed Station Mode for each location. 

		// This holds all overriding station modes per station location, hence the use of a vector within the map.
		std::map< int, std::vector<int> > m_currentOverridingProposedStationModeValues;

		TA_IRS_Bus::IMasterEcsAgentCorbaDef::ProposedModes m_currentOverridingStationModes;

        TA_Base_Core::ReEntrantThreadLockable m_updateLock; // IThreadlockable to ensure updates from multiple threads are processed synchronously.
		std::string m_sessionId;

		std::auto_ptr<MasterECSAgentNamedObj> m_masterECSAgent;

		TA_IRS_App::ProposedStationModeUpdateSubscriber *m_proposedStationModeUpdateSubscriber;

		void ECSStationModesOverview::filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType type,
			TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& proposedModes,
			TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& filteredModes);

        typedef  std::multimap<int, unsigned long> CongestionModeMap;
		typedef CongestionModeMap::iterator  CongestionModeMapIt;
        CongestionModeMap m_mapCongestionModes;
    };

} // namespace TA_IRS_App

#endif // !defined(ECSSTATIONMODESOVERVIEW_4E0A6164_7475_4889_B919_306A91528839__INCLUDED_)
