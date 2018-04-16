/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/bus/response_plans/common/src/RpcAgency.h $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/02/06 18:35:32 $
  * Last modified by:  $Author: qi.huang $
  *
  **/

#ifndef RPCAGENCY_H__INCLUDED_
#define RPCAGENCY_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>
#include <set>

#include "ace/Thread_Mutex.h"
#include "ace/Condition_Thread_Mutex.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/response_plans/common/src/IAgency.h"


namespace TA_Base_Bus
{	
    class MessageSender;
    class MessageReceiver;
    class IReqHandler;
    /////////////////////////////////////////////////////////////////////////////
    // RpcAgency

    class RpcAgency : public IAgency
    {
    // Operations
    public:
        ~RpcAgency();

        static RpcAgency& getInstance();
        static void setInstance( RpcAgency& refAgency );
        static void createInstance( const unsigned int ulLocation, const unsigned int ulHandler );
        static void destroyInstance();

        virtual void handleAck( const unsigned int ulToLocation, const unsigned int unId );
        virtual void handleReq( const unsigned int ulToLocation, const unsigned int unId, const int eType, 
            const unsigned int ulPlanLocation, const unsigned int ulPlanId, const unsigned int ulInstanceNum, 
            const unsigned int ulStepNum, const unsigned int ulHandler, const std::vector<std::string>& vecParameters );

        void asyncSendAck( const unsigned int ulToLocation, const unsigned int unId );

        void asyncSendReq( const unsigned int ulToLocation, const int eType, const unsigned int ulPlanLocation, 
            const unsigned int ulPlanId, const unsigned int ulInstanceNum, const unsigned int ulStepNum, 
            const unsigned int ulHandler, const std::vector<std::string>& vecParameters );

        bool syncSendReq( const unsigned int ulToLocation, const int eType, const unsigned int ulPlanLocation, 
            const unsigned int ulPlanId, const unsigned int ulInstanceNum, const unsigned int ulStepNum,
            const unsigned int ulHandler, const std::vector<std::string>& vecParameters, const unsigned int unMsTimeout = 5000u );

        void subscribe( IReqHandler& refCallback );
        void unsubscribe( IReqHandler& refCallback );

    private:
        RpcAgency();
        RpcAgency( const unsigned int ulLocation, const unsigned int ulHandler );
        RpcAgency(const RpcAgency& other);
        RpcAgency& operator=(const RpcAgency& other);

    protected:
        void initSender();
        void initReceiver( const bool bRequest = false );
        void uninitSender();
        void uninitReceiver();
        unsigned int getNewReqId();

    protected:
        // Handler id is workstation id (MFT entity key)
        const unsigned int m_ulHandler;
        // Location key of the agent
        const unsigned int m_ulLocation;
        unsigned int m_unReqId;
        MessageSender* m_pSender;
        MessageReceiver* m_pReceiver;
        ACE_Condition_Thread_Mutex* m_pAckNotify;
        std::set<unsigned int> m_setAckedReqIds;
        std::set<IReqHandler*> m_setReqCallbacks;

    // Attributes
    private:
        static RpcAgency *s_instance;
        static ACE_Thread_Mutex s_singletonLock;
    };
}

#endif // !defined(RPCAGENCY_H__INCLUDED_)
