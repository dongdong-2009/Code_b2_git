#include "stdafx.h"
#include "MessageHandler.h"

IMessageHandler::IMessageHandler()
:m_inputData(""),m_outputData(""),m_extInfo(SimulatorConfig::getInstance()->m_extInfo)
{
}

IMessageHandler::~IMessageHandler()
{
	m_extInfo.clear();
}
