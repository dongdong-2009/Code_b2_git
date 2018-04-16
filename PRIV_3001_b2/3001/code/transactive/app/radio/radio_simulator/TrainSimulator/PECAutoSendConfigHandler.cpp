#include "PECAutoSendConfigHandler.h"
#include "SingletonIniConfig.h"


std::string PECAutoSendConfigHandler::PECANSWER_KEY	=	 "PECANSWER";
std::string PECAutoSendConfigHandler::PECHOLD_KEY	=	 "PECHOLD";
std::string PECAutoSendConfigHandler::PECRESET_KEY	=	 "PECRESET";

std::string PECAutoSendConfigHandler::AUTOSENDING_SECTION	= "AUTOSENDRESP";

std::string PECAutoSendConfigHandler::STR_FALSE				= "0";
std::string PECAutoSendConfigHandler::STR_TRUE				= "1";



PECAutoSendConfigHandler::PECAutoSendConfigHandler()
{
	
}

PECAutoSendConfigHandler::~PECAutoSendConfigHandler(void)
{
}

void PECAutoSendConfigHandler::updateConfigFile (IConfigDataItemPtrType autoSendPtr)
{
	
	PecAutoSendInfo * pecAutoSendInfo = (PecAutoSendInfo *)autoSendPtr.get();

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																 PECANSWER_KEY,
																 convertToString(pecAutoSendInfo->getAutoSendPecAnswer()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																 PECHOLD_KEY,
																 convertToString(pecAutoSendInfo->getAutoSendPecHold()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																 PECRESET_KEY,
																 convertToString(pecAutoSendInfo->getAutoSendPecReset()));
}

std::string PECAutoSendConfigHandler::convertToString (bool bConvert)
{
	char * pszdata = new char[10];

	sprintf(pszdata,"%d",bConvert);

	std::string strData = pszdata;
	delete [] pszdata;

	return strData;
}

IConfigDataItemPtrType PECAutoSendConfigHandler::getAutoSendingData ()
{
	
	IConfigDataItemPtrType sharedPtrObj = m_dataFactory.createAutoSendDataInfo(EPecType);
	PecAutoSendInfo * pecAutoSend = (PecAutoSendInfo *)sharedPtrObj.get();

	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,PECANSWER_KEY).compare(STR_TRUE))
	{
		pecAutoSend->setAutoSendPecAnswer(TRUE);
	}
	else
	{
		pecAutoSend->setAutoSendPecAnswer(FALSE);
	}

	if (0 ==SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,PECHOLD_KEY).compare(STR_TRUE))
	{
		pecAutoSend->setAutoSendPecHold(TRUE);
	}
	else
	{
		pecAutoSend->setAutoSendPecHold(FALSE);
	}

	if (0 ==SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,PECRESET_KEY).compare(STR_TRUE))
	{
		pecAutoSend->setAutoSendPecReset(TRUE);
	}
	else
	{
		pecAutoSend->setAutoSendPecReset(FALSE);
	}

	return sharedPtrObj;
}