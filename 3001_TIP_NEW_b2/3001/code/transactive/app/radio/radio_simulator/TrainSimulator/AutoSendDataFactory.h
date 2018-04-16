#pragma once

#include "AutoSendDataModel.h"


class AutoSendDataFactory
{
	public:
		AutoSendDataFactory(void);
		~AutoSendDataFactory(void);

		IConfigDataItemPtrType createAutoSendDataInfo (EAutoSendType autoSendType);

	private:
		IConfigDataItem * createPecAutoSendInfo();
		IConfigDataItem * createPAAutoSendInfo();
		IConfigDataItem * createTTISAutoSendInfo ();
		IConfigDataItem * createChangeOver ();
		IConfigDataItem * createTrainCount ();
		IConfigDataItem * createRadioIdSetup ();
};
