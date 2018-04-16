
#include "EtcTrainSimConfigHandler.h"

#include "SingletonIniConfig.h"

EtcTrainSimConfigHandler * EtcTrainSimConfigHandler::m_iniConfInstance = NULL;
TA_Base_Core::ReEntrantThreadLockable EtcTrainSimConfigHandler::m_threadLock;

EtcTrainSimConfigHandler::EtcTrainSimConfigHandler(void)
{

	m_pecHandler = new PECAutoSendConfigHandler();
	m_paHandler = new PAAutoSendConfigHandler();
	m_ttisHandler = new TtisAutoSendConfigHandler();
	m_changeOverhandler = new ChangeOverConfHandler();
	m_radioConfHandler = new RadioIdSetupConfigHandler();
}

EtcTrainSimConfigHandler::~EtcTrainSimConfigHandler(void)
{
	delete m_paHandler;
	delete m_pecHandler;
	delete m_ttisHandler;
	delete m_changeOverhandler;
}

EtcTrainSimConfigHandler * EtcTrainSimConfigHandler::getInstance ()
{
	{
		TA_THREADGUARD (m_threadLock);

		if (!m_iniConfInstance)
		{
			m_iniConfInstance = new EtcTrainSimConfigHandler ();
		}
	}

	return m_iniConfInstance;
}

void EtcTrainSimConfigHandler::deleteInstace ()
{
	{
		TA_THREADGUARD (m_threadLock);

		if (!m_iniConfInstance)
		{
			delete m_iniConfInstance;
			m_iniConfInstance = NULL;
		}
	}
}

void EtcTrainSimConfigHandler::updateEtcCConfiguration (IConfigDataItemPtrType autoSendDataPtr)
{
	switch (autoSendDataPtr->getType())
	{
		case EPecType:
			m_pecHandler->updateConfigFile(autoSendDataPtr);
			break;
		case EPAType:
			m_paHandler->updateConfigFile(autoSendDataPtr);
			break;
		case ETTISType:
			m_ttisHandler->updateConfigFile(autoSendDataPtr);
			break;
		case EChangeOver:
			m_changeOverhandler->updateConfigFile(autoSendDataPtr);
			break;
		case ETrainCount:
			m_trainCountHandler->updateConfigFile(autoSendDataPtr);
			break;
		case ERadioIdSetup:
			m_radioConfHandler->updateConfigFile(autoSendDataPtr);
			break;
	}
}

IConfigDataItemPtrType EtcTrainSimConfigHandler::getAutoSendingData (EAutoSendType autoSendType)
{
	return createAutoSendingData(autoSendType);
}

IConfigDataItemPtrType EtcTrainSimConfigHandler::createAutoSendingData (EAutoSendType autoSendType)
{
	IConfigDataItemPtrType sharedPtrObj;
	
	switch (autoSendType)
	{
		case EPecType:
			sharedPtrObj = m_pecHandler->getAutoSendingData();
			break;
		case EPAType:
			sharedPtrObj = m_paHandler->getAutoSendingData();
			break;
		case ETTISType:
			sharedPtrObj = m_ttisHandler->getAutoSendingData();
			break;
		case EChangeOver:
			sharedPtrObj = m_changeOverhandler->getAutoSendingData();
			break;

		case ETrainCount:
			sharedPtrObj = m_trainCountHandler->getAutoSendingData();
			break;

		case ERadioIdSetup:
			sharedPtrObj = m_radioConfHandler->getAutoSendingData();
			break;
	}

	return sharedPtrObj;
}

std::vector<unsigned long> EtcTrainSimConfigHandler::getControlledLocation (bool bRelease)
{
	std::vector<unsigned long> vctrRet;
	
	std::string strData;

	if (bRelease)
		strData = SingletonIniConfig::getInstance()->getValueFromKeyAndSection("DUTY","RELEASE_LOCATION");
	else
		strData = SingletonIniConfig::getInstance()->getValueFromKeyAndSection("DUTY","CONTROL_LOCATION");

	vctrRet = parseData(strData);

	return vctrRet;
}

std::vector <unsigned long> EtcTrainSimConfigHandler::parseData (const std::string & strData)
{

	std::string strTemp = strData;
	std::vector <unsigned long> vctrReturn;

	int nPos = strTemp.find(",");

	while (nPos != strTemp.npos)
	{
		// get the first value

		std::string strVal = strTemp.substr(0,nPos);
		strTemp.erase(0,  nPos + 1);

		vctrReturn.push_back (stringToLong(strVal));

		nPos = strTemp.find(",");

		// add the last item on the vector
		if (nPos == strTemp.npos)
		{
			vctrReturn.push_back ((unsigned long)stringToLong(strTemp));
		}		
	}

	return vctrReturn;
}

long EtcTrainSimConfigHandler::stringToLong (const std::string & strData)
{
	
	long lData;

	lData = atol (strData.c_str());

	return lData;
}


long EtcTrainSimConfigHandler::getChangeOverFailureStatus ()
{
	long lStatus = 0;


	return lStatus;
}