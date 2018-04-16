#if !defined(TA_GF_3002_app_bas_BasParameterManager_BasParameterManagerGUI)
#define TA_GF_3002_app_bas_BasParameterManager_BasParameterManagerGUI
#pragma once


#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/AbstractDialogGUI.h"

namespace TA_IRS_App
{
	class CBasParameterManagerDlg;
	class BasParameterManagerGUI : public TA_Base_Bus::AbstractDialogGUI
	{
	public:
		BasParameterManagerGUI(void);
		virtual ~BasParameterManagerGUI(void);
	public:
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
		BasParameterManagerGUI(const BasParameterManagerGUI&);
		BasParameterManagerGUI& operator=(const BasParameterManagerGUI&);
	private:
		CBasParameterManagerDlg* m_basManager;
		bool m_commandLineInvalid;
	};
}
#endif //TA_GF_3002_app_bas_BasParameterManager_BasParameterManagerGUI