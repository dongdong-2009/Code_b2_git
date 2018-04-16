#if !defined(StationModesDataPointManager_E51B6279_8001_48ac_9E2F_C0C86BF93A55__INCLUDED_)
#define StationModesDataPointManager_E51B6279_8001_48ac_9E2F_C0C86BF93A55__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/StationModesDataPointManager.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Utility to manage the creation and access to set of DataPointProxies for all DataPoints relating to
  * ECS Station Modes. To receive updates to changes in Proposed Station Mode values, client code 
  * should implement the IProposedStationModeObserver interface and register with this class for updates.
  * To receive updates to changes in current Station Mode Status and In Progress values, client code should 
  * implement the IStationModeStatusObserver interface and register with this class for updates.
  *  
  * IMPORTANT! If a client object has registered for updates and threadlocks any of the update callbacks
  * (such as proposedMasterMode1Updated) while calling either of the refresh methods, there is the potential
  * for deadlock. This is because the refresh methods construct DataPointProxies, and during the construction
  * the object subscribes for update messages. Deadlock can occur because the subscription manager will not 
  * allow a new subscriber to be added while any existing subscribers have not yet returned from processing
  * an update message. The simplest way to avoid this is to not register for updates until AFTER the desired
  * refresh methods have been called (and hence all the DataPointProxies constructed).
  */

#include <set>
#include <map>

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"



//
// Forward Declarations
//
namespace TA_Base_Core
{
    class DataPointEntityData;
}

namespace TA_Base_Bus
{
	class ScadaProxyFactory;
}

namespace TA_IRS_App
{
    class IStationModeStatusObserver;
    class IProposedStationModeObserver;
}


namespace TA_IRS_App
{
	std::string getErrorMessage(std::string exception);

	 //
    // Typedefs used by this class only.
    //

    typedef enum EStationModeDataPointType
    {
        CONTROL = 0,
        STATUS = 1,
        IN_PROGRESS = 2,
		VISUAL_LOCK = 3,
		AUTOMODE_STATUS =4
    };

    // enum hack - so we can have a const int defined in the header (without #define)
    enum { NUMBER_OF_STATIONMODE_DATAPOINT_TYPES = 6 };

    typedef struct
    {
        TA_Base_Bus::DataPointProxySmartPtr p_IDataPointProxy;
        unsigned long locationKey;
        EStationModeDataPointType stationModeDataPointType;
    }                                                                  DataPointProxyAndIdentifiers;
	//zn++ - CL-20670
    //typedef std::map< unsigned long, DataPointProxyAndIdentifiers* >     EntityKeyToDataPointProxyAndIdentifiers; 
	typedef std::map< unsigned long, boost::shared_ptr<DataPointProxyAndIdentifiers> >     EntityKeyToDataPointProxyAndIdentifiers; 
	//++zn

    typedef std::vector< unsigned long >                                StationModeDataPointKeySet;
    typedef std::map< unsigned long, StationModeDataPointKeySet >       LocationKeyToStationModeDataPointKeySet;

    typedef std::vector< TA_Base_Core::DataPointEntityData* >           DataPointEntityDataVector;
    typedef std::vector< IStationModeStatusObserver* >                  StationModeStatusObservers;

    //TD13744, not used at all
    //typedef std::vector< IProposedStationModeObserver* >                ProposedStationModeObservers;

    class StationModesDataPointManager : public TA_Base_Bus::IEntityUpdateEventProcessor 
    {

    public:

        /** 
          * Constructor
          */
        StationModesDataPointManager();


        /** 
          * Destructor
          */
        virtual ~StationModesDataPointManager();


        /** 
          * registerStationModeStatusObserver
          *
          * Register IStationModeStatusObserver to receive updates to changes in Station Mode Status 
          * or In Progress values.
          *
          * @param observer - Pointer to IStationModeStatusObserver registering to receive updates.
          */
        void registerStationModeStatusObserver( IStationModeStatusObserver* observer );


        /** 
          * deregisterStationModeStatusObserver
          *
          * De-register currently registered IStationModeStatusObserver.
          *
          * @param observer - Pointer to IStationModeStatusObserver de-registering from updates.
          */
        void deregisterStationModeStatusObserver( IStationModeStatusObserver* observer );


