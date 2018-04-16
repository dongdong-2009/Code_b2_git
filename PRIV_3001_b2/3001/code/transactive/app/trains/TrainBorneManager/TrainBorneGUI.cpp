#if defined( WIN32 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif // defined( WIN32 )

#include "stdafx.h"

#include "app/Trains/TrainBorneManager/TrainBorneManager.h"
#include "app/Trains//TrainBorneManager/TrainBorneGUI.h"
#include "app/Trains//TrainBorneManager/TrainBorneCommsDlg.h"
#include "app/Trains//TrainBorneManager/TrainBorneManagerDlg.h"
#include "app/Trains//TrainBorneManager/TrainBorneTabManager.h"

#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/application_types/src/apptypes.h"
#include "bus\generic_gui/src/AbstractGUIApplication.h"
#include "core/data_access_interface/entity_access/src/DisplayManagerEntityData.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
 
namespace TA_IRS_App
{
	TrainBorneGUI::TrainBorneGUI()
		:AbstractDialogGUI( IDD_TRAINBORNEMANAGER_DIALOG )
		,m_pTrainBorneDlg(NULL)
		
	{
	//	m_trainBorneTabManager = new TrainBorneTabManager();
	}

	TrainBorneGUI::~TrainBorneGUI()
	{
		FUNCTION_ENTRY( "~TrainBorneGUI" );

        try
		{
            if (getApplicationWnd() != NULL)
            {
                if ( getApplicationWnd() == m_pTrainBorneDlg )
                {
                    delete m_pTrainBorneDlg;
                    m_pTrainBorneDlg = NULL;
                }
                else
                {
                    delete getApplicationWnd();
                    delete m_pTrainBorneDlg;
                    m_pTrainBorneDlg = NULL;
                }
            }
            else if ( NULL != m_pTrainBorneDlg )
            {
                delete m_pTrainBorneDlg;
                m_pTrainBorneDlg = NULL;
            }
 		//	if(m_trainBorneTabManager != NULL)
		//		delete m_trainBorneTabManager;
        }
        catch( ... )
        {
        }

        FUNCTION_EXIT;
	}

	void TrainBorneGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity) throw(TA_Base_Core::TransactiveException)
	{
		try
		{
		    ASSERT(guiEntity!=NULL);

		    m_pTrainBorneDlg->GetTrainBorneCommsDlg()->GetTrainBorneManager()->checkEntity(guiEntity);
		}
		catch(...)
		{
		}
	}

	void TrainBorneGUI::checkCommandLine() throw(TA_Base_Core::TransactiveException)
	{
		int i=1;
	}

	void TrainBorneGUI::entityChanged(const std::vector<std::string>& changes)
	{
		int i=1;
	}

	void TrainBorneGUI::serverIsDown()
	{
		int i=1;
	}

	void TrainBorneGUI::serverIsUp()
	{
		int i=1;
	}

	void TrainBorneGUI::loadAllUserSettings()
	{
		int i=1;
	}

	void TrainBorneGUI::saveAllUserSettings()
	{
		int i=1;
	}

	void TrainBorneGUI::createApplicationObject() 
	{
		FUNCTION_ENTRY( "createApplicationObject" );
		if (!m_pTrainBorneDlg)
		{
			CWnd *pWndPrev;
			if (pWndPrev = CWnd::FindWindow(NULL,"Train-Borne Manager")) 
			{
				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"FindWindow Train-Borne Manager"); 
				CWnd * pWndChild = pWndPrev->GetLastActivePopup();
				pWndPrev->SendMessage(WM_TB_CHANGEPOS);
				pWndPrev->ShowWindow(SW_RESTORE);
				pWndChild->SetForegroundWindow();
				exit(0);
			}
			m_pTrainBorneDlg = new CTrainBorneManagerDlg(*this);
		}	
		setApplicationWnd( *m_pTrainBorneDlg );
		FUNCTION_EXIT;
	}

	void TrainBorneGUI::prepareForClose() 
	{
        if ( NULL != m_pTrainBorneDlg )
        {
            m_pTrainBorneDlg->OnDestory();
        }
	}

	unsigned long TrainBorneGUI::getApplicationType()
	{
		FUNCTION_ENTRY( "getApplicationType" );
		FUNCTION_EXIT;
		return TRAIN_MANAGER_GUI_APPTYPE;
	}

	void TrainBorneGUI::onInitGenericGUICompleted()
	{	
	//	m_trainBorneTabManager->setWndParent(m_pTrainBorneDlg);
	//	m_trainBorneTabManager->createApplicationObject();

		CWnd *pWnd = getApplicationWnd();
		pWnd->LockWindowUpdate();

		CTrainBorneManagerDlg * pDlg = dynamic_cast<CTrainBorneManagerDlg * >(pWnd);
		TA_ASSERT(pDlg != NULL, "the application window is NULL!");

		pDlg->initializeWindow();
		pDlg->UnlockWindowUpdate();
		pDlg->UpdateWindow();

	}
}

