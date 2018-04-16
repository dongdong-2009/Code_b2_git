#if !defined (APP_BAS_BASManager_BASManagerGUI)
#define APP_BAS_BASManager_BASManagerGUI
#if _MSC_VER > 1000
#pragma once
#endif

#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/AbstractDialogGUI.h"

namespace TA_IRS_App
{
	class CBASManagerDlg;
	class BASManagerGUI : public TA_Base_Bus::AbstractDialogGUI
	{
	public:
		BASManagerGUI(void);
		virtual ~BASManagerGUI(void);
	public:
	protected:
		// The implement of AbstractGUIApplication
		virtual void createApplicationObject();
		virtual void prepareForClose();

		//
		// The implement of IGUIApplication
		//
		void onInitGenericGUICompleted();
		unsigned long getApplicationType();
		void checkEntity(TA_Base_Core::IEntityData* guiEntity) throw(TA_Base_Core::TransactiveException);
		void checkCommandLine() throw(TA_Base_Core::TransactiveException);
		void entityChanged(const std::vector<std::string>& changes);
		void serverIsDown();
		void serverIsUp();
	
		//
		// The implement of IGUIAccess
		// 
		void loadAllUserSettings();

		void saveAllUserSettings();
	private:
		BASManagerGUI(const BASManagerGUI&);
		BASManagerGUI& operator=(const BASManagerGUI&);
	private:
		CBASManagerDlg* m_basManager;
		bool m_commandLineInvalid;
	};

}
#endif //APP_BAS_BASManager_BASManagerGUI

