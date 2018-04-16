// TrainBorneManagerDlg.h : header file
//

#ifndef _TRAIN_BORN_MANAGER_DLG_
#define _TRAIN_BORN_MANAGER_DLG_


#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/radio/radiosearchpage/src/isearchpagehelper.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"
#include "TrainBorneTabManager.h"
#include "LibraryDownloadPage.h"
//#include "VideoSourcesListCtrl.h"
#define WM_TB_CHANGEPOS  WM_USER+111
// CTrainBorneTestDlg dialog
namespace TA_IRS_App
{
class TrainDetailTest;
class TBTrainSelectorGUI;
class CTrainBorneCommsDlg;
class TrainDvaVersionsPage;
class CTrainBorneManagerDlg :  public TA_Base_Bus::TransActiveDialog,
							   public TA_Base_Core::RunParamUser
{
// Construction
public:
    CTrainBorneManagerDlg(TA_Base_Bus::IGUIAccess& controlClass);	// standard constructor
    ~CTrainBorneManagerDlg();
	void onRunParamChange( const std::string& name, const std::string& paramvalue );

// Dialog Data
	enum { IDD = IDD_TRAINBORNEMANAGER_DIALOG };
	TrainBorneTabManager*  GetTrainBorneManager() {return &m_TrainBorneTabManager;}
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	//Lucky
	afx_msg void OnAboutbutton();



// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();					//Hide it when launch for the first time
	afx_msg HCURSOR OnQueryDragIcon();
	virtual afx_msg LRESULT OnSetWindowPosition(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTBChangePos(WPARAM wParam, LPARAM lParam);
	CTabCtrl m_TbTrainBorneGui;

public:
	afx_msg void OnClose();
	afx_msg void OnDestory();
	afx_msg void OnTcnSelchangeTbTrainBorneGui(NMHDR *pNMHDR, LRESULT *pResult);
	CTrainBorneCommsDlg* GetTrainBorneCommsDlg(){ return m_trainBorneCommsDlg;};

	void initializeWindow();

private:
	void ModifyWindowPos();

private:
	TrainBorneTabManager m_TrainBorneTabManager; 
	CTrainBorneCommsDlg* m_trainBorneCommsDlg;
    TrainDvaVersionsPage* m_pTrainDvaVersionPage;
	bool m_isLaunched;
	bool m_isAssignedBySchematic;
	CLibraryDownloadPage* m_libraryDownloadPage;
	int m_showCounter;
	DECLARE_MESSAGE_MAP()
};
}
#endif