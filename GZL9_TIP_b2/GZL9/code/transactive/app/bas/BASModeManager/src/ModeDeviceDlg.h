#pragma once
#include "afxcmn.h"
#include "core/data_access_interface/bas/src/ModeAccessFactory.h"
#include "app/bas/BASModeManager/src/ModeStatusListenerManager.h"
#include "app/bas/BASModeManager/src/DeviceList.h"
#include "app/bas/BASModeManager/src/CommonDef.h"
#include "app/bas/BASModeManager/src/BasManagerActionThread.h"
namespace TA_IRS_App
{
	// CModeDeviceDlg dialog
	using namespace TA_IRS_Core;
	class CModeDeviceDlg : public CDialog, 
						   public IModeStatusListenerInterface,
						   public IActionThreadFunctionInterface
	{
		DECLARE_DYNAMIC(CModeDeviceDlg)

	public:
		CModeDeviceDlg(Mode_SmartPtr mode, CWnd* pParent = NULL);   // standard constructor
		virtual ~CModeDeviceDlg();

	// Dialog Data
		enum { IDD = IDD_DIALOG_DEVICE };
	public:
		// IModeStatusListenerInterface implement
		void updateModeStatus(unsigned long modeKey, int status);

		// the IActionThreadFunctionInterface implement
		void  executeThreadFunction(BasManagerAction action);
	protected:
		void InitializeDlgData();
		void StartMonitorDevices();
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnBnClickedCancel();
		afx_msg void OnTimer(UINT_PTR nIDEvent);
		DECLARE_MESSAGE_MAP()
	private:
		CDeviceList m_deviceList;
		Mode_SmartPtr m_mode;

		BasManagerActionThread* m_initDlgDataThread;
		bool m_isStopLoading;
	};
}

