/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/proxy_library/src/DataPointLocalSynchronizer.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  DirectAgentSynchronisationInterface.h
//  Implementation of the Class DirectAgentSynchronisationInterface
//  Created on:      28-Jun-2004 03:44:08 PM
///////////////////////////////////////////////////////////

#if !defined(DataPointEventDrivenAgentSyncIf_C8B57EB2_D403_431e_B32A_3278BD7772B7__INCLUDED_)
#define DataPointEventDrivenAgentSyncIf_C8B57EB2_D403_431e_B32A_3278BD7772B7__INCLUDED_

#include <map>
#include <set>

#include "bus/scada/proxy_library/src/AgentSynchronisationInterface.h"

#include "bus/scada/datapoint_library/src/IDataPointObserverNotification.h"

/**
 * This class is used to synchronise datapoints that are managed in the same
 * process context.  There will only be one instance of this type.
 * @version 1.0
 * @created 28-Jun-2004 03:44:08 PM
 */

namespace TA_Base_Bus
{
    class DataPointSyncManager;
    class DataPointProxyManager;
    
    class DataPointLocalSynchronizer : public AgentSynchronisationInterface,
                                            public TA_Base_Bus::IDataPointObserverNotification
    {
    public:
        DataPointLocalSynchronizer(const std::string& agentName,
										unsigned long location,
										DataPointSyncManager& manager,
										DataPointProxyManager& callback);
        
        virtual ~DataPointLocalSynchronizer();

		//
		// Operations required by IDataPointObserverNotification
		//
		virtual void processDataPointUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails );

        /**
         * Define a run() method to do your work. This should block until terminate() is
         * called eg: CorbaUtil::GetInstance().Run() or m_dialog->DoModal()  NOTE: do not
         * call this method directly. It needs to be public so that it can be called from
         * the thread-spawning function
         */
        virtual void run();

      protected:

        void cancelAllSubscription();

        std::set<unsigned long>             m_entityKeyList;
        TA_Base_Core::ReEntrantThreadLockable   m_messageQueueLock;

      private:
          DataPointSyncManager& m_manager;
          DataPointProxyManager& m_callback;
          
    };
};
#endif // !defined(DataPointEventDrivenAgentSyncIf_C8B57EB2_D403_431e_B32A_3278BD7772B7__INCLUDED_)
