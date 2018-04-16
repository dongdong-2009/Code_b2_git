#include "RadioIdSetupConfigHandler.h"
#include "SingletonIniConfig.h"


std::string RadioIdSetupConfigHandler::RADIOIDSETUP_SECTION	=				"RADIOIDSETUP";
std::string RadioIdSetupConfigHandler::RADIOIDSETUP_KEY	=					"USEPRIMARY";

std::string RadioIdSetupConfigHandler::STR_FALSE				= "0";
std::string RadioIdSetupConfigHandler::STR_TRUE				= "1";



RadioIdSetupConfigHandler::RadioIdSetupConfigHandler()
{
	
}

RadioIdSetupConfigHandler::~RadioIdSetupConfigHandler(void)
{
}

void RadioIdSetupConfigHandler::updateConfigFile (IConfigDataItemPtrType autoSendPtr)
{
	
	RadioIdSetupConfig * radioSetup = (RadioIdSetupConfig *)autoSendPtr.get();

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(RADIOIDSETUP_SECTION,
																 RADIOIDSETUP_KEY,
																 convertToString(radioSetup->getRadioConfigValue()));

}

std::string RadioIdSetupConfigHandler::convertToString (bool bConvert)
{
	char * pszdata = new char[10];

	sprintf(pszdata,"%d",bConvert);

	std::string strData = pszdata;
	delete [] pszdata;

	return strData;
}

IConfigDataItemPtrType RadioIdSetupConfigHandler::getAutoSendingData ()
{
	
	IConfigDataItemPtrType sharedPtrObj = m_dataFactory.createAutoSendDataInfo(ERadioIdSetup);
	RadioIdSetupConfig * radioConfig = (RadioIdSetupConfig *)sharedPtrObj.get();

	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(RADIOIDSETUP_SECTION,RADIOIDSETUP_KEY).compare(STR_TRUE))
	{
		radioConfig->setRadioConfigValue(TRUE);
	}
	else
	{
		radioConfig->setRadioConfigValue(FALSE);
	}

	return sharedPtrObj;
}