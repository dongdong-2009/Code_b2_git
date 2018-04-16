#include "TtisAutoSendConfigHandler.h"
#include "SingletonIniConfig.h"


std::string TtisAutoSendConfigHandler::TTISLIBDOWNLOAD	=	"TTISLIBDOWNLOAD";

std::string TtisAutoSendConfigHandler::AUTOSENDING_SECTION	= "AUTOSENDRESP";
std::string TtisAutoSendConfigHandler::TTISCMDRECVFAILED	=	"TTISCMDRECVFAILED";

std::string TtisAutoSendConfigHandler::STR_FALSE			= "0";
std::string TtisAutoSendConfigHandler::STR_TRUE				= "1";



TtisAutoSendConfigHandler::TtisAutoSendConfigHandler()
{
	
}

TtisAutoSendConfigHandler::~TtisAutoSendConfigHandler(void)
{
}

void TtisAutoSendConfigHandler::updateConfigFile (IConfigDataItemPtrType autoSendPtr)
{
	
	TTISAutoSendInfo * ttisAutoSendInfo = (TTISAutoSendInfo *)autoSendPtr.get();

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
																 TTISLIBDOWNLOAD,
																 convertToString(ttisAutoSendInfo->getAutoSendTTISLib()));

	SingletonIniConfig::getInstance()->setValueFromKeyAndSection(AUTOSENDING_SECTION,
		TTISCMDRECVFAILED,
		convertToString(ttisAutoSendInfo->getSendTtisCmdRecvFailed()));
}

std::string TtisAutoSendConfigHandler::convertToString (bool bConvert)
{
	char * pszdata = new char[10];

	sprintf(pszdata,"%d",bConvert);

	std::string strData = pszdata;
	delete [] pszdata;

	return strData;
}

IConfigDataItemPtrType TtisAutoSendConfigHandler::getAutoSendingData ()
{
	
	IConfigDataItemPtrType sharedPtrObj = m_dataFactory.createAutoSendDataInfo(ETTISType);
	TTISAutoSendInfo * ttisAutoSend = (TTISAutoSendInfo *)sharedPtrObj.get();

	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,TTISLIBDOWNLOAD).compare(STR_TRUE))
	{
		ttisAutoSend->setAutoSendTTISLib(TRUE);
	}
	else
	{
		ttisAutoSend->setAutoSendTTISLib(FALSE);
	}

	if (0 == SingletonIniConfig::getInstance()->getValueFromKeyAndSection(AUTOSENDING_SECTION,TTISCMDRECVFAILED).compare(STR_TRUE))
	{
		ttisAutoSend->setSendTtisCmdRecvFailed(TRUE);
	}
	else
	{
		ttisAutoSend->setSendTtisCmdRecvFailed(FALSE);
	}

	
	return sharedPtrObj;
}