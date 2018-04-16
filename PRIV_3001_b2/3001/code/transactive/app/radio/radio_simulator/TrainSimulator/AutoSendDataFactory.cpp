
#include "AutoSendDataFactory.h"

AutoSendDataFactory::AutoSendDataFactory(void)
{
}

AutoSendDataFactory::~AutoSendDataFactory(void)
{
}

IConfigDataItemPtrType AutoSendDataFactory::createAutoSendDataInfo (EAutoSendType autoSendType)
{
	IConfigDataItem * autoSendInfo = NULL;

	switch(autoSendType)
	{
		case EPecType:
			autoSendInfo = createPecAutoSendInfo();
			break;
		case EPAType:
			autoSendInfo = createPAAutoSendInfo();
			break;
		case ETTISType:
			autoSendInfo = createTTISAutoSendInfo();
			break;
		case EChangeOver:
			autoSendInfo = createChangeOver ();
			break;

		case ETrainCount:
			autoSendInfo = createTrainCount ();
			break;

		case ERadioIdSetup:
			autoSendInfo = createRadioIdSetup();
			break;
	}

	IConfigDataItemPtrType sharePtrAutoSend(autoSendInfo);

	return sharePtrAutoSend;
}

IConfigDataItem * AutoSendDataFactory::createPecAutoSendInfo()
{
	IConfigDataItem * autoSendInfo = new PecAutoSendInfo ();
	return autoSendInfo;
}

IConfigDataItem * AutoSendDataFactory::createPAAutoSendInfo()
{
	IConfigDataItem * autoSendInfo = new PAAutoSendInfo ();
	return autoSendInfo;
}

IConfigDataItem * AutoSendDataFactory::createTTISAutoSendInfo ()
{
	IConfigDataItem * autoSendInfo = new TTISAutoSendInfo ();
	return autoSendInfo;
}

IConfigDataItem * AutoSendDataFactory::createChangeOver ()
{
	IConfigDataItem * autoSendInfo = new ChangeOverConfig ();
	return autoSendInfo;
}

IConfigDataItem * AutoSendDataFactory::createTrainCount ()
{
	IConfigDataItem * autoSendInfo = new TrainCountForAutoSend ();
	return autoSendInfo;
}

IConfigDataItem * AutoSendDataFactory::createRadioIdSetup ()
{
	IConfigDataItem * autoSendInfo = new RadioIdSetupConfig ();
	return autoSendInfo;
}