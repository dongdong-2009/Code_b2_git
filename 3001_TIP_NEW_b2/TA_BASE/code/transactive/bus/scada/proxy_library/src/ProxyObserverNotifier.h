/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/proxy_library/src/ProxyObserverNotifier.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// ProxyObserverNotifier.h: interface for the ProxyObserverNotifier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROXYOBSERVERNOTIFIER_H__C449CAF3_E239_48D5_8D9C_AF52DCF2B6B3__INCLUDED_)
#define AFX_PROXYOBSERVERNOTIFIER_H__C449CAF3_E239_48D5_8D9C_AF52DCF2B6B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/proxy_library/src/ObserverUpdateQueueProcessor.h"

namespace TA_Base_Bus
{
    class ObserverUpdateQueueProcessor;
    class IEntityUpdateEventProcessor;
    
    class IProxyObserverNotifier
    {
    public:
        virtual ~IProxyObserverNotifier() {};
        virtual void addObserver(IEntityUpdateEventProcessor& observer) = 0;
        virtual void removeObserver(IEntityUpdateEventProcessor& observer) = 0;
        virtual void notifyObservers(ScadaEntityUpdateType updateType) = 0;
        virtual bool isObserved() = 0;
    };
    
    class ProxyObserverNotifier : public IProxyObserverNotifier  
    {
    public:
        ProxyObserverNotifier(ObserverUpdateQueueProcessor& observerUpdateQueueProcessor);
        virtual ~ProxyObserverNotifier();

        void queueNotification(ScadaEntityUpdateType updateType);
        
        // implement IProxyObserverNotifier functions
        virtual void addObserver(IEntityUpdateEventProcessor& observer);
        virtual void removeObserver(IEntityUpdateEventProcessor& observer);
        virtual void notifyObservers(ScadaEntityUpdateType updateType);
        virtual bool isObserved();
        virtual unsigned long getEntityKey() const = 0;
        
    private:
        ObserverUpdateQueueProcessor& m_observerUpdateQueueProcessor;
        
        typedef std::map<IEntityUpdateEventProcessor*, unsigned long> ObserverMap;//<observer pointer, observer reference count>
        
        ObserverMap m_observers; 
        TA_Base_Core::ReEntrantThreadLockable		m_observerLock;
    };
}

#endif // !defined(AFX_PROXYOBSERVERNOTIFIER_H__C449CAF3_E239_48D5_8D9C_AF52DCF2B6B3__INCLUDED_)
