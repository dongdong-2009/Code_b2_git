
#include "RadioInitMsgDlgHelper.h"

namespace TA_IRS_App
{
	RadioInitMsgDlgHelper::RadioInitMsgDlgHelper(CWnd * parenWnd) : m_parentWnd(parenWnd),
																	m_radioMsgInitDlg(NULL)
	{
		FUNCTION_ENTRY ("RadioInitMsgDlgHelper");
		FUNCTION_EXIT;
	}

	RadioInitMsgDlgHelper::~RadioInitMsgDlgHelper(void)
	{
		FUNCTION_ENTRY ("~RadioInitMsgDlgHelper");
		FUNCTION_EXIT;
	}

	bool RadioInitMsgDlgHelper::showDlg ()
	{

		FUNCTION_ENTRY ("showDlg");

		if (EInit != m_radioMsgState)
		{
			LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "showDlg : Initialization is done no need for this dialog");
			return false;
		}
		
		if (!m_radioMsgInitDlg)
		{
			m_radioMsgInitDlg = new CRadioInitMsgDlg;
			
			bool bCreated = m_radioMsgInitDlg->Create(IDD_RADIO_INITIALISE,m_parentWnd);
				
			if (!bCreated)
			{
				LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Cannot create dialog");
				
				if (m_radioMsgInitDlg)
				{	
					delete m_radioMsgInitDlg;
					m_radioMsgInitDlg = NULL;
				}
				
				FUNCTION_EXIT;
				return false;
			}
			
		}

		LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RadioMsgInitDlg Created");

				
		FUNCTION_EXIT;

		return m_radioMsgInitDlg->ShowWindow(SW_SHOW);

	}

	bool RadioInitMsgDlgHelper::closeDlg ()
	{
		
		FUNCTION_ENTRY("closeDlg");
		if (EOperation == m_radioMsgState)
		{
			LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "closeDlg : Dialog is already close by this time, operation mode already, nothing to close");
			FUNCTION_EXIT;
			return false;
		}
			
		if (!m_radioMsgInitDlg)
		{
			LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No valid pointer yet to close the dialog");
			FUNCTION_EXIT;
			return false;
		}

		LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Deleting RadioMsgInitDlg");

		bool bDestroyed = m_radioMsgInitDlg->DestroyWindow();

		if (m_radioMsgInitDlg)
		{
			delete m_radioMsgInitDlg;
			m_radioMsgInitDlg = NULL;
		}

		FUNCTION_EXIT;
		return true;

	}

	void RadioInitMsgDlgHelper::setState (ERadioInitMsgState * radioInitMsgState)
	{
		FUNCTION_ENTRY("setState");

		LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, " Setting RadioInitMsgDlg State to : %d",*radioInitMsgState);
		m_radioMsgState = *radioInitMsgState;
		
		FUNCTION_EXIT;
	}
	
	bool RadioInitMsgDlgHelper::isDialogActive ()
	{
		bool bIsActive;
		
		if (m_radioMsgInitDlg)
			bIsActive = true;
		else
			bIsActive = false;

		return bIsActive;
	}

	IRadioInitDlgMsg::ERadioInitMsgState * RadioInitMsgDlgHelper::getState ()
	{
		return &m_radioMsgState;
	}

}
