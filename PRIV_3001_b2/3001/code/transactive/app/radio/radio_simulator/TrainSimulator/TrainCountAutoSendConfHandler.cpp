#include "TrainCountAutoSendConfHandler.h"
#include "SingletonIniConfig.h"


std::string TrainCountAutoSendConfHandler::TRAINCOUNT_SECTION	=			"TRAIN_COUNT_INVOLVE_ON_AUTOSEND";
std::string TrainCountAutoSendConfHandler::VALUE_KEY	=					"VALUE";

std::string TrainCountAutoSendConfHandler::STR_FALSE			= "0";
std::string TrainCountAutoSendConfHandler::STR_TRUE				= "1";



TrainCountAutoSendConfHandler::TrainCountAutoSendConfHandler()
{
	
}

TrainCountAutoSendConfHandler::~TrainCountAutoSendConfHandler(void)
{
}

void TrainCountAutoSendConfHandler::updateConfigFile (IConfigDataItemPtrType autoSendPtr)
{
	
	TrainCountForAutoSend * trainCountHandler = (TrainCountForAutoSend *)autoSendPtr.get();

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(TRAINCOUNT_SECTION,
																 VALUE_KEY,
																 trainCountHandler->getTrainCount().c_str());
	
	
}

std::string TrainCountAutoSendConfHandler::convertToString (bool bConvert)
{
	char * pszdata = new char[10];

	sprintf(pszdata,"%d",bConvert);

	std::string strData = pszdata;
	delete [] pszdata;

	return strData;
}

IConfigDataItemPtrType TrainCountAutoSendConfHandler::getAutoSendingData ()
{
	
	IConfigDataItemPtrType sharedPtrObj = m_dataFactory.createAutoSendDataInfo(ETrainCount);
	TrainCountForAutoSend * trainCount = (TrainCountForAutoSend *)sharedPtrObj.get();

	std::string strValue = SingletonIniConfig::getInstance()->getValueFromKeyAndSection(TRAINCOUNT_SECTION,VALUE_KEY);

	trainCount->setTrainCount(strValue);
	
	return sharedPtrObj;
}