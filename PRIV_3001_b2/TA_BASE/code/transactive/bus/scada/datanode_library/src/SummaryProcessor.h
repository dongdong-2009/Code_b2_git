/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/scada/datanode_library/src/SummaryProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * The DataNodeActivationProcessor thread is responsible
  * for generating events and submitting
  * them.
  *
  */

#ifndef SUMMARYPROCESSOR_H
#define SUMMARYPROCESSOR_H

#include "bus/scada/datanode_library/src/SummaryRequest.h"
#include "bus/scada/datanode_library/src/SummaryQueueHandler.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	class SummaryProcessor 
	{
	public:
        static SummaryProcessor * getInstance();
    	static void removeInstance( SummaryProcessor *& removeMe );
		void setEntityMap(const std::map<unsigned long, IEntity*>* entityMap);
        void queueItemUnique(SummaryRequest item, bool forceHandle = false);
		void setOperatorMode(TA_Base_Core::EOperationMode operationMode);

	private:

        SummaryProcessor();
		~SummaryProcessor();

		static SummaryProcessor*                        m_singleton;
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		SummaryQueueHandler*							m_queueHandler;
	};
}

#endif
