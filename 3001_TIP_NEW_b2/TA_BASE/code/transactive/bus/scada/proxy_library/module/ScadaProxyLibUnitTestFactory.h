#ifndef _SCADA_PROXY_UNIT_TEST_FACTORY_H__
#define _SCADA_PROXY_UNIT_TEST_FACTORY_H__
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "StubInterface.h"
//#include "StubEntityUpdateEventProcessor.h"
using namespace TA_Base_Core;
using namespace TA_Base_Bus;
class ScadaProxyLibUnitTestFactory
{
public:
	DataNodeEntityDataPtr createDataNodeEntityData();
	DataPointEntityDataPtr createAnalogueEntityData();
	DataPointEntityDataPtr createMeteredEntityData();
	DataPointEntityDataPtr createCombinedOutputEntityData();
	DataPointEntityDataPtr createBooleanEntityData();
	DataPointEntityDataPtr createDerivedEntityData();
	DataPointEntityDataPtr createTextEntityData();
	DataPointEntityDataPtr createEnumEntityData();
	
	IProxyObserverNotifier* createProxyObserverNotifier();
	IProxySmartPtrCallback* createProxySmartPtrCallback();
	IEntityUpdateEventProcessor* createEntityUpdateEventProcessor();
	boost::shared_ptr<DpDynamicSyncUpdate>	createDpDynamicSyncUpdate();
public:
	static void init();
private:
	static bool g_isInit;
};

#endif