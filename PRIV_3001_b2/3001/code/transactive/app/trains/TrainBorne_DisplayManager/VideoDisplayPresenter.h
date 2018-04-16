#pragma once

#include <string>
#include <list>
#include <map>
#include "afxwin.h"
#include "MonitorScreen.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

namespace TA_IRS_Bus
{
	class TrainSelectorGUI;
	class TrainSelector;
}

namespace TA_IRS_App
{
class TrainControllerDlg;
class VideoDisplayPresenter
{
public:
	VideoDisplayPresenter(CWnd* wnd,TA_IRS_Bus::TrainSelectorGUI* parentGUI,TA_IRS_Bus::TrainSelector* parentSelector,UINT ctrlID);
	~VideoDisplayPresenter();
	void SetupMonitorScreen();
	void ClearMonitor();
	
	MonitorScreen* getMonitorScreen();
	void OnTVSSCommand(WPARAM wParam);
	void OnMouseMove(UINT nFlags, CPoint point,bool bMsgFromParent=false);
	void OnLButtonDown(UINT nFlags, CPoint point,bool bMsgFromParent=false);
	void OnLButtonUp(UINT nFlags, CPoint point,bool bMsgFromParent=false);
	void OnRButtonDown(UINT nFlags, CPoint point,bool bMsgFromParent=false);
	void OnRButtonUp(UINT nFlags, CPoint point,bool bMsgFromParent=false);

	//TVSS iterface
	bool AssignSingleMonitor(unsigned char cameraID);
	//bool AssignQuad(unsigned char cameraW,unsigned char cameraX,unsigned char cameraY,unsigned char cameraZ);
	bool AssignQuad(unsigned char quadId);
	bool AssignSequence(unsigned sequnceID,unsigned char trainID=0);
	bool TrainBorneAlarmEvent(const char* alarmStr);
	bool TrainBorneAcknowleageAlarm(unsigned long entityKey,const char* sessionID);
	void ClearMonitorScreen();
private:
	
	void ShowTrainDetails();
	bool AssignTrainToAllMonitors(TA_IRS_Bus::TrainInformationTypes::TrainDetails train);
	void ClearMessageList(stuMonitorScreen* screen);
private:
	MonitorScreen m_screen;
	TA_IRS_Bus::TrainSelectorGUI* m_parentGUI;
	TA_IRS_Bus::TrainSelector* m_parentSelector;
	CWnd* m_wnd;
	UINT m_msgCtrlID;
	std::map<std::string ,MonitorScreen*> m_mapScreenDetails;
	std::map<std::string,TA_IRS_Bus::TrainInformationTypes::TrainDetails> m_mapTrainsInScreen;
	TrainControllerDlg* m_trainControllerDlg;
};
}