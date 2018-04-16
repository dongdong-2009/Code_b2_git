/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/proxy_library/src/ObserverUpdateQueueProcessor.h $
  * @author:  Andy Parker
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/07/11 18:24:37 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class managers the list of observers to be notified when a data point
  * proxy has updated information
  *
  */

#if !defined(ProxyObserverManager_D6E1E73A_9715_4715_8E47_E6EBB1062F3B__INCLUDED_)
#define ProxyObserverManager_D6E1E73A_9715_4715_8E47_E6EBB1062F3B__INCLUDED_

#include <map>
#include <vector>

#include "boost/shared_ptr.hpp"

#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/naming/src/INamedObject.h"
#include "core/threads/src/QueueProcessorPool.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"


/**
 * This class will manage the list of observers for each data point and ensure
 * they are notified of any data changes.
 * @version 1.0
 * @created 30-Jun-2004 10:18:53 AM
 */
namespace TA_Base_Bus
{
    class IProxyObserverNotifier;

    struct ObserverUpdate
    {
        IProxyObserverNotifier*     notifier;
        ScadaEntityUpdateType       updateType;
    };

    class ObserverUpdateQueueProcessor : public TA_Base_Core::QueueProcessorPoolCallback<ObserverUpdate>
	{
		public:
			ObserverUpdateQueueProcessor(int numberOfWorkers);
			virtual ~ObserverUpdateQueueProcessor();

		public:
            /**
		     * queueProcessorPoolCallback
		     *
		     * processing is done here
		     */
			void queueProcessorPoolCallback( boost::shared_ptr<ObserverUpdate> newEvent );

            void queueNotification(IProxyObserverNotifier* notifier, unsigned long entityKey, ScadaEntityUpdateType updateType);

			void startProcess();

			void stopProcess();

		private:
            ObserverUpdateQueueProcessor();

        private:

			TA_Base_Core::QueueProcessorPool<ObserverUpdate, 
				TA_Base_Core::QueueProcessorWorker<ObserverUpdate> >*	m_updateProcessorPool;

            bool														m_logStats;
			bool														m_isProcessing;
	};

}
#endif // !defined(ProxyObserverManager_D6E1E73A_9715_4715_8E47_E6EBB1062F3B__INCLUDED_)
