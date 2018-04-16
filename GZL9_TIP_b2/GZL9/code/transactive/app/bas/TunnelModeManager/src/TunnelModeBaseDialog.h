#if !defined (Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelModeBaseDialog)
#define Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelModeBaseDialog
#if _MSC_VER > 1000
#pragma once
#endif
#include "core/threads/src/Thread.h"
#include "bus/generic_gui/src/ResizingProperties.h"
#include "TunnelModeListCtrl.h"
#include "app/bas/TunnelModeManager/src/CommonDefine.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerDataHelper.h"
namespace TA_IRS_App
{
	// CTunnelModeBaseDialog dialog
	class CTunnelModeBaseDialog : public CDialog
	{
	public:
		CTunnelModeBaseDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
		virtual ~CTunnelModeBaseDialog();
	public:
		virtual bool canCloseWindow() = 0;
		virtual TA_Base_Bus::ResizingProperties getResizingProperties() = 0;
		virtual DlgInitializeSize getDlgInitSize() = 0;
        virtual std::string getApplicationCaption() = 0;
	protected:
		virtual BOOL OnInitDialog();
		virtual void OnCancel();
        virtual int ConfirmModeExe(CTunnelModeListCtrl& modeList, int item, std::string titel, std::string modeTypeName);

	protected:
		bool isThreadRunning(const TA_Base_Core::Thread* thread);
		void setIsExecuteModeFlag(bool bExecuteNow);
		bool getIsExecuteModeFlag();
		std::string executeStationMode(TA_IRS_Core::Mode_SmartPtr stationModePtr, bool canOverrideStationMode, const std::string& sessionId);
		std::string getTrackNumberFromSchematic();
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	private:
		bool m_isExecuteMode;
	};
}

#endif //Transactive_3002_GF_App_Bas_TunnelModeManager_CTunnelModeBaseDialog
