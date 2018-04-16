#include "SingletonDlgLoader.h"

#include "app\radio\radio_manager_gui\src\RadioInitMsgDlgHelper.h"

namespace TA_IRS_App
{
	
	SingletonDlgLoader * SingletonDlgLoader::m_SingletonDlgLoader = NULL;
	TA_Base_Core::ReEntrantThreadLockable SingletonDlgLoader::m_ThreadLock;
	
	SingletonDlgLoader::SingletonDlgLoader(void)
	{
		FUNCTION_ENTRY ("SingletonDlgLoader");
		FUNCTION_EXIT;
	}

	SingletonDlgLoader::~SingletonDlgLoader(void)
	{
		FUNCTION_ENTRY ("~SingletonDlgLoader");
		FUNCTION_EXIT;
	}

	SingletonDlgLoader * SingletonDlgLoader ::getInstance ()
	{
		FUNCTION_ENTRY ("getInstance");
		TA_THREADGUARD (m_ThreadLock);
		
		if (!m_SingletonDlgLoader)
		{
			m_SingletonDlgLoader = new SingletonDlgLoader();
		}
		
		FUNCTION_EXIT;
		return m_SingletonDlgLoader;
	}

	void SingletonDlgLoader::removeInstance ()
	{
		FUNCTION_ENTRY ("removeInstance");

		TA_THREADGUARD (m_ThreadLock);
		if (m_SingletonDlgLoader)
		{
			delete m_SingletonDlgLoader;
			m_SingletonDlgLoader = NULL;
		}
		FUNCTION_EXIT;
	}
	
	void SingletonDlgLoader::addDlgToLoad (EDlgType dlgType, CWnd * parentWnd)
	{
		
		FUNCTION_ENTRY ("addDlgToLoad");
		
		TA_THREADGUARD (m_ThreadLock);

		std::map <EDlgType,TA_IRS_App::IDlgLoader * >::iterator dlgIter = m_mapDialogCollection.begin();
		
		dlgIter = m_mapDialogCollection.find(dlgType);

		if (dlgIter != m_mapDialogCollection.end())
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Cannot Add Dialog, already on the record map");
			FUNCTION_EXIT;
			return;
		}
		
		IDlgLoader * dlgLoader;
		switch (dlgType)
		{
			case EDlgType::ERadioMsgDlgInit:
				dlgLoader = new RadioInitMsgDlgHelper (parentWnd);
				break;
			default:
				break;
		}		
		
		m_mapDialogCollection.insert(std::pair <EDlgType,TA_IRS_App::IDlgLoader *>(dlgType,dlgLoader));

