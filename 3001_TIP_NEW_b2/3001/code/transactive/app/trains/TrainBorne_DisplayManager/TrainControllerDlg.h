#ifndef _TRAIN_CONTROLLER_DLG_H_
#define _TRAIN_CONTROLLER_DLG_H_

#include <map>
#include <string>
#include "TrainBorne_DisplayManager/resource.h"
#include "TrainBorne_DisplayManager/MonitorScreen.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
// TrainControllerDlg dialog

namespace TA_IRS_App
{
class Sequence;
class VideoDisplayPresenter;
class TrainControllerDlg : public CDialog
{
	DECLARE_DYNAMIC(TrainControllerDlg)

	struct stuBackupScreen
	{
		unsigned char monitorElement;
		ElementType monitorElementType;
		bool monitorSelected;
		std::string Operation;
		std::string SelScreen;
	};
public:
	TrainControllerDlg(VideoDisplayPresenter* presenter,CWnd* pParent = NULL);   // standard constructor
	virtual ~TrainControllerDlg();
	virtual BOOL OnInitDialog();
	void SetTrainDetails(TA_IRS_Bus::TrainInformationTypes::TrainDetails* train);
	void SetMonitorScreen(MonitorScreen* screen);
	MonitorScreen* GetMonitorScreen();
	void ClearMonitorScreen();
	
// Dialog Data
	enum { IDD = IDD_VIDEODISPLAYMANAGER_CONTROLLER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnDestory();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLvnBegindragLbCameras(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeDvQuad();
	afx_msg void OnCbnSelchangeDvSequence();
	afx_msg void OnBnClickedRaSingleMonitor();
	afx_msg void OnBnClickedRaQuad();
	afx_msg void OnBnClickedRaSequence();

	afx_msg void OnBnClickedControllerApply();
	afx_msg void OnBnClickedControllerClose();

	bool AssignSingleMonitor();
	bool AssignQuad();
	bool AssignSequence();

	DECLARE_MESSAGE_MAP()
private:
	void InitQuad();
	void InitSequence();
	int GetQuadIndexFromAddress(int iAddr);
	int GetQuadAddressFromIndex(int index);
	void BackupScreen(MonitorScreen* monitorScreen);
	void RestoreScreen(MonitorScreen* monitorScreen);
private:
	int m_Operation;
	CListCtrl m_LtCamera;
	CComboBox m_CbQuad,m_CbSequence;
	std::map<unsigned long, Sequence*> m_mapSequence;
	MonitorScreen* m_monitorScreen;
	bool m_bDragging;
	UINT m_curSelCamera;
	CImageList* m_imgDrag;
	TA_IRS_Bus::TrainInformationTypes::TrainDetails* m_train;
	VideoDisplayPresenter* m_presenter;
	stuBackupScreen m_backupScreen;
};
}
#endif
