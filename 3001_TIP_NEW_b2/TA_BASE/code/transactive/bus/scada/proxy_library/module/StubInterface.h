#ifndef STUB_PROXY_OBSERVER_NOTIFIER_H__
#define STUB_PROXY_OBSERVER_NOTIFIER_H__
#include "bus/scada/proxy_library/src/ProxyObserverNotifier.h "
#include "bus/scada/proxy_library/src/IProxySmartPtrCallback.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "StubControl.h"
using namespace TA_Base_Bus;
class StubProxyObserverNotifier: public TA_Base_Bus::IProxyObserverNotifier
{
public:
    void addObserver(IEntityUpdateEventProcessor& observer){};
    void removeObserver(IEntityUpdateEventProcessor& observer){};
    void notifyObservers(ScadaEntityUpdateType updateType)
	{
		if(StubControl::getInstance().isSet("UnknownException"))
		{
			throw 1;
		}
	};
    bool isObserved(){return true;};
};

class StubEntityUpdateEventProcessor: public TA_Base_Bus::IEntityUpdateEventProcessor
{
public:
	virtual void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType){};
};

class StubProxySmartPtrCallback:public TA_Base_Bus::IProxySmartPtrCallback
{
public:
	virtual void onCreateProxy(IProxyObserverNotifier* observerNotifier, IEntityUpdateEventProcessor& observer){};

	virtual void onCopyProxy(IProxyObserverNotifier* observerNotifier, IEntityUpdateEventProcessor& observer){};

	virtual void onDeleteProxy(IProxyObserverNotifier* observerNotifier, IEntityUpdateEventProcessor& observer){};
};
#endif