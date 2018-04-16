/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/scada/proxy_library/src/DataPointProxyManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  DataPointProxyManager.h
//  Implementation of the Class DataPointProxyManager
//  Created on:      28-Jun-2004 09:39:36 AM
///////////////////////////////////////////////////////////

#if !defined(DataPointProxyManager_0C5992F9_1E43_470b_B25B_B88B060429B6__INCLUDED_)
#define DataPointProxyManager_0C5992F9_1E43_470b_B25B_B88B060429B6__INCLUDED_

#include <map>

#include "bus/scada/proxy_library/src/ObserverUpdateQueueProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IProxySmartPtrCallback.h"
#include "bus/scada/common_library/src/ThreadsafeMap.h"

#include "core/threads/src/QueueProcessor.h"
#include "core/naming/src/INamedObject.h"    // CorbaName definition
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

/**
 * This class will be responsible for managing the collection of data points and
 * notifying the subscribers of any change.
 * @version 1.0
 * @created 28-Jun-2004 09:39:36 AM
 */

namespace TA_Base_Bus
{
	class DataPointSyncManager;
	class DataPointProxy;
	class IDataPointProxy;
	class IEntityUpdateEventProcessor;

    struct DpProxyDynamicSyncUpdate
    {
        unsigned long entityKey;
        DpDynamicSyncUpdate detail;
    };

	//TD15939, Use this class as a queue processor to avoid blocking much time of caller
	class DataPointProxyManager : public IProxySmartPtrCallback,
								  public TA_Base_Core::QueueProcessor<DpProxyDynamicSyncUpdate> 
	{

		public:

			DataPointProxyManager(DataPointSyncManager& syncManager,
								 ObserverUpdateQueueProcessor& observerUpdateQueueProcessor);
			virtual ~DataPointProxyManager();

		public:

            void registerDataPointEntityData( TA_Base_Core::IEntityDataPtr EntityData );
			void getProxySmartPointer(unsigned long entityKey, TA_Base_Core::DataPointEntityDataPtr entityData,
														IEntityUpdateEventProcessor& observer,
														DataPointProxySmartPtr & dataPointProxySmartPtr);

			void setMonitorMode();

			void setControlMode();

			//
			// IDataPointProxySmartPtrCallback interface
			//

			// Used by DataPointSmartPtr to notify library of destruction/copy
			virtual void onCreateProxy( IProxyObserverNotifier* observerNotifier,
										IEntityUpdateEventProcessor& observer );
			virtual void onCopyProxy( IProxyObserverNotifier* observerNotifier,
									  IEntityUpdateEventProcessor& observer );
			virtual void onDeleteProxy( IProxyObserverNotifier* observerNotifier,
										IEntityUpdateEventProcessor& observer );


         //************************************
         // Synchronisation Update Interfaces *
         //************************************
			/**
			 * Data Point Configuration
			 */
			virtual void onLoadDataPointConfiguration(unsigned long entityKey);

			/**
			 * Data Point Agent Interface not available
			 */
			virtual void onDataPointAgentInterfaceNotAvailable (unsigned long entityKey);

			//TD15939, to add update into queue
			virtual void insertUpdateQueue(boost::shared_ptr<DpProxyDynamicSyncUpdate> dataPointUpdate);
			
		protected:

			boost::shared_ptr<DataPointProxy> getProxy (unsigned long entityKey);
			boost::shared_ptr<DataPointProxy> addProxy (unsigned long entityKey, TA_Base_Core::DataPointEntityDataPtr entityData);
			void removeProxy (unsigned long entityKey);

			//TD15939, virtual function must be implemented for queue processor.
			virtual void processEvent(boost::shared_ptr<DpProxyDynamicSyncUpdate> newEvent);
			
		private:

			typedef ThreadsafeMap < unsigned long, DataPointProxy >  ProxyMap;
            typedef ThreadsafeMap < unsigned long, TA_Base_Core::DataPointEntityData> DpEntityMap;

			ProxyMap								m_proxyMap;
            DpEntityMap                             m_entityMap;
            
			DataPointSyncManager&					m_syncManager;
			ObserverUpdateQueueProcessor&			m_observerUpdateQueueProcessor;
	};
};
#endif // !defined(DataPointProxyManager_0C5992F9_1E43_470b_B25B_B88B060429B6__INCLUDED_)