		FUNCTION_EXIT;
	}
	
	void SingletonDlgLoader::removeDlg (EDlgType dlgType)
	{
		FUNCTION_ENTRY ("removeDlg");

		TA_THREADGUARD (m_ThreadLock);

		std::map <EDlgType,TA_IRS_App::IDlgLoader *>::iterator dlgIter = m_mapDialogCollection.begin();

		dlgIter = m_mapDialogCollection.find(dlgType);

		if (dlgIter == m_mapDialogCollection.end())
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Cannot remove Dialog, dialog not found");
			FUNCTION_EXIT;
			return;
		}

		delete dlgIter->second;

		m_mapDialogCollection.erase(dlgIter);
		
		FUNCTION_EXIT;
	}
	
	void SingletonDlgLoader::setSpecAssignableForDlg (SDialogSpecParam & dlgParam)
	{
		
		FUNCTION_ENTRY ("setSpecAssignableForDlg");
		
		TA_THREADGUARD (m_ThreadLock);
		
		std::map <EDlgType,TA_IRS_App::IDlgLoader * >::iterator dlgIter = m_mapDialogCollection.begin();

		dlgIter = m_mapDialogCollection.find(dlgParam.dlgType);
		
		if (dlgIter == m_mapDialogCollection.end())
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Cannot set a data on this dialog, not yet existing");
			FUNCTION_EXIT;
			return;
		}

		switch (dlgParam.dlgType)
		{
			case EDlgType::ERadioMsgDlgInit:
				{
					TA_IRS_App::IRadioInitDlgMsg * radioInitDlg = (TA_IRS_App::IRadioInitDlgMsg *)(dlgIter->second);
					
					void * vData = dlgParam.vData;

					radioInitDlg->setState((TA_IRS_App::IRadioInitDlgMsg::ERadioInitMsgState *)(vData));

				}
				break;
			default:
				break;
		}		

		FUNCTION_EXIT;
	}


	bool SingletonDlgLoader::openDlg (EDlgType dlgType)
	{
		
		FUNCTION_ENTRY ("openDlg");
		
		TA_THREADGUARD (m_ThreadLock);
		bool bReturn = false;
		std::map <EDlgType,TA_IRS_App::IDlgLoader *>::iterator dlgIter = m_mapDialogCollection.begin();
		
		dlgIter = m_mapDialogCollection.find(dlgType);

		if (dlgIter == m_mapDialogCollection.end())
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Cannot Open Dialog, no record");
			FUNCTION_EXIT;
			return bReturn;;
		}

		switch (dlgType)
		{
			case EDlgType::ERadioMsgDlgInit:
				{
					TA_IRS_App::IRadioInitDlgMsg * radioInitDlg = (TA_IRS_App::IRadioInitDlgMsg *)(dlgIter->second);
					bReturn = radioInitDlg->showDlg();
				}
				break;
			default:
				break;
		}	
		FUNCTION_EXIT;

		return bReturn;
	}
	
	bool SingletonDlgLoader::closeDlg(EDlgType dlgType)
	{
		
		FUNCTION_ENTRY ("closeDlg");

		TA_THREADGUARD (m_ThreadLock);
		
		bool bReturn = false;
		std::map <EDlgType,TA_IRS_App::IDlgLoader *>::iterator dlgIter = m_mapDialogCollection.begin();

		dlgIter = m_mapDialogCollection.find(dlgType);

		if (dlgIter == m_mapDialogCollection.end())
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Cannot Open Dialog, no record");
			FUNCTION_EXIT;
			return bReturn;
		}

		switch (dlgType)
		{
			case EDlgType::ERadioMsgDlgInit:
				{
					TA_IRS_App::IRadioInitDlgMsg * radioInitDlg = (TA_IRS_App::IRadioInitDlgMsg *)(dlgIter->second);
					bReturn = radioInitDlg->closeDlg();
				}
				break;
			default:
				break;
		}
		
		FUNCTION_EXIT;
		return bReturn;
	}


	bool SingletonDlgLoader::isDlgExist (EDlgType dlgType)
	{
		FUNCTION_ENTRY ("isDlgExist");

		TA_THREADGUARD (m_ThreadLock);

		bool bReturn = false;
 		std::map <EDlgType,TA_IRS_App::IDlgLoader *>::iterator dlgIter = m_mapDialogCollection.begin();

		dlgIter = m_mapDialogCollection.find(dlgType);

		if (dlgIter == m_mapDialogCollection.end())
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Cannot Open Dialog, no record");
			FUNCTION_EXIT;
			return bReturn;
		}
		
		TA_IRS_App::IRadioInitDlgMsg * radioInitDlg = (TA_IRS_App::IRadioInitDlgMsg *)(dlgIter->second);
		bReturn = radioInitDlg->isDialogActive();

		FUNCTION_EXIT;
		return bReturn;
	}
	
	void * SingletonDlgLoader::getState (EDlgType dlgType)
	{
		FUNCTION_ENTRY ("isDlgExist");
		
		void * vDataRet = NULL;

		bool bReturn = false;
		std::map <EDlgType,TA_IRS_App::IDlgLoader *>::iterator dlgIter = m_mapDialogCollection.begin();

		dlgIter = m_mapDialogCollection.find(dlgType);


		if (dlgIter == m_mapDialogCollection.end())
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Cannot Open Dialog, no record");
			FUNCTION_EXIT;
			return vDataRet;
		}
		
		TA_IRS_App::IRadioInitDlgMsg * radioInitDlg = (TA_IRS_App::IRadioInitDlgMsg *)(dlgIter->second);
		
		vDataRet = (void *)radioInitDlg->getState();

		FUNCTION_EXIT;

		return vDataRet;
	}

}
