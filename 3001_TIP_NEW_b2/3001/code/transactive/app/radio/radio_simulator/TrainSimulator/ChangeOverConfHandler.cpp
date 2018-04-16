#include "ChangeOverConfHandler.h"
#include "SingletonIniConfig.h"


std::string ChangeOverConfHandler::CHANGEOVER_KEY	=				"FAILURE_SOURCE_STATUS";
std::string ChangeOverConfHandler::CHANGEOVER_TTISKEY		=		"TTIS_LIB";
std::string ChangeOverConfHandler::CHANGEOVER_SECTION	=			"CHANGEOVER";
std::string ChangeOverConfHandler::CHANGEOVER_PECKEY	=			"PEC";
std::string ChangeOverConfHandler::CHANGEOVER_PALIVESKEY	=		"PALIVE";
std::string ChangeOverConfHandler::CHANGEOVER_PAPRERECKEY	=		"PAPREREC";
std::string ChangeOverConfHandler::CHANGEOVER_TTISUPGRADEKEY	=	"UPGRADETTIS";

std::string ChangeOverConfHandler::STR_FALSE			= "0";
std::string ChangeOverConfHandler::STR_TRUE				= "1";



ChangeOverConfHandler::ChangeOverConfHandler()
{
	
}

ChangeOverConfHandler::~ChangeOverConfHandler(void)
{
}

void ChangeOverConfHandler::updateConfigFile (IConfigDataItemPtrType autoSendPtr)
{
	
	ChangeOverConfig * changeOver = (ChangeOverConfig *)autoSendPtr.get();

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(CHANGEOVER_SECTION,
																 CHANGEOVER_KEY,
																 convertToString(changeOver->getStatusFailure()));
	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(CHANGEOVER_SECTION,
																 CHANGEOVER_TTISKEY,
																 convertToString(changeOver->getTtisStatusFailure()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(CHANGEOVER_SECTION,
		CHANGEOVER_PECKEY,
		convertToString(changeOver->getPECAnsStatusFailure()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(CHANGEOVER_SECTION,
		CHANGEOVER_PALIVESKEY,
		convertToString(changeOver->getPALiveStatusFailure()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(CHANGEOVER_SECTION,
		CHANGEOVER_PAPRERECKEY,
		convertToString(changeOver->getPAPreRecStatusFailure()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(CHANGEOVER_SECTION,
		CHANGEOVER_TTISUPGRADEKEY,
		convertToString(changeOver->getUpgradeTtisStatusFailure()));
	
}

std::string ChangeOverConfHandler::convertToString (bool bConvert)
{
	char * pszdata = new char[10];

	sprintf(pszdata,"%d",bConvert);

	std::string strData = pszdata;
	delete [] pszdata;

	return strData;
}

IConfigDataItemPtrType ChangeOverConfHandler::getAutoSendingData ()
{
	
	IConfigDataItemPtrType sharedPtrObj = m_dataFactory.createAutoSendDataInfo(EChangeOver);
	ChangeOverConfig * changeOver = (ChangeOverConfig *)sharedPtrObj.get();

	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(CHANGEOVER_SECTION,CHANGEOVER_KEY).compare(STR_TRUE))
	{
		changeOver->setStatusFailure(1);
	}
	else
	{
		changeOver->setStatusFailure(0);
	}
	
	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(CHANGEOVER_SECTION,CHANGEOVER_TTISKEY).compare(STR_TRUE))
	{
		changeOver->setTtisStatusFailure(1);
	}
	else
	{
		changeOver->setTtisStatusFailure(0);
	}
	
	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(CHANGEOVER_SECTION,CHANGEOVER_PECKEY).compare(STR_TRUE))
	{
		changeOver->setPECAnsStatusFailure(1);
	}
	else
	{
		changeOver->setPECAnsStatusFailure(0);
	}

	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(CHANGEOVER_SECTION,CHANGEOVER_PALIVESKEY).compare(STR_TRUE))
	{
		changeOver->setPALiveStatusFailure(1);
	}
	else
	{
		changeOver->setPALiveStatusFailure(0);
	}

	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(CHANGEOVER_SECTION,CHANGEOVER_PAPRERECKEY).compare(STR_TRUE))
	{
		changeOver->setPAPreRecStatusFailure(1);
	}
	else
	{
		changeOver->setPAPreRecStatusFailure(0);
	}
	
	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(CHANGEOVER_SECTION,CHANGEOVER_TTISUPGRADEKEY).compare(STR_TRUE))
	{
		changeOver->setUpgradeTtisStatusFailure(1);
	}
	else
	{
		changeOver->setUpgradeTtisStatusFailure(0);
	}


	return sharedPtrObj;
}