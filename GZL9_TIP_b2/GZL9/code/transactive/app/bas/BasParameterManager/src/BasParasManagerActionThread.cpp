#include "StdAfx.h"
#include "app/bas/BasParameterManager/src/BasParasManagerActionThread.h"
#include "app/bas/BasParameterManager/src/BasParameterManagerDlg.h"
#include "core/utilities/src/DebugUtil.h"
namespace TA_IRS_App
{
	BasParasManagerActionThread::BasParasManagerActionThread(CBasParameterManagerDlg* callback)
		:m_callback(callback)
	{
	}

	BasParasManagerActionThread::~BasParasManagerActionThread(void)
	{
	}

	void BasParasManagerActionThread::setAction(BasParasManagerAction action)
	{
		m_action = action;
	}

	void BasParasManagerActionThread::run()
	{
		if (NULL == m_callback)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,  "m_callBack is NULL.");
			return;
		}

		switch (m_action)
		{
		case BasParameterManagerAction_Initialize:
			m_callback->InitializeData();
			break;
		case BasParameterManagerAction_FilterEquipment:
			m_callback->FileterEquipment();
			break;
		default:
			break;
		}
	}

	void BasParasManagerActionThread::terminate()
	{

	}
}
