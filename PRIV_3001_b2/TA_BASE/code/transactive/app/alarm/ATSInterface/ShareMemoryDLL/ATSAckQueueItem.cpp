// ATSAckQueueItem.cpp: implementation of the ATSAckQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ATSAckQueueItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ATSAckQueueItem::ATSAckQueueItem(const char* _alarmId, const char* _alarmAcknowledgeBy, unsigned long _ackTime)
{
	memset(m_alarmId, 0, ALARM_ID_SIZE);
	strcpy(m_alarmId, _alarmId);

	memset(m_alarmAcknowledgeBy, 0, ALARM_ACKNOWLEDGE_BY_SIZE);
	strcpy(m_alarmAcknowledgeBy, _alarmAcknowledgeBy);

	m_ackTime = _ackTime;
}

ATSAckQueueItem::ATSAckQueueItem() : m_ackTime(0)
{
	memset(m_alarmId, 0, ALARM_ID_SIZE);
	memset(m_alarmAcknowledgeBy, 0, ALARM_ACKNOWLEDGE_BY_SIZE);
}

ATSAckQueueItem::ATSAckQueueItem(const ATSAckQueueItem& atsAckQueueItem)
{
	memcpy(m_alarmId, atsAckQueueItem.m_alarmId, ALARM_ID_SIZE);
	memcpy(m_alarmAcknowledgeBy, atsAckQueueItem.m_alarmAcknowledgeBy, ALARM_ACKNOWLEDGE_BY_SIZE);
	m_ackTime = atsAckQueueItem.m_ackTime;
}

ATSAckQueueItem& ATSAckQueueItem::operator=(const ATSAckQueueItem& atsAckQueueItem)
{
	if (this == &atsAckQueueItem)
	{
		return *this;
	}

	memcpy(m_alarmId, atsAckQueueItem.m_alarmId, ALARM_ID_SIZE);
	memcpy(m_alarmAcknowledgeBy, atsAckQueueItem.m_alarmAcknowledgeBy, ALARM_ACKNOWLEDGE_BY_SIZE);
	m_ackTime = atsAckQueueItem.m_ackTime;

	return *this;
}

ATSAckQueueItem::~ATSAckQueueItem()
{
}
