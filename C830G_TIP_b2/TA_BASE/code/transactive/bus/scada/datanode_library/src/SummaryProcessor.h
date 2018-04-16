/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datanode_library/src/SummaryProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The DataNodeActivationProcessor thread is responsible
  * for generating events and submitting
  * them.
  *
  */

#ifndef SUMMARYPROCESSOR_H
#define SUMMARYPROCESSOR_H

#include <map>
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datanode_library/src/SummaryRequest.h"
#include "core/threads/src/QueueProcessorPool.h"

#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	class IEntity;
}

namespace TA_Base_Bus
{
	class SummaryProcessor : public TA_Base_Core::QueueProcessorPoolCallback<SummaryRequest>
	{

	public:

		typedef std::map<unsigned long, TA_Base_Bus::IEntity*> EntityMap;

        static SummaryProcessor * getInstance();
    	static void removeInstance( SummaryProcessor *& removeMe );
	
		void setEntityMap(const EntityMap* entityMap);

        void queueItem( boost::shared_ptr<SummaryRequest>& item );

        void queueItemUnique( boost::shared_ptr<SummaryRequest>& item );

		void queueProcessorPoolCallback(boost::shared_ptr<SummaryRequest> newRequest);
		
	private:

        SummaryProcessor();
		~SummaryProcessor();

		static SummaryProcessor*                        m_singleton;
	    static int                                      m_count;
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

        const EntityMap* m_entityMap;

		TA_Base_Core::QueueProcessorPool<SummaryRequest, 
			TA_Base_Core::QueueProcessorWorker<SummaryRequest> >*     m_summaryProcessor;

        bool											m_logStats;
	};
}

#endif
