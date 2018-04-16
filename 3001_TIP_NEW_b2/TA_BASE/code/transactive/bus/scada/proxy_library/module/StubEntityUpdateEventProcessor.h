
#ifndef STUB_ENTITY_UPDATE_EVENT_PROCESSOR
#define STUB_ENTITY_UPDATE_EVENT_PROCESSOR

#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

class StubEntityUpdateEventProcessor: TA_Base_Bus::IEntityUpdateEventProcessor
{
public:
	void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType)
	{
	}
};

#endif

