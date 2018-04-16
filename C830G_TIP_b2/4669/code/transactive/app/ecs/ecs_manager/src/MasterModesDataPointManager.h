#if !defined(MasterModesDataPointManager_EA5DD5C1_89C7_4d1b_BB3C_38FFBAB0672A__INCLUDED_)
#define MasterModesDataPointManager_EA5DD5C1_89C7_4d1b_BB3C_38FFBAB0672A__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ecs_manager/src/MasterModesDataPointManager.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Utility to manage the creation and access to set of DataPointProxies for all DataPoints relating to
  * ECS Master Modes. To receive updates to changes in Proposed and Current Master Mode values, client code 
  * should implement the IProposedMasterModeObserver interface and register with this class for updates.
  *
  * IMPORTANT! If a client object has registered for updates and threadlocks any of the update callbacks
  * (such as proposedMasterMode1Updated) while calling either of the refresh methods, there is the potential
  * for deadlock. This is because the refresh methods construct DataPointProxies, and during the construction
  * the object subscribes for update messages. Deadlock can occur because the subscription manager will not 
  * allow a new subscriber to be added while any existing subscribers have not yet returned from processing
  * an update message. The simplest way to avoid this is to not register for updates until AFTER the desired
  * refresh methods have been called (and hence all the DataPointProxies constructed).
  */

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/proxies/src/ScadaProxyFactory.h"
#include "bus/scada/proxies/src/IEntityUpdateEventProcessor.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <vector>
#include <map>


namespace TA_IRS_App
{
    class IDataPointProxyWithoutAlarmAckStatus;
    class IProposedMasterModeObserver;
}

namespace TA_Base_Core
{
    class DataPointEntityData;
}

namespace TA_IRS_App
{
    class MasterModesDataPointManager : public TA_Base_Bus::IEntityUpdateEventProcessor 
    {

    public:

        /** 
          * Constructor
          */
        MasterModesDataPointManager();

        
        /** 
          * Destructor
          */
        virtual ~MasterModesDataPointManager();


        /** 
          * refreshProposedAndCurrentDataPointProxies
          *
          * Cleans up any existing Proposed and Current DataPoint Proxies
          * and creates new ones.
          *
          * IMPORTANT! - This method contructs new DataPointProxy objects which attempt to subscribe
          * to update messages during construction. As such, it should not be called while client is
          * currently registered for updates and threadlocking the processing of these updates, 
          * (else DEADLOCK can occur).
          */
        void refreshProposedAndCurrentDataPointProxies();


        /** 
          * refreshZoneDirectionDataPointProxies
          *
          * Cleans up any existing Zone Direction DataPoint Proxies
          * and creates new ones from vector of DataPoint keys passed in.
          *
          * IMPORTANT! - This method contructs new DataPointProxy objects which attempt to subscribe
          * to update messages during construction. As such, it should not be called while client is
          * currently registered for updates and threadlocking the processing of these updates, 
          * (else DEADLOCK can occur).
          *
          * @param zoneDirectionKey - Vector of keys to populate new Zone Direction DataPoint Proxies
          *                           from.
          */
        void refreshZoneDirectionDataPointProxies( const TA_ECSManagerCommonDefs::ZoneDirectionKeys& zoneDirectionKeys ); 

        
        /** 
          * registerProposedMasterModeObserver
          *
          * Register IProposedMasterModeObserver to receive updates to changes in Proposed or
          * Current Master Mode value changes.
          *
          * @param observer - Pointer to IProposedMasterModeObserver registering to receive updates.
          */
        void registerProposedMasterModeObserver( IProposedMasterModeObserver* observer );


        /** 
          * deregisterProposedMasterModeObserver
          *
          * De-register currently registered IProposedMasterModeObserver.
          *
          * @param observer - Pointer to IProposedMasterModeObserver de-registering from updates.
          */
        void deregisterProposedMasterModeObserver( IProposedMasterModeObserver* observer );


        /** 
          * cleanUp
          *
          * Cleanup all DataPointProxy objects. 
          */
        void cleanUp();

        
        /** 
          * getProposedMasterMode1
          *
          * Get the current value of Proposed Master Mode 1.
          *
          * @return int value of Proposed Master Mode 1.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not retrieve value 
          *               of DataPoint from agent.
          */
        int getProposedMasterMode1();


        /** 
          * getProposedMasterMode2
          *
          * Get the current value of Proposed Master Mode 2.
          *
          * @return int value of Proposed Master Mode 2.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not retrieve value 
          *               of DataPoint from agent.
          */
        int getProposedMasterMode2();


        /** 
          * getCurrentMasterMode
          *
          * Get the current value of Current Master Mode.
          *
          * @return int value of Current Master Mode.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not retrieve value 
          *               of DataPoint from agent.
          */
        int getCurrentMasterMode();


        /** 
          * setCurrentMasterMode
          *
          * Set the value of Current Master Mode.
          *
          * @param value - Value to set the Current Master Mode to.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not write to DataPoint.
          */
        void setCurrentMasterMode( int value );


        /** 
          * getDirectionValue
          *
          * Get current value of particular Zone Direction.
          *
          * @param directionKey - Key of Zone Direction to retrieve value of.
          *
          * @return int value of Direction requested..
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not retrieve value 
          *               of DataPoint from agent.
          */
        int getDirectionValue( unsigned long directionKey );


        /** 
          * setDirectionValue
          *
          * Set the current value of particular Zone Direction.
          *
          * @param directionKey - Key of Zone Direction to set value of.
          * @param value        - Value to set the Current Master Mode to.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not retrieve value 
          *               of DataPoint from agent.
          */
        void setDirectionValue( unsigned long directionKey, int value );


        /** 
          * processEntityUpdateEvent
          * 
          * Callback for DataPointProxies to notify when value changes or becomes 
          * invalid/valid.
          *
          * @param entityKey  - Key of related DataPointProxy notifying this object.
          * @param updateType - Type of update this object being notified of.
          */
        virtual void processEntityUpdateEvent( unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType );


        /** 
          * areProposedAndCurrentInitialised
          *
          * Indicate whether Proposed and Current DataPoint Proxies have been initialised.
          *
          * @return true if Proposed and Current DataPoint Proxies have been initialised,
          *         false otherwise.
          */
        bool areProposedAndCurrentInitialised() { return m_areProposedAndCurrentInitialised; }


        /** 
          * areZoneDirectionInitialised
          *
          * Indicate whether Zone Direction dataPointProxies have been initialised.
          *
          * @return true if Proposed and Current DataPoint Proxies have been initialised,
          *         false otherwise.
          */
        bool areZoneDirectionInitialised() { return m_areZoneDirectionInitialised; }

    private:

        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        MasterModesDataPointManager( const MasterModesDataPointManager& );


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        MasterModesDataPointManager& operator=( const MasterModesDataPointManager& );

    private:
        typedef enum EMasterModeDataPointType
        {
            PROPOSED_MASTER_MODE_1 = 0,
            PROPOSED_MASTER_MODE_2 = 1,
            CURRENT_MASTER_MODE = 2,
            DIRECTION = 3
        };
        typedef std::pair< TA_Bus::IDataPointProxyWithoutAlarmAckStatus*, EMasterModeDataPointType > DataPointProxyAndType;
        typedef std::map< unsigned long, DataPointProxyAndType > EntityKeyToDataPointProxyAndType;
        typedef std::vector< IProposedMasterModeObserver* > ProposedMasterModeObservers;
        typedef std::vector< TA_Base_Core::DataPointEntityData* > DataPointEntityDataVector;


        /** 
          * populateProposedAndCurrentMasterModeDataPointProxies
          *
          * Helper to create proposed and current Master Mode DataPointProxies.
          */
        void populateProposedAndCurrentMasterModeDataPointProxies();

    
        /** 
          * populateZoneDirectionDataPointProxies
          *
          * Helper to create Zone Direction DaatPoint Proxies from container of keys.
          *
          * @param zoneDirectionKeys - Reference to container of Zone Direction keys to create proxies for.
          */
        void populateZoneDirectionDataPointProxies( const TA_ECSManagerCommonDefs::ZoneDirectionKeys& zoneDirectionKeys );
        
        
        /** 
          * cleanUpDataPointEntityDataVector
          *
          * Helper to clean up the heap memory allocated for vector of DataPoint database entity objects.
          *
          * @param dataPointEntityVector - Reference to vector to clean up.
          */
        void cleanUpDataPointEntityDataVector( DataPointEntityDataVector& dataPointEntityVector );

        
        /** 
          * getMasterModeDataPointValue
          *
          * Helper to retrieve value of Master Mode DataPointProxy.
          *
          * @param entityKey - Key of DataPointProxy to retrieve value for.
          *
          * @return Int value of specified Master Mode DataPointProxy.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not retrieve value 
          *               of DataPoint from agent.
          */
        int getMasterModeDataPointValue( unsigned long entityKey );

    
        /** 
          * setMasterModeDataPointValue
          *
          * Helper to set value of Master Mode DataPointProxy.
          *
          * @param entityKey - Key of DataPointProxy to set value for.
          * @param value     - Value to set DataPointProxy to.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not set value 
          *               of DataPoint.
          */
        void setMasterModeDataPointValue( unsigned long entityKey, int value );


        /** 
          * updateObserversWithDataPointValue
          *
          * Helper to notify all registered observers of change to current or proposed
          * Master Mode Value or validity.
          *
          * @param type    - Identifier of DataPoint that has changed.
          * @param isValid - Validity of DataPoint (true if valid, false otherwise).
          */
        void updateObserversWithDataPointValue( EntityKeyToDataPointProxyAndType::iterator entityKeyToDataPointProxyAndTypeIt, bool isValid );

    private:
        ProposedMasterModeObservers             m_registeredProposedMasterModeObservers; // Currently registered observers.
        EntityKeyToDataPointProxyAndType        m_entityKeyToDataPointProxyAndType; // Mapping of entity key to DataPointProxies maanged by this object.
        unsigned long                           m_proposedMasterMode1Key; // Key of Proposed Master Mode 1.
        unsigned long                           m_proposedMasterMode2Key; // Key of Proposed Master Mode 2.
        unsigned long                           m_currentMasterModeKey; // Key of Current Master Mode.
        TA_Base_Core::ReEntrantThreadLockable        m_observersLock; // Lockable to ensure synchronous access to vector of registered observers.

        bool m_areProposedAndCurrentInitialised; // Flag to indicate when Proposed and Current DataPointProxies created.
        bool m_areZoneDirectionInitialised; // Flag to indicate when Zone Direction DataPointProxies created.
    };
} // namespace TA_IRS_App

#endif // !defined(MasterModesDataPointManager_EA5DD5C1_89C7_4d1b_BB3C_38FFBAB0672A__INCLUDED_)
