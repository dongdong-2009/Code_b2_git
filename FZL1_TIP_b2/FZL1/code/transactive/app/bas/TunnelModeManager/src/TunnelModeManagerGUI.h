#if !defined (Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerGUI)
#define Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerGUI
#if _MSC_VER > 1000
#pragma once
#endif

#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/AbstractDialogGUI.h"

namespace TA_IRS_App
{
	class TunnelModeManagerGUI : public TA_Base_Bus::AbstractDialogGUI
	{
	public:
		TunnelModeManagerGUI(void);
		~TunnelModeManagerGUI(void);

	protected:
		// The implement of AbstractGUIApplication
		virtual void createApplicationObject();
		virtual void prepareForClose();

		//
		// The implement of IGUIApplication
		//
		virtual void onInitGenericGUICompleted();
		virtual unsigned long getApplicationType();
		virtual void checkEntity(TA_Base_Core::IEntityData* guiEntity);
		virtual void checkCommandLine();
		virtual void entityChanged(const std::vector<std::string>& changes);
		virtual void serverIsDown();
		virtual void serverIsUp();

		//
		// The implement of IGUIAccess
		// 
		virtual void loadAllUserSettings();
		virtual void saveAllUserSettings();

	private:
		TunnelModeManagerGUI(const TunnelModeManagerGUI&);
		TunnelModeManagerGUI& operator=(const TunnelModeManagerGUI&);
	private:
		CDialog* m_tunnelManager;
		bool m_commandLineInvalid;
	};
}

#endif //Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerGUI