
#ifndef _IENTITYUPDATEEVENTPROCESSOR_HELP_H
#define _IENTITYUPDATEEVENTPROCESSOR_HELP_H

#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

namespace TA_Base_Bus
{
	class EntityUpdateEventProcessorHelp : public IEntityUpdateEventProcessor
	{
	public:
		EntityUpdateEventProcessorHelp(){}
		~EntityUpdateEventProcessorHelp(){}
		virtual void processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType) {};
	};
}

#endif //_IENTITYUPDATEEVENTPROCESSOR_HELP_H
