// ATSMmsQueueItem.cpp: implementation of the ATSMmsQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ATSMmsQueueItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ATSMmsQueueItem::ATSMmsQueueItem(const char* _assetName, const char* _alarmId, const char* _operatorId, 
								 unsigned long targetDate, const char* _problemReport)
{
	memset(m_assetName, 0, ASSERT_NAME_SIZE);
	strcpy(m_assetName, _assetName);
	
	memset(m_alarmId, 0, ALARM_ID_SIZE);
	strcpy(m_alarmId, _alarmId);

	memset(m_operatorId, 0, OPERATOR_ID_SIZE);
	strcpy(m_operatorId, _operatorId);

	m_targetDate = targetDate;
	
	memset(m_problemReport, 0, ALARM_DESCRIPTION_SIZE);
	strcpy(m_problemReport, _problemReport);
}

ATSMmsQueueItem::ATSMmsQueueItem() : m_targetDate(0)
{
	memset(m_assetName, 0, ASSERT_NAME_SIZE);
	memset(m_alarmId, 0, ALARM_ID_SIZE);
	memset(m_operatorId, 0, OPERATOR_ID_SIZE);
	memset(m_problemReport, 0, ALARM_DESCRIPTION_SIZE);
}

ATSMmsQueueItem::~ATSMmsQueueItem()
{
}

ATSMmsQueueItem::ATSMmsQueueItem(const ATSMmsQueueItem& atsMmsQueueItem)
{
	memcpy(m_assetName, atsMmsQueueItem.m_assetName, ASSERT_NAME_SIZE);
	memcpy(m_alarmId, atsMmsQueueItem.m_alarmId, ALARM_ID_SIZE);
	memcpy(m_operatorId, atsMmsQueueItem.m_operatorId, OPERATOR_ID_SIZE);
	memcpy(m_problemReport, atsMmsQueueItem.m_problemReport, ALARM_DESCRIPTION_SIZE);
	m_targetDate = atsMmsQueueItem.m_targetDate;
}

ATSMmsQueueItem& ATSMmsQueueItem::operator=(const ATSMmsQueueItem& atsMmsQueueItem)
{
	if (this == &atsMmsQueueItem)
		return *this;

	memcpy(m_assetName, atsMmsQueueItem.m_assetName, ASSERT_NAME_SIZE);
	memcpy(m_alarmId, atsMmsQueueItem.m_alarmId, ALARM_ID_SIZE);
	memcpy(m_operatorId, atsMmsQueueItem.m_operatorId, OPERATOR_ID_SIZE);
	memcpy(m_problemReport, atsMmsQueueItem.m_problemReport, ALARM_DESCRIPTION_SIZE);
	m_targetDate = atsMmsQueueItem.m_targetDate;

	return *this;
}