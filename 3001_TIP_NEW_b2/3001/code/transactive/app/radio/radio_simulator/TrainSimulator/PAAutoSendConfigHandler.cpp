#include "PAAutoSendConfigHandler.h"
#include "SingletonIniConfig.h"


std::string PAAutoSendConfigHandler::PALIVE_KEY	=				"PALIVE";
std::string PAAutoSendConfigHandler::PAPRERECORDED_KEY	=		"PAPRERECORDED";
std::string PAAutoSendConfigHandler::PALIVERESET_KEY	=		"PALIVERESET";
std::string PAAutoSendConfigHandler::PAPRERECORDEDRESET_KEY	=	"PAPRERECORDEDRESET";
std::string PAAutoSendConfigHandler::PACONTINUE_KEY	=			"PACONTINUE";
std::string PAAutoSendConfigHandler::PAPRERECCOMPLETE_KEY	=	"PAPRERECCOMPLETE";
std::string PAAutoSendConfigHandler::PAPRERECEMERGENCY_KEY	=	"PAPRERECEMERGENCY";


std::string PAAutoSendConfigHandler::AUTOSENDING_SECTION	= "AUTOSENDRESP";

std::string PAAutoSendConfigHandler::STR_FALSE				= "0";
std::string PAAutoSendConfigHandler::STR_TRUE				= "1";



PAAutoSendConfigHandler::PAAutoSendConfigHandler()
{
	
}

PAAutoSendConfigHandler::~PAAutoSendConfigHandler(void)
{
}

void PAAutoSendConfigHandler::updateConfigFile (IConfigDataItemPtrType autoSendPtr)
{
	
	PAAutoSendInfo * paAutoSendInfo = (PAAutoSendInfo *)autoSendPtr.get();

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																 PALIVE_KEY,
																 convertToString(paAutoSendInfo->getAutoSendPALive()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																 PAPRERECORDED_KEY,
																 convertToString(paAutoSendInfo->getAutoSendPAPreRecorded()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																 PALIVERESET_KEY,
																 convertToString(paAutoSendInfo->getAutoSendPALiveReset()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																 PAPRERECORDEDRESET_KEY,
																 convertToString(paAutoSendInfo->getAutoSendPAPreRecordedReset()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																 PACONTINUE_KEY,
																 convertToString(paAutoSendInfo->getAutoSendPAContinue()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																PAPRERECCOMPLETE_KEY,
																convertToString(paAutoSendInfo->getAutoSendPAPreRecCompl()));
	
	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																PAPRERECEMERGENCY_KEY,
																convertToString(paAutoSendInfo->getAutoSendPAPreRecEmergency()));

}

std::string PAAutoSendConfigHandler::convertToString (bool bConvert)
{
	char * pszdata = new char[10];

	sprintf(pszdata,"%d",bConvert);

	std::string strData = pszdata;
	delete [] pszdata;

	return strData;
}

IConfigDataItemPtrType PAAutoSendConfigHandler::getAutoSendingData ()
{
	
	IConfigDataItemPtrType sharedPtrObj = m_dataFactory.createAutoSendDataInfo(EPAType);
	PAAutoSendInfo * paAutoSend = (PAAutoSendInfo *)sharedPtrObj.get();

	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,PALIVE_KEY).compare(STR_TRUE))
	{
		paAutoSend->setAutoSendPALive(TRUE);
	}
	else
	{
		paAutoSend->setAutoSendPALive(FALSE);
	}

	if (0 ==SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,PAPRERECORDED_KEY).compare(STR_TRUE))
	{
		paAutoSend->setAutoSendPAPreRecorded(TRUE);
	}
	else
	{
		paAutoSend->setAutoSendPAPreRecorded(FALSE);
	}

	if (0 ==SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,PALIVERESET_KEY).compare(STR_TRUE))
	{
		paAutoSend->setAutoSendPALiveReset(TRUE);
	}
	else
	{
		paAutoSend->setAutoSendPALiveReset(FALSE);
	}
	
	if (0 ==SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,PAPRERECORDEDRESET_KEY).compare(STR_TRUE))
	{
		paAutoSend->setAutoSendPAPreRecordedReset(TRUE);
	}
	else
	{
		paAutoSend->setAutoSendPAPreRecordedReset(FALSE);
	}

	if (0 ==SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,PACONTINUE_KEY).compare(STR_TRUE))
	{
		paAutoSend->setAutoSendPAContinue(TRUE);
	}
	else
	{
		paAutoSend->setAutoSendPAContinue(FALSE);
	}

	if (0 ==SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,PAPRERECCOMPLETE_KEY).compare(STR_TRUE))
	{
		paAutoSend->setAutoSendPAPreRecCompl(TRUE);
	}
	else
	{
		paAutoSend->setAutoSendPAPreRecCompl(FALSE);
	}

	if (0 ==SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,PAPRERECEMERGENCY_KEY).compare(STR_TRUE))
	{
		paAutoSend->setAutoSendPAPreRecEmergency(TRUE);
	}
	else
	{
		paAutoSend->setAutoSendPAPreRecEmergency(FALSE);
	}

	return sharedPtrObj;
}