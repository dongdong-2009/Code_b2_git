#include "StdAfx.h"
#include "CommonDefine.h"
#include "RequestHandler.h"
#include "CommandHandler.h"

using namespace I_ATS_ISCS_ALARM;

extern IATSAlarm *g_pAtsInterface;

CRequestHandler::CRequestHandler(CATSListCtrl* pListCtrl) 
				: m_bTerminate(false)
				, m_pAtsListCtrl(0)
{
	m_pAtsListCtrl = pListCtrl;
}

CRequestHandler::~CRequestHandler(void)
{
}

void CRequestHandler::run()
{
	ProfileInfo profileOld;
	CCommandHandler::getInstance().getProfileInfo(profileOld);
	while(!m_bTerminate)
	{
		if(g_pAtsInterface)
		{
			//retrieve ack
			std::list<AlarmAckUpdate*> AckupdateList;
			g_pAtsInterface->retrieveATSAlarmAckUpdate(AckupdateList);
			for(std::list<AlarmAckUpdate*>::iterator itr = AckupdateList.begin();
				itr != AckupdateList.end(); 
				++itr)
			{
				if (m_pAtsListCtrl)
				{
					AlarmAckUpdate* pAck = *itr;
					I_ATS_ISCS_ALARM::AlarmDetail* pAlarmDetail = m_pAtsListCtrl->getAlarmDetails(pAck->alarmId);
					if (pAlarmDetail)
					{
						pAlarmDetail->alarmAcknowledgeBy = pAck->alarmAcknowledgeBy;
						pAlarmDetail->ackTime = pAck->ackTime;
						CCommandHandler::getInstance().acknowledgeAlarm(pAlarmDetail);
					}
					delete pAck;
					pAck = NULL;
				}
			}

			//retrieve mms
			std::list<MmsJobRequest*> MmsupdateList;
			g_pAtsInterface->retrieveATSMmsJobRequest(MmsupdateList);
			for(std::list<MmsJobRequest*>::iterator itr1 = MmsupdateList.begin();
				itr1 != MmsupdateList.end(); 
				++itr1)
			{
				if (m_pAtsListCtrl)
				{
					MmsJobRequest* mmsJobReq = *itr1;
					I_ATS_ISCS_ALARM::AlarmDetail* pAlarmDetail = m_pAtsListCtrl->getAlarmDetails(mmsJobReq->AlarmId);
					if (pAlarmDetail)
					{
						pAlarmDetail->mmsState = I_ATS_ISCS_ALARM::MMS_JR_SENT;
						CCommandHandler::getInstance().updateMMSState(pAlarmDetail);
					}
					delete mmsJobReq;
					mmsJobReq = NULL;
				}
			}

			ProfileInfo profile;
			g_pAtsInterface->getProfileInfo(profile);
			if ( profileOld.updateTime != profile.updateTime )
			{
				profileOld.operatorId = profile.operatorId;
				profileOld.profileId = profile.profileId;
				profileOld.loginLocation = profile.loginLocation;
				profileOld.displayOnly = profile.displayOnly;
				profileOld.updateTime = profile.updateTime;

				CCommandHandler::getInstance().setProfileInfo(profile);

				// Resend all the existing alarms
				for (int nIndex = 0; nIndex < m_pAtsListCtrl->GetItemCount(); nIndex++)
				{
					I_ATS_ISCS_ALARM::AlarmDetail* pAlarmDetail = NULL;
					pAlarmDetail = (I_ATS_ISCS_ALARM::AlarmDetail*)m_pAtsListCtrl->GetItemData(nIndex);

					CCommandHandler::getInstance().submitAlarm(pAlarmDetail, false);
				}
			}
		}
		Sleep(1000);
	}
}

void CRequestHandler::terminate()
{
	m_bTerminate = true;
}
