#include "ViTriggeringMap.h"
#include "ViTriggeringMapHelper.h"

ViTriggeringMap::ViTriggeringMap(unsigned long triggeringEntityKey)
    : m_triggeringEntityKey(triggeringEntityKey),
      m_cameraEntityKey(-1),
      m_valid(false)
{
}

ViTriggeringMap::ViTriggeringMap(unsigned long triggeringEntityKey,
                                 unsigned long cameraEntityKey)
    : m_triggeringEntityKey(triggeringEntityKey),
      m_cameraEntityKey(cameraEntityKey),
      m_valid(true)
{
}

ViTriggeringMap::~ViTriggeringMap(void)
{
}

unsigned long ViTriggeringMap::getTriggeringEntityKey()
{
	return m_triggeringEntityKey;
}

unsigned long ViTriggeringMap::getCameraEntityKey()
{
	if(!m_valid)
	{
		reload();
	}
	return m_cameraEntityKey;
}

void ViTriggeringMap::reload()
{
	ViTriggeringMapHelper helper;
	ViTriggeringMap t = helper.load(m_triggeringEntityKey);
	(*this) = t;
}

