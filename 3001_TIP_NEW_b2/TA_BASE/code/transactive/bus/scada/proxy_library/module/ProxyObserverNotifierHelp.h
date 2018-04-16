#ifndef PROXY_OBSERVER_NOTIFIER_HELP_H
#define PROXY_OBSERVER_NOTIFIER_HELP_H

#include "bus/scada/proxy_library/src/ProxyObserverNotifier.h"


#include "string.h"

namespace TA_Base_Bus
{

	class ProxyObserverNotifierHelp : public IProxyObserverNotifier
	{
	public:
		ProxyObserverNotifierHelp() {}
		~ProxyObserverNotifierHelp() {}
		virtual void addObserver(IEntityUpdateEventProcessor& observer) {}
		virtual void removeObserver(IEntityUpdateEventProcessor& observer) {}
		virtual void notifyObservers(ScadaEntityUpdateType updateType) {}
		virtual bool isObserved() {return true;}
	};
}
#endif //PROXY_OBSERVER_NOTIFIER_HELP_H