        /** 
          * cleanUp
          *
          * Cleanup all DataPointProxy objects. 
          */
        void cleanUp();


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
          * refreshDataPointProxies
          *
          * Cleans up any existing Proposed and Status/InProgress/Control DataPointProxies and/or
          * creates new ones.
          *
          * IMPORTANT! - This method contructs new DataPointProxy objects which attempt to subscribe
          * to update messages during construction. As such, it should not be called while client is
          * currently registered for updates and threadlocking the processing of these updates, 
          * (else DEADLOCK can occur).
          *
          * @param locationKeys       - Vector of Locations (by key) to create new StationMode DataPointProxies for.
          * @param doStatusAndControl - true if should create new Status/InProgress/Control DataPointProxies,
          *                             false otherwise.
          * @param doProposed         - true if should create new Proposed Station Mode DataPointProxies,
          *                             false otherwise. 
          * @param doClearAllExisting - true if should clear all existing DataPointPrroxies prior to creating new ones,
          *                             false if should just clear those we are creating new DataPointProxies for.
          *
          */
        void refreshDataPointProxies( const TA_ECSManagerCommonDefs::LocationKeys& locationKeys, 
            bool doStatusAndControl, bool doClearAllExisting = true );


        /** 
          * getStationModeStatus
          *
          * Get the current Station Mode Status value for particular location.
          *
          * @param locationKey - Key of location to get Station Mode Status for.
          *
          * @return Current int value of Station Mode Status for specified location.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not retrieve value 
          *               of DataPoint from agent.
          */
        int getStationModeStatus( unsigned long locationKey );


        // liuyu++ 10745a
        /** 
          * getStationAutoModeStatus
          *
          * Get the current Station Auto Mode Status value for particular location.
          *
          * @param locationKey - Key of location to get Station Auto Mode Status for.
          *
          * @return Current int value of Station Auto Mode Status for specified location.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not retrieve value 
          *               of DataPoint from agent.
          */
        int getStationAutoModeStatus( unsigned long locationKey );
        // liuyu++ 10745a

        /** 
          * getStationModeInProgress
          *
          * Get the current Station Mode In Progress value for particular location.
          *
          * @param locationKey - Key of location to get Station Mode In Progress for.
          *
          * @return Current int value of Station Mode In Progress for specified location.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not retrieve value 
          *               of DataPoint from agent.
          */
        int getStationModeInProgress( unsigned long locationKey );

		// 7196-Start
		int getStationLockStatus(unsigned long locationKey);

		/** 
         * refreshHealthPollInterval
         * refresh the DataPollHealthInterval 
         */
		void  refreshHealthPollInterval();
        // 7196-End

		// CL-20847 : no used
		//void setAutoModeStatusForLocation(int locationIndex,TA_ECSManagerCommonDefs::ECurrentStationAutoModeType currentStationAutoModeType);

		void setAutoModeStatusForLocationAndType(unsigned long locationKey,TA_ECSManagerCommonDefs::ECurrentStationAutoModeType currentStationAutoModeType );

    private:

        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        StationModesDataPointManager( const StationModesDataPointManager& );


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        StationModesDataPointManager& operator=( const StationModesDataPointManager& );


      
        //
        // Helper functions
        //

        /** 
          * populateStatusAndControlDataPointProxies
          *
          * Helper to create new DataPointProxies for Station Mode Status/Control/In Progress.
          *
          * @param locationKeys - Vector of locations (by key) to create proxies for.
          */
        void populateStatusAndControlDataPointProxies( const TA_ECSManagerCommonDefs::LocationKeys& locationKeys );


        // Note this method will delete the memory allocated on the heap of for the contents of the vector
        // passed in.
        /** 
          * populateProxiesFromDataPointEntityData
          *
          * Helper to create new DataPointProxies.
          *
          * @param locationKeys             - Vector of locations (by key) to create proxies for.
          * @param dataPointEntities        - Vector of (pointers to) database entity objects to create proxies 
          *                                   for. Note this method will delete the memory allocated on the heap
          *                                   for its contents.
          * @param stationModeDataPointType - Enum specifying type of Station Mode DataPointProxies being created.
          */
        void populateProxiesFromDataPointEntityData( const TA_ECSManagerCommonDefs::LocationKeys& locationKeys, 
            const DataPointEntityDataVector& dataPointEntities, EStationModeDataPointType stationModeDataPointType );


        /** 
          * StationModesDataPointManager::getValueFromLocationAndType
          *
          * Helper to retrieve value of particular (type and location) Station Mode DataPointProxy value. 
          *
          * @param locationKey              - location (by key) to retrieve value for,
          * @param stationModeDataPointType - Station Mode DataPoint Type to retrieve value for.
          *
          * @return The current int value of the specified Station Mode DataPoint.
          *
          * @exception ScadaProxyException if:
          *            1) No corresponding DataPointProxy exists.
          *            2) DataPointProxy could not retrieve value 
          *               of DataPoint from agent.
          */
        int StationModesDataPointManager::getValueFromLocationAndType( unsigned long locationKey, 
            EStationModeDataPointType stationModeDataPointType );



        /** 
          * cleanUpSpecifiedDataPointProxies
          *
          * Helper to cleanup specified subset of the DataPointProxies managed by this object.
          *
          * @param locationKeys       - locations (by key) to cleanup for.
          * @param doStatusAndControl - true if should cleanup Station Mode Status/In Progress/Control DataPointProxies, false otherwise.
          * @param doProposed         - true if should cleanup Proposed Station Mode DataPointProxies, false otherwise.    
          */
        void cleanUpSpecifiedDataPointProxies( const TA_ECSManagerCommonDefs::LocationKeys& locationKeys, bool doStatusAndControl);

        
        /** 
          * cleanUpAllDataPointProxies
          *
          * Helper to cleanup all DataPointProxies managed by this object.
          *
          */
        void cleanUpAllDataPointProxies();


       /** 
         * updateObserversWithDataPointValue
         *
          * Helper to notify all registered observers of change to Status, InProgress, or proposed
          * Station Mode Values or validity.
          *
          * @param type    - Identifier of DataPoint that has changed.
          * @param isValid - Validity of DataPoint (true if valid, false otherwise).
          */
        void updateObserversWithDataPointValue( EntityKeyToDataPointProxyAndIdentifiers::iterator entityKeyToDataPointProxyAndIdentifiersIt, bool isValid );


		void populateProxiesForAutoModeControl(const TA_ECSManagerCommonDefs::LocationKeys& locationKeys);		

		
    private:

        //
        // Flags to indicate whether certain catgories of StationMode DataPoints are currently
        // being managed by this object.
        //
        bool                                        m_isStatusAndControlInitialised;
        bool                                        m_isProposedInitialised;


        TA_ECSManagerCommonDefs::LocationKeys       m_locationKeys;

        //
        // Members to store StationMode DataPoints being managed.
        //

        // Mapping of each DataPoint entity key to Location key and IDataPointProxy pointer.
        EntityKeyToDataPointProxyAndIdentifiers     m_entityKeyToDataPointProxyAndIdentifiers;

        // Mapping of Location Key to the set of Keys correponding to the Location's StationMode 
        // DataPoints.  
        LocationKeyToStationModeDataPointKeySet     m_locationKeyToStationModeDataPointKeySet;


        // Members to store registered StationMode datapoint observers
        StationModeStatusObservers                  m_registeredStationModeStatusObservers;
       
        // Lockable to ensure access to vectors of observers is synchronous.
        TA_Base_Core::ReEntrantThreadLockable       m_observersLock;

		// 7196-Start
		int m_pollTime;
		// 7196-End

		TA_Base_Bus::ScadaProxyFactory*				m_proxyFactory;	

		std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>	m_autoModeControlDataPoint;

		// Reduce manager lauching time - Jiang shengguang++
		TA_ECSManagerCommonDefs::LocationKeys		m_LocationKeys;
		static DWORD WINAPI							ThreadFunction(LPVOID lpThreadParameter);
		DWORD										s_thread_id;
		HANDLE										s_thread_hd;
		// ++Jiang shengguang
    };
} // namespace TA_IRS_App

#endif // !defined(StationModesDataPointManager_E51B6279_8001_48ac_9E2F_C0C86BF93A55__INCLUDED_)
