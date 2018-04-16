#include "ScadaProxyLibUnitTestFactory.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
bool ScadaProxyLibUnitTestFactory::g_isInit = false;

void ScadaProxyLibUnitTestFactory::init() 
{
	if(g_isInit == false)
	{
		//std::string cmdLine = "--run-param-file=D:/LocalTesting/3001_TIP_NEW/unitTest_start.ini";
		//UtilityInitialiser::initialiseUtilities(cmdLine);
		std::string cmdLine = "--db-connection-file=DataPointLibUnitTest.csv --DataBase_225C955=DataBaseGood --location-key=50 --entity-name=Dt17PmsAgent";
		UtilityInitialiser::initialiseUtilities(cmdLine);
		::TA_Base_Core::gSetRuntimeBreakAction(BREAK_NO);
		::TA_Base_Core::gSetRuntimeAssertAction(FAIL_EXCEPTION);
		g_isInit = true;
	}
}
DataNodeEntityDataPtr ScadaProxyLibUnitTestFactory::createDataNodeEntityData()
{
	DataNodeEntityDataPtr rtn(new DataNodeEntityData(10023661)); 
	return rtn;
}

DataPointEntityDataPtr ScadaProxyLibUnitTestFactory::createAnalogueEntityData()
{
	DataPointEntityDataPtr rtn(new DataPointEntityData(10024251));
	return rtn;
}
DataPointEntityDataPtr ScadaProxyLibUnitTestFactory::createMeteredEntityData()
{
	DataPointEntityDataPtr rtn(new DataPointEntityData(10024349));
	return rtn;
}
DataPointEntityDataPtr ScadaProxyLibUnitTestFactory::createCombinedOutputEntityData()
{
	DataPointEntityDataPtr rtn(new DataPointEntityData(10024332));
	return rtn;
}
DataPointEntityDataPtr ScadaProxyLibUnitTestFactory::createBooleanEntityData()
{
	//10020405
	//10023859
	DataPointEntityDataPtr rtn(new DataPointEntityData(10020405));
	return rtn;
}
DataPointEntityDataPtr ScadaProxyLibUnitTestFactory::createDerivedEntityData()
{
	DataPointEntityDataPtr rtn(new DataPointEntityData(10024272));
	return rtn;
}
DataPointEntityDataPtr ScadaProxyLibUnitTestFactory::createTextEntityData()
{
	DataPointEntityDataPtr rtn(new DataPointEntityData(10024343));
	return rtn;
}
DataPointEntityDataPtr ScadaProxyLibUnitTestFactory::createEnumEntityData()
{
	DataPointEntityDataPtr rtn(new DataPointEntityData(10017674));
	return rtn;
}

IEntityUpdateEventProcessor* ScadaProxyLibUnitTestFactory::createEntityUpdateEventProcessor()
{
	StubEntityUpdateEventProcessor *rtn = new StubEntityUpdateEventProcessor();
	return rtn;
}

IProxyObserverNotifier* ScadaProxyLibUnitTestFactory::createProxyObserverNotifier()
{
	StubProxyObserverNotifier *rtn = new StubProxyObserverNotifier();
	return rtn;
}

IProxySmartPtrCallback* ScadaProxyLibUnitTestFactory::createProxySmartPtrCallback()
{
	StubProxySmartPtrCallback *rtn = new StubProxySmartPtrCallback();
	return rtn;
}

boost::shared_ptr<DpDynamicSyncUpdate>	ScadaProxyLibUnitTestFactory::createDpDynamicSyncUpdate()
{
	DpDynamicSyncUpdate * rtn = new DpDynamicSyncUpdate();
	rtn->updateType = ValueUpdate;

	rtn->value.dpType = DPT_BOOL_TYPE;
	rtn->value.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
	rtn->value.booleanValue = true;
	rtn->value.timeStamp.time = 1000;
	rtn->value.timeStamp.millitm = 100;

	rtn->alarmState = DptNoAlarm;

	rtn->alarmAckState = ScadaAlarmNack;

	rtn->controlState.controlState = noControl;
	rtn->controlState.lccResult ="Failed";
	rtn->controlState.rccResult = true;
	rtn->controlState.triggerringDpKey = 0;
	rtn->controlState.triggerringDpValue = "OPEN";

	rtn->inhibitState.inputInhibit = NoInputInhibit;
	rtn->inhibitState.outputInhibit = NoOutputInhibit;

	rtn->lastOperatorInfo.sessionID = "InternalProcessing";
	rtn->lastOperatorInfo.timeStamp.time = 10000;
	rtn->lastOperatorInfo.timeStamp.millitm = 100;

		rtn->syncCompleteState.value.dpType = DPT_BOOL_TYPE;
	rtn->syncCompleteState.value.quality = QUALITY_GOOD_NO_SPECIFIC_REASON;
	rtn->syncCompleteState.value.booleanValue = true;
	rtn->syncCompleteState.value.timeStamp.time = 1000;
	rtn->syncCompleteState.value.timeStamp.millitm = 100;

	rtn->syncCompleteState.alarmState = DptNoAlarm;

	rtn->syncCompleteState.alarmAckState = ScadaAlarmNack;

	rtn->syncCompleteState.controlState.controlState = noControl;
	rtn->syncCompleteState.controlState.lccResult ="Failed";
	rtn->syncCompleteState.controlState.rccResult = true;
	rtn->syncCompleteState.controlState.triggerringDpKey = 0;
	rtn->syncCompleteState.controlState.triggerringDpValue = "OPEN";

	rtn->syncCompleteState.inhibitState.inputInhibit = NoInputInhibit;
	rtn->syncCompleteState.inhibitState.outputInhibit = NoOutputInhibit;

	rtn->syncCompleteState.lastOperatorInfo.sessionID = "InternalProcessing";
	rtn->syncCompleteState.lastOperatorInfo.timeStamp.time = 10000;
	rtn->syncCompleteState.lastOperatorInfo.timeStamp.millitm = 100;

	return boost::shared_ptr<DpDynamicSyncUpdate> (rtn);
}
/*
boost::shared_ptr<DataPointProxy> ScadaProxyLibUnitTestFactory::getProxy()
{
	boost::shared_ptr<DataPointProxy> rtn;
	return rtn;
}*/