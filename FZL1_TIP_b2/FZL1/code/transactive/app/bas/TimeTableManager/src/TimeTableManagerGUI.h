#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerGUI)
#define Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerGUI
#if _MSC_VER > 1000
#pragma once
#endif

#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/AbstractDialogGUI.h"

namespace TA_IRS_App
{
	class TimeTableManagerGUI : public TA_Base_Bus::AbstractDialogGUI
	{
	public:
		TimeTableManagerGUI(void);
		virtual ~TimeTableManagerGUI(void);
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
		void checkEntity(TA_Base_Core::IEntityData* guiEntity);
		void checkCommandLine();
		void entityChanged(const std::vector<std::string>& changes);
		void serverIsDown();
		void serverIsUp();
		
		//
		// The implement of IGUIAccess
		// 
		void loadAllUserSettings();

		void saveAllUserSettings();
	private:
		TimeTableManagerGUI(const TimeTableManagerGUI&);
		TimeTableManagerGUI& operator=(const TimeTableManagerGUI&);
	private:
		CDialog* m_timeTableManager;
		bool m_commandLineInvalid;
	};
}

#endif //Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableManagerGUI