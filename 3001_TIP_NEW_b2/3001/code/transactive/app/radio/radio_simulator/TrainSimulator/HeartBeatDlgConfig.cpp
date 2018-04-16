// HeartBeatDlgConfig.cpp : implementation file
//

#include "stdafx.h"
#include "TrainSimulatorDlg.h"
#include "HeartBeatDlgConfig.h"
#include "AddTrainHeartBeatDlg.h"
#include "TrainCmdResponseDlg.h"

#include <vector>
#include <algorithm>
#include <string>


// CHeartBeatDlgConfig dialog

IMPLEMENT_DYNAMIC(CHeartBeatDlgConfig, CDialog)

CHeartBeatDlgConfig::CHeartBeatDlgConfig(CWnd* pParent /*=NULL*/)
: CDialog(CHeartBeatDlgConfig::IDD, pParent)
, m_addTrainHeartBeatDlg(NULL)
, m_commsObserverStat(0)
,m_isUiUpdated(false)
{

}

CHeartBeatDlgConfig::~CHeartBeatDlgConfig()
{
}

void CHeartBeatDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_HEARTBEAT_INFO, m_lstHeartBeatInfo);
	DDX_Control(pDX, IDC_BUTTON_TRAIN_STATUS, m_trainStatus);
	DDX_Control(pDX, IDC_LIST_RESPONSE_STRATEGY, m_lstResponseStrategy);
}


BEGIN_MESSAGE_MAP(CHeartBeatDlgConfig, CDialog)
	
	ON_BN_CLICKED(IDC_BUTTON_ADD_TRAIN, &CHeartBeatDlgConfig::OnBnClickedButtonAddTrain)
	ON_NOTIFY(NM_CLICK, IDC_LIST_HEARTBEAT_INFO, &CHeartBeatDlgConfig::OnNMClickListHeartbeatInfo)
	ON_MESSAGE(WM_HEARTBEAT,onUIUPdate)
	ON_BN_CLICKED(IDC_BUTTON_TRAIN_STATUS, &CHeartBeatDlgConfig::OnBnClickedButtonTrainStatus)
	ON_BN_CLICKED(IDC_BUTTON_KILL_TRAIN, &CHeartBeatDlgConfig::OnBnClickedButtonKillTrain)
	ON_BN_CLICKED(IDC_BUTTON_ADD_STRATEGY, &CHeartBeatDlgConfig::OnBnClickedButtonAddStrategy)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_TRAIN_STRATEGY, &CHeartBeatDlgConfig::OnBnClickedButtonRemoveTrainStrategy)
END_MESSAGE_MAP()

BOOL CHeartBeatDlgConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Initialization

	m_lstHeartBeatInfo.ModifyStyle(0, LVS_REPORT, NULL);
	m_lstHeartBeatInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);



	m_lstHeartBeatInfo.InsertColumn(0,L"Train ID",LVCFMT_LEFT,150);
	m_lstHeartBeatInfo.InsertColumn(1,L"Delay In Seconds",LVCFMT_LEFT,130);
	m_lstHeartBeatInfo.InsertColumn(2,L"Status",LVCFMT_LEFT,120);


	m_lstResponseStrategy.ModifyStyle(0, LVS_REPORT, NULL);
	m_lstResponseStrategy.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);


	m_lstResponseStrategy.InsertColumn(0,L"Train ID",LVCFMT_LEFT,90);
	m_lstResponseStrategy.InsertColumn(1,L"Timer Response Strategy",LVCFMT_LEFT,145);
	m_lstResponseStrategy.InsertColumn(2,L"Response Time in MilliSeconds",LVCFMT_LEFT,145);

	upDateUIHeartBeatInfo();

	updateTrainStrategyUI();

	//Check if the current state is running
	if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING == m_state && ( m_commsObserverStat == 1) )
	{
		m_trainStatus.EnableWindow(TRUE);
	}
	else
		m_trainStatus.EnableWindow(FALSE);



	return TRUE;
}


void CHeartBeatDlgConfig::upDateUIHeartBeatInfo()
{
	//make sure that no item in the list before adding 
	m_lstHeartBeatInfo.DeleteAllItems();

	for( int count = 0; count < m_HeartBeatInfo.TrainID.size(); count++ )
	{
		char cTrainID[5];
		char cTimeDelay[10];
		
		sprintf(cTrainID,"%d",m_HeartBeatInfo.TrainID.at(count));
		sprintf(cTimeDelay,"%d",m_HeartBeatInfo.TimeDelay.at(count));
	
		CString _trainID(cTrainID);
		CString _trainDelay(cTimeDelay);
		CString _status(m_HeartBeatInfo.TrainStatus.at(count).c_str());

		m_lstHeartBeatInfo.InsertItem(count,L"");
		m_lstHeartBeatInfo.SetItemText(count,0,_trainID);
		m_lstHeartBeatInfo.SetItemText(count,1,_trainDelay);
		m_lstHeartBeatInfo.SetItemText(count,2,_status);
	}

}

void CHeartBeatDlgConfig::addTrainList(int trainID, int delay, std::string status)
{	
	char cTrainID[5];
	char cTimeDelay[10];

	sprintf(cTrainID,"%d",trainID);
	sprintf(cTimeDelay,"%d",delay);

	CString _trainID(cTrainID);
	CString _trainDelay(cTimeDelay);
	CString _trainStatus(status.c_str());

	LVFINDINFO _info;

	_info.flags = LVFI_PARTIAL | LVFI_STRING;
	_info.psz = _trainID;
	int _item = m_lstHeartBeatInfo.FindItem(&_info);

	int nRow = m_lstHeartBeatInfo.GetItemCount();


	//check if the train id is not exist
	if ( _item < 0 )
	{
	
		m_lstHeartBeatInfo.InsertItem(nRow,L"");
		m_lstHeartBeatInfo.SetItemText(nRow,0,_trainID);
		m_lstHeartBeatInfo.SetItemText(nRow,1,_trainDelay);
		m_lstHeartBeatInfo.SetItemText(nRow,2,_trainStatus);
	}
}

void CHeartBeatDlgConfig::setHeartBeatInfo(const HeartBeatInfo& info)
{
	m_HeartBeatInfo = info;
}


// CHeartBeatDlgConfig message handlers


void CHeartBeatDlgConfig::OnBnClickedButtonAddTrain()
{
	// TODO: Add your control notification handler code here
	CAddTrainHeartBeatDlg dlg;
	dlg.addObserver(this);
	dlg.DoModal();
}

void CHeartBeatDlgConfig::addTrainHeartbeat(int trainID, int delay)
{
	std::vector<int>::iterator iter = std::find(m_HeartBeatInfo.TrainID.begin(),
											    m_HeartBeatInfo.TrainID.end(),
												trainID);
	if ( iter == m_HeartBeatInfo.TrainID.end())
	{
		//Add train here
		m_HeartBeatInfo.TrainID.push_back(trainID);
		m_HeartBeatInfo.TimeDelay.push_back(delay);
		m_HeartBeatInfo.TrainStatus.push_back("Stopped");

		//Update UI
		//upDateUIHeartBeatInfo();
		addTrainList(trainID,delay,"Stopped");

		//Call TrainSimulatorDLG
		m_addTrainHeartBeatDlg->addTrainHeartbeat(trainID,delay);

	}
	else
		::MessageBox(this->m_hWnd,L"Cannot add new train because its already exist",L"Message",MB_ICONINFORMATION | MB_OK);

}

void CHeartBeatDlgConfig::addObserver(TA_IRS_App::ITrainHeartBeat* trainHeartBeat)
{
	m_addTrainHeartBeatDlg = trainHeartBeat;
}

void CHeartBeatDlgConfig::setCurrentState(TA_Base_Core::Thread::ThreadState_t state)
{
	m_state = state;
}

void CHeartBeatDlgConfig::OnNMClickListHeartbeatInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	int currentSelected = m_lstHeartBeatInfo.GetSelectionMark();
	CString strStatus = m_lstHeartBeatInfo.GetItemText(currentSelected,2);

 	CT2A pszString(strStatus);
 	std::string _status(pszString);
 
 	if ( 0 == _status.compare("Running"))
 	{
 		m_trainStatus.SetWindowText(L"Stopped Train");
 	}
 	else
 		m_trainStatus.SetWindowText(L"Run Train");
	 
}

void CHeartBeatDlgConfig::setCommsObserverStatus(int stat)
{
	m_commsObserverStat = stat;
}

LRESULT CHeartBeatDlgConfig::onUIUPdate(WPARAM wParm, LPARAM lParam)
{

	m_isUiUpdated = true;
	HeartBeatInfo* _info = reinterpret_cast<HeartBeatInfo*>(wParm);
	//copy to local
	HeartBeatInfo _cpyInfo;
	_cpyInfo.TrainID = _info->TrainID;
	_cpyInfo.TimeDelay = _info->TimeDelay;
	_cpyInfo.TrainStatus = _info->TrainStatus;
	_cpyInfo.CommsStatus = _info->CommsStatus;

	delete _info;
	_info = NULL;

	//Update the TrainStatus Map
	updateTrainsListInfo();

	//Compare the Train Status updates
	compareTrainStatusUpdate(_cpyInfo);

	if ( 0 != _cpyInfo.CommsStatus )
	{
		//enable button
		m_trainStatus.EnableWindow(TRUE);
	}
	

	return 0l;
}

void CHeartBeatDlgConfig::compareTrainStatusUpdate(const HeartBeatInfo& info)
{
	size_t _size = info.TrainID.size();
	for (int count = 0; count < _size; count++)
	{
		int _train = info.TrainID.at(count);
		CString sTrainID;
		sTrainID.Format(L"%d",_train);
		std::string _status = info.TrainStatus.at(count);
		CString strStatus(_status.c_str());

		std::map<CString,CString>::iterator iter = TrainStatusMap.find(sTrainID); 

		if ( (*iter).second.Compare(strStatus) != 0 )
		{
			//Update train status
			updateTrainStatus(sTrainID,strStatus);
		}
	}
}

void CHeartBeatDlgConfig::updateTrainStatus(CString _trainID, CString _status)
{
	LVFINDINFO _info;

	_info.flags = LVFI_PARTIAL | LVFI_STRING;
	_info.psz = _trainID;
	int _item = m_lstHeartBeatInfo.FindItem(&_info);

	if ( _item > -1 )
	{
		m_lstHeartBeatInfo.SetItemText(_item,2,_status);
	}
}

void CHeartBeatDlgConfig::OnBnClickedButtonTrainStatus()
{
	// TODO: Add your control notification handler code here
	//Get the selected item
	int currentSelection = m_lstHeartBeatInfo.GetSelectionMark();
	//Get the Train ID
	CString _traiID = m_lstHeartBeatInfo.GetItemText(currentSelection,0);
	//Get Status of the Train
	CString _status = m_lstHeartBeatInfo.GetItemText(currentSelection,2);

	CString _btnCaption;
	m_trainStatus.GetWindowText(_btnCaption);
	if ( _btnCaption.Compare(L"Stopped Train") == 0)
	{
		if ( IDYES == ::MessageBox(this->m_hWnd,L"Stop the Train ?",L"Train Message", MB_ICONQUESTION | MB_YESNO ) )
		{
			m_lstHeartBeatInfo.SetItemText(currentSelection,2,L"Stopped");
			m_addTrainHeartBeatDlg->sendStatusTrainHeartBeat(TrainStatus::STOP,_ttoi(_traiID));
		}
	}
	else
	{
		if ( IDYES == ::MessageBox(this->m_hWnd,L"Run the Train ?",L"Train Message", MB_ICONQUESTION | MB_YESNO ) )
		{
			m_lstHeartBeatInfo.SetItemText(currentSelection,2,L"Running");
			m_addTrainHeartBeatDlg->sendStatusTrainHeartBeat(TrainStatus::START,_ttoi(_traiID));
		}
		
	}

}

void CHeartBeatDlgConfig::updateTrainsListInfo()
{
	//Update TrainsList info
	int _items = m_lstHeartBeatInfo.GetItemCount();
	for ( int count = 0; count < _items; count ++)
	{
		CString _trainID = m_lstHeartBeatInfo.GetItemText(count,0);
		CString _status = m_lstHeartBeatInfo.GetItemText(count,2);

		TrainStatusMap.insert(std::pair<CString,CString>(_trainID,_status));
	}
}

void CHeartBeatDlgConfig::OnBnClickedButtonKillTrain()
{
	// TODO: Add your control notification handler code here
	//Get the selected item
	int currentSelection = m_lstHeartBeatInfo.GetSelectionMark();
	//Get the Train ID
	CString _traiID = m_lstHeartBeatInfo.GetItemText(currentSelection,0);

	//update the m_HeartBeatInfo, remove the train id in the list
	removeTrainIDList(_ttoi(_traiID));

	
	//remove the train from list
	m_lstHeartBeatInfo.DeleteItem(currentSelection);

	m_addTrainHeartBeatDlg->sendStatusTrainHeartBeat(TrainStatus::REMOVE,_ttoi(_traiID));

}

void CHeartBeatDlgConfig::setHeatBeatProcessor(const std::vector<TA_IRS_App::HeartBeatCommandProcessor*> heartBeatProcessor)
{
	m_heartBeatProcessor = heartBeatProcessor;
}

void CHeartBeatDlgConfig::removeTrainIDList(int trainID)
{
	std::vector<int>::iterator iter = std::find(m_HeartBeatInfo.TrainID.begin(),
												m_HeartBeatInfo.TrainID.end(),
												trainID);
	if ( iter != m_HeartBeatInfo.TrainID.end() )
	{
		size_t item = std::distance(m_HeartBeatInfo.TrainID.begin(),iter);
		//remove the data from the list
		m_HeartBeatInfo.TrainID.erase(m_HeartBeatInfo.TrainID.begin() + item);
		m_HeartBeatInfo.TimeDelay.erase(m_HeartBeatInfo.TimeDelay.begin() + item );
		m_HeartBeatInfo.TrainStatus.erase(m_HeartBeatInfo.TrainStatus.begin() + item );

	}
}

void CHeartBeatDlgConfig::OnBnClickedButtonAddStrategy()
{
	// TODO: Add your control notification handler code here
	//Get the selected item
	int currentSelection = m_lstHeartBeatInfo.GetSelectionMark();
	//Get the Train ID
	CString _traiID = m_lstHeartBeatInfo.GetItemText(currentSelection,0);
	CString _display;
	_display.Format(L"Selected Train ID : %s",_traiID);

	//Check if the train exist
	std::vector<int>::iterator iter = std::find(m_HeartBeatInfo.TrainID.begin(),
												m_HeartBeatInfo.TrainID.end(),
												_ttoi(_traiID) );
	if ( iter == m_HeartBeatInfo.TrainID.end())
	{
		::MessageBox(this->m_hWnd,L"No Train ID Selected. \nSelect a Train ID in the list \nof Train HeartBeat Information",L"Message",MB_ICONERROR | MB_OK);
		return;
	}

	CTrainCmdResponseDlg dlg;
	dlg.setTrainID(_ttoi(_traiID));
	dlg.addObserver(this);
	dlg.DoModal();
}

void CHeartBeatDlgConfig::addTrainResponseStrategy(const TrainStrategyInfo &info)
{
	//Add the train strategy list
	addTrainStrategeyList(info);
	m_addTrainHeartBeatDlg->addTrainResponseStrategy(info);
}

void CHeartBeatDlgConfig::addTrainStrategeyList(const TrainStrategyInfo &info)
{
	CString _trainID;
	CString _timerSet;
	CString _strategyType;

	_trainID.Format(L"%d",info.TrainID);
	_timerSet.Format(L"%d",info.TimerSet);
	_strategyType = getTrainStrategyType(info.StrategyType);

	if ( TRUE == canAddTrainStrategyToMap(info) )
	{
		int nRow = m_lstResponseStrategy.GetItemCount();
		m_lstResponseStrategy.InsertItem(nRow,L"");
		m_lstResponseStrategy.SetItemText(nRow,0,_trainID);
		m_lstResponseStrategy.SetItemText(nRow,1,_strategyType);
		m_lstResponseStrategy.SetItemText(nRow,2,_timerSet);
	}
	else
		::MessageBox(this->m_hWnd,L"Train ID is already exist in the list",L"Message",MB_ICONINFORMATION | MB_OK );
	
}

CString CHeartBeatDlgConfig::getTrainStrategyType(const TrainStrategyType& type)
{
	CString _ret = L"";
	if ( TIMER_SEND == type )
	{
		_ret = L"Timer Send";
	}
	else
		_ret = L"Train Timeout";

	return _ret;
}

BOOL CHeartBeatDlgConfig::canAddTrainStrategyToMap(const TrainStrategyInfo& info)
{
	std::map<int,TrainStrategyInfo>::iterator iter = TrainStrategyMapInfo.find(info.TrainID);
	BOOL _ret = FALSE;

 	if ( iter == TrainStrategyMapInfo.end())
 	{
 		//insert the data into map
 		TrainStrategyMapInfo.insert(std::pair<int,TrainStrategyInfo>(info.TrainID,info));
 		_ret = TRUE;
 	}
	
	return _ret;
}

void CHeartBeatDlgConfig::setTrainStrategyMap(const std::map<int,TrainStrategyInfo>& trainStrategy)
{
	//refresh the data
	TrainStrategyMapInfo.clear();
	TrainStrategyMapInfo.insert(trainStrategy.begin(),trainStrategy.end());

}

void CHeartBeatDlgConfig::updateTrainStrategyUI()
{
	std::map<int, TrainStrategyInfo>::iterator iter = TrainStrategyMapInfo.begin();
	for ( iter; iter != TrainStrategyMapInfo.end(); iter ++)
	{
		CString _trainID;
		CString _timerSet;
		CString _strategyType;

		_trainID.Format(L"%d",(*iter).first);
		_timerSet.Format(L"%d",(*iter).second.TimerSet);
		_strategyType = getTrainStrategyType((*iter).second.StrategyType);

		int nRow = m_lstResponseStrategy.GetItemCount();
		m_lstResponseStrategy.InsertItem(nRow,L"");
		m_lstResponseStrategy.SetItemText(nRow,0,_trainID);
		m_lstResponseStrategy.SetItemText(nRow,1,_strategyType);
		m_lstResponseStrategy.SetItemText(nRow,2,_timerSet);
	}
}
void CHeartBeatDlgConfig::OnBnClickedButtonRemoveTrainStrategy()
{
	// TODO: Add your control notification handler code here
	//Get the selected item
	int currentSelection = m_lstResponseStrategy.GetSelectionMark();

	if ( currentSelection == -1 )
	{
		::MessageBox(this->m_hWnd,L"No Train ID Selected. \nSelect a Train ID in the list \nof Command Response Strategy",L"Message",MB_ICONERROR | MB_OK);
		return;
	}
	//Get the Train ID
	CString _traiID = m_lstResponseStrategy.GetItemText(currentSelection,0);

	//remove the trainID in the list
	m_lstResponseStrategy.DeleteItem(currentSelection);

	//remove the train id from the map
	removeTrainIDStrategyMap(_ttoi(_traiID));
	
	m_addTrainHeartBeatDlg->removeTrainResponseStrategy(_ttoi(_traiID));
}

void CHeartBeatDlgConfig::removeTrainIDStrategyMap(int trainID)
{
	std::map<int,TrainStrategyInfo>::iterator iter = TrainStrategyMapInfo.find(trainID);

	if ( iter != TrainStrategyMapInfo.end())
	{
		TrainStrategyMapInfo.erase(iter);
	}
}