#include "VideoDisplayPresenter.h"
#include "VideoDisplayManagerDlg.h"
#include "resource.h"
#include "DisplayItem/Quad.h"
#include "DisplayItem/DisplayItemManager.h"
#include "TrainControllerDlg.h"
#include "SwitchAgentCommunicator.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h""
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"

namespace TA_IRS_App
{
	using TA_Base_Bus::TA_GenericGui::WM_SET_WINDOW_POSITION;
	using TA_Base_Bus::TA_GenericGui::NO_REPOSITION;

	VideoDisplayPresenter::VideoDisplayPresenter(CWnd* wnd,TA_IRS_Bus::TrainSelectorGUI* parentGUI,TA_IRS_Bus::TrainSelector* parentSelector,UINT ctrlID)
		:m_wnd(wnd),m_parentGUI(parentGUI),m_parentSelector(parentSelector),m_msgCtrlID(ctrlID),m_trainControllerDlg(NULL)
	{
			DisplayItemManager::getInstance().initialiseDisplayItems( wnd );
	}

	VideoDisplayPresenter::~VideoDisplayPresenter()
	{
		ClearMonitorScreen();
		if(!m_trainControllerDlg)
		{
			delete m_trainControllerDlg;
			m_trainControllerDlg = NULL;
		}
	}

	void VideoDisplayPresenter::SetupMonitorScreen()
	{
		m_screen.setWnd(m_wnd);
		std::string strLocationName = DisplayItemManager::getInstance().getLocationNameStr();
		unsigned long consoleKey = DisplayItemManager::getInstance().getConsoleKey();
		SwitchAgentCommunicator::setLocationName(strLocationName);
		m_screen.setMaps(::LoadBitmap( AfxGetInstanceHandle(),MAKEINTRESOURCE( IDB_VD_WALL_SMALL ) ),
				::LoadBitmap( AfxGetInstanceHandle(),MAKEINTRESOURCE( IDB_VD_WALL_SMALL_H ) ),
				::LoadBitmap( AfxGetInstanceHandle(),MAKEINTRESOURCE( IDB_VD_CONSOLE ) ),
				::LoadBitmap( AfxGetInstanceHandle(),MAKEINTRESOURCE( IDB_VD_CONSOLE_H ) ),
				::LoadBitmap( AfxGetInstanceHandle(),MAKEINTRESOURCE( IDB_VD_OVERALL ) ),
				::LoadBitmap( AfxGetInstanceHandle(),MAKEINTRESOURCE( IDB_VD_OVERALL_H ) ));
		std::map<std::string, TrainBorneMonitor*> monitors=DisplayItemManager::getInstance().getAllMonitors();
		std::map<std::string, TrainBorneMonitor*>::iterator it =monitors.begin();
		ScreenType screenType;
		for(int i=0;it!=monitors.end();it++,i++)
		{
			if(it->second->isConsoleMonitor())
			{
				if(strLocationName == LOCATION_NAME_OCC)
				{
					if(it->second->getConsoleKey()!=consoleKey)
						continue;
				}
				screenType = ScreenConsole;
			}
			else if(it->second->isOverAllMonitor())
				screenType = ScreenOverAll;
			else
				screenType = ScreenWall;
			m_screen.setMonitor(it->second->getDisplayOrder(),screenType,it->second);
		}
		m_screen.drawAll();
		m_trainControllerDlg = new TrainControllerDlg(this,m_wnd);
		m_trainControllerDlg->Create(IDD_VIDEODISPLAYMANAGER_CONTROLLER,m_wnd);
		CRect wndRect,dlgRect,newRect;
		m_trainControllerDlg->GetWindowRect(&dlgRect);
		CPoint pt(0,0);
		ClientToScreen(m_wnd->m_hWnd,&pt);
		newRect.left = pt.x;
		newRect.top = pt.y;
		newRect.right = dlgRect.Width()+pt.x;;
		newRect.bottom = dlgRect.Height()+pt.y;
		
		m_trainControllerDlg->MoveWindow(newRect);
		std::list<stuMonitorScreen*> screenList = m_screen.getScreenList();
		
		std::list<stuMonitorScreen*>::iterator screen_it = screenList.begin();
		for(;screen_it!=screenList.end();screen_it++)
		{
			MonitorScreen* details = new MonitorScreen((*screen_it)->name);
			details->insertScreen("SingleMonitor",IDC_VD_MAP_SINGLE_MONITOR,IDC_VD_TEXT_SINGLE_MONITOR,1,ScreenSingleMonitor,ElementSequence,1);
			details->setMaps(ScreenSingleMonitor,::LoadBitmap( AfxGetInstanceHandle(),MAKEINTRESOURCE( IDB_VD_SINGLE_MONITOR ) ),::LoadBitmap( AfxGetInstanceHandle(),MAKEINTRESOURCE( IDB_VD_SINGLE_MONITOR_H ) ));
			details->setWnd(m_trainControllerDlg);
			m_mapScreenDetails[(*screen_it)->name]=details;
		}
		
	}

	bool VideoDisplayPresenter::AssignSingleMonitor(unsigned char cameraID)
	{
		bool success = false;
		stuMonitorScreen* selScreen = m_screen.getSelScreen();
		if(NULL != selScreen)
		{
			unsigned char monitorID=atoi(selScreen->monitor->getAddress().c_str());
			success = (selScreen->elementID == 0
				       || SwitchAgentCommunicator::getInstance().clearMonitor(selScreen->elementID,monitorID))
					   && SwitchAgentCommunicator::getInstance().setSingleScreen(selScreen->elementID,monitorID,cameraID)
					   && SwitchAgentCommunicator::getInstance().vssClearMonitor(selScreen->monitor->getKey())
					   && SwitchAgentCommunicator::getInstance().mappingTrainBorneMonitor(selScreen->monitor->getKey());
		}
		return success;
	}

	bool VideoDisplayPresenter::AssignQuad(unsigned char quadId)
	{
		bool success = false;
		stuMonitorScreen* selScreen = m_screen.getSelScreen();
		if(NULL != selScreen)
		{
			unsigned char monitorID=atoi(selScreen->monitor->getAddress().c_str());
			success = (selScreen->elementID == 0
				       || SwitchAgentCommunicator::getInstance().clearMonitor(selScreen->elementID,monitorID))
					   && SwitchAgentCommunicator::getInstance().setQuad(selScreen->elementID,monitorID,quadId)
					   && SwitchAgentCommunicator::getInstance().vssClearMonitor(selScreen->monitor->getKey())
					   && SwitchAgentCommunicator::getInstance().mappingTrainBorneMonitor(selScreen->monitor->getKey());
		}
		return success;
	}

	bool VideoDisplayPresenter::AssignSequence(unsigned sequnceID,unsigned char trainID)
	{
		bool success = false;
		stuMonitorScreen* selScreen = m_screen.getSelScreen();
		if(NULL != selScreen)
		{
			unsigned char monitorID=atoi(selScreen->monitor->getAddress().c_str());
			success = (selScreen->elementID == 0
				       || SwitchAgentCommunicator::getInstance().clearMonitor(selScreen->elementID,monitorID))
                       && SwitchAgentCommunicator::getInstance().setSequence(trainID>0?trainID:selScreen->elementID,monitorID,sequnceID)
			           && SwitchAgentCommunicator::getInstance().vssClearMonitor(selScreen->monitor->getKey())
					   && SwitchAgentCommunicator::getInstance().mappingTrainBorneMonitor(selScreen->monitor->getKey());
		}
		return success;
	}

	bool VideoDisplayPresenter::TrainBorneAlarmEvent(const char* alarmStr)
	{
		stuMonitorScreen* selScreen = m_screen.getSelScreen();
		if(NULL==selScreen)
		{
			AfxMessageBox("Please select Monitor at first");
			return false;
		}
		if(!SwitchAgentCommunicator::getInstance().mappingTrainBorneMonitor(selScreen->monitor->getKey()))
			return false;
		return SwitchAgentCommunicator::getInstance().trainBorneAlarmEvent(selScreen->elementID,alarmStr);

	}

	bool VideoDisplayPresenter::TrainBorneAcknowleageAlarm(unsigned long entityKey,const char*sessionID)
	{

		return SwitchAgentCommunicator::getInstance().trainBorneAcknowleageAlarm(entityKey,sessionID);;
	}

	void VideoDisplayPresenter::ClearMonitor()
	{
		
		stuMonitorScreen* selScreen = m_screen.getSelScreen();
		if(NULL==selScreen)
		{
			return;
		}
		unsigned char monitorID=atoi(selScreen->monitor->getAddress().c_str());
		if(!SwitchAgentCommunicator::getInstance().clearMonitor(selScreen->elementID,monitorID))
		{
			return;
		}
		if(!SwitchAgentCommunicator::getInstance().vssClearMonitor(selScreen->monitor->getKey()))
		{
			return;
		}
		m_trainControllerDlg->ClearMonitorScreen();
		selScreen->elementID=0;
		m_screen.draw(*selScreen);

		std::map<std::string,TA_IRS_Bus::TrainInformationTypes::TrainDetails>::iterator it = m_mapTrainsInScreen.find(selScreen->name);
		if(it != m_mapTrainsInScreen.end())
		{
			m_mapTrainsInScreen.erase(it);
		}
		if(m_trainControllerDlg->IsWindowVisible())
		{
			m_trainControllerDlg->ShowWindow(SW_HIDE);
		}
		ClearMessageList(selScreen);
	}

	void VideoDisplayPresenter::ClearMessageList(stuMonitorScreen* screen)
	{
		CString strMonitoDisplayName;
		if(screen->type == ScreenOverAll)
			strMonitoDisplayName = "Over All Monitor";
		else if(screen->type == ScreenConsole)
			strMonitoDisplayName.Format("Console Monitor %d", screen->index);
		else
			strMonitoDisplayName.Format("Wall Monitor %d",screen->index);
		CListBox* listBox = (CListBox*)( m_wnd->GetDlgItem(m_msgCtrlID));
		for(int i=0;i<listBox->GetCount();i++)
		{
			CString strLine;
			listBox->GetText(i,strLine);
			if(strLine.Find(strMonitoDisplayName)>=0)
			{	
				listBox->DeleteString(i);
			}
		}
	}

	void VideoDisplayPresenter::ClearMonitorScreen()
	{
		std::list<stuMonitorScreen*> screenList = m_screen.getScreenList();
		std::list<stuMonitorScreen*>::iterator screen_it = screenList.begin();
		for(;screen_it!=screenList.end();screen_it++)
		{
			std::map<std::string,MonitorScreen*>::iterator  details_it = m_mapScreenDetails.find((*screen_it)->name);
			if(m_mapScreenDetails.end()==details_it)
				continue; 
			MonitorScreen* details=m_mapScreenDetails[(*screen_it)->name];
			if(details==NULL)
				continue;
			delete details;
			details=NULL;
			m_mapScreenDetails.erase(details_it);
			std::map<std::string,TA_IRS_Bus::TrainInformationTypes::TrainDetails>::iterator trainit = m_mapTrainsInScreen.find((*screen_it)->name);
			if(trainit==m_mapTrainsInScreen.end())
				continue;

			m_mapTrainsInScreen.erase(trainit);
			
		}
	}


	MonitorScreen* VideoDisplayPresenter::getMonitorScreen()
	{
		return &m_screen;
	}

	bool VideoDisplayPresenter::AssignTrainToAllMonitors(TA_IRS_Bus::TrainInformationTypes::TrainDetails train)
	{
		std::list<stuMonitorScreen*> screenList = m_screen.getScreenList();
		std::list<stuMonitorScreen*>::iterator it = screenList.begin();
		for( ;it!= screenList.end();it++)
		{
			if((*it)->type ==ScreenOverAll)
				continue;
			if(NULL==(*it)->monitor)
				continue;

			unsigned char monitorID = atoi((*it)->monitor->getAddress().c_str());
			if(!SwitchAgentCommunicator::getInstance().setSequence(train.trainId,monitorID, 1))
				return false;
			(*it)->elementID = train.trainId;
			m_mapTrainsInScreen[(*it)->name]=train;
			m_screen.draw(*(*it));
		}
		return true;
	}

	void VideoDisplayPresenter::OnTVSSCommand(WPARAM wParam)
	{
		TA_Base_Bus::TVSSManagerCorbaDef::TVSSCommand* tvssCommand =(TA_Base_Bus::TVSSManagerCorbaDef::TVSSCommand*)(wParam);
		TA_ASSERT(tvssCommand != NULL, "");
		stuMonitorScreen* screen = m_screen.getSelScreenByMonitorAddress(tvssCommand->monitorId);
		if(screen==NULL)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Unknown console monitor.Index:%d",tvssCommand->monitorId );
			return;
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"OnTVSSCommand cmdtype:%d,trainId:%ld,monitorId:%d,input0:%d,input1:%d"
					,tvssCommand->cmdType,tvssCommand->trainId, tvssCommand->monitorId,tvssCommand->videoInput[0],tvssCommand->videoInput[1] );
		CString monitorDisplayName;
		switch(screen->type)
		{
		case ScreenWall:
			monitorDisplayName.Format("Wall Monitor %d",screen->index); 
			break;
		case ScreenConsole:
			monitorDisplayName.Format("Console Monitor %d",screen->index); 
			break;
		case ScreenOverAll:
			monitorDisplayName.Format("OverAll Display System");
			break;
		default:
			break;
		}
		if(tvssCommand->cmdType == TA_Base_Bus::TVSSManagerCorbaDef::SingleScreenAlarm || tvssCommand->cmdType == TA_Base_Bus::TVSSManagerCorbaDef::QuadScreenAlarm|| tvssCommand->cmdType == TA_Base_Bus::TVSSManagerCorbaDef::SequnceScreenAlarm)
		{
			if( !(screen->elementID == 0 || SwitchAgentCommunicator::getInstance().clearMonitor(screen->elementID,tvssCommand->monitorId))
			    || !SwitchAgentCommunicator::getInstance().setAcknowledgeEvent(tvssCommand->trainId,tvssCommand->monitorId,tvssCommand->videoInput[0],tvssCommand->videoInput[1])
				|| !SwitchAgentCommunicator::getInstance().vssClearMonitor(screen->monitor->getKey())
				|| !SwitchAgentCommunicator::getInstance().mappingTrainBorneMonitor(screen->monitor->getKey()))
			{
				return;
			}
		}
    /*
		else if(tvssCommand->cmdType == TA_Base_Bus::TVSSManagerCorbaDef::SingleScreen || tvssCommand->cmdType == TA_Base_Bus::TVSSManagerCorbaDef::QuadScreen|| tvssCommand->cmdType == TA_Base_Bus::TVSSManagerCorbaDef::SequnceScreen)
		{
			if(screen->elementID>0 && !m_switchCommunicator.mappingTrainBorneMonitor(screen->monitor->getKey()))
			{
				return ;
			}
		}
    */
		long trainId = (unsigned long)(screen->elementID);
		char monitorId = tvssCommand->monitorId;
		screen->elementID = tvssCommand->trainId;
		MonitorScreen* screenDetails = m_mapScreenDetails[screen->name];
		if(screenDetails == NULL)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Can not find matched screen details:[%s]",screen->name.c_str() );
			return;
		}
		stuMonitorScreen* subscreen;
		CString strMsg="";
		CWnd* wnd = m_wnd->GetTopLevelParent();
		switch(tvssCommand->cmdType)
		{
		case TA_Base_Bus::TVSSManagerCorbaDef::SingleScreen:
		case TA_Base_Bus::TVSSManagerCorbaDef::SingleScreenAlarm:
			{	
				screenDetails->setScreenInfo(SCREEN_OPERATION,"0");
				char cameraId[16] = {0};
				sprintf(cameraId,"%d",tvssCommand->videoInput[0]);
				screenDetails->setScreenInfo(SCREEN_SELSCREEN,cameraId);
				subscreen = screenDetails->getScreen(1,ScreenSingleMonitor);
				subscreen->elementID = tvssCommand->videoInput[0];
				subscreen->elementType = ElementCamera;
				if(TA_Base_Bus::TVSSManagerCorbaDef::SingleScreen==tvssCommand->cmdType)
				{
					strMsg.Format("Train %d assign camera (%d) on %s",tvssCommand->trainId,tvssCommand->videoInput[0],monitorDisplayName );
				}
				else
				{
					strMsg.Format("Train %d acknowledge camera (%d) alarm on %s",tvssCommand->trainId,tvssCommand->videoInput[0],monitorDisplayName );
				}
				if(wnd != NULL)
				{
					wnd->ShowWindow(SW_RESTORE);
					wnd->SetForegroundWindow();
				}
			}
			break;
		case TA_Base_Bus::TVSSManagerCorbaDef::QuadScreen:
		case TA_Base_Bus::TVSSManagerCorbaDef::QuadScreenAlarm:
			{	
				std::string quadId = "0";
				std::map<std::string,Quad*> quads = DisplayItemManager::getInstance().getAllQuads();
				for(std::map<std::string,Quad*>::iterator quad_it = quads.begin(); quad_it != quads.end(); ++quad_it)
				{
					int find_cameras = 0;
					int i;
					Quad* quad =(*quad_it).second;
					unsigned char* camerasAddress = quad->getCameraAddress();
					for( i= 0; i < 4 && tvssCommand->videoInput[i] > 0; ++i)
					{
						for(int j = 0; j < 4; ++j)
						{
							if(camerasAddress[j] == tvssCommand->videoInput[i] )
							{
								++find_cameras;
								break;
							}
						}
					}
					if(find_cameras == i)
					{
						quadId = quad->getAddress();
						break;
					}
				}
				screenDetails->setScreenInfo(SCREEN_OPERATION,"1");
				screenDetails->setScreenInfo(SCREEN_SELSCREEN,quadId);
				if(TA_Base_Bus::TVSSManagerCorbaDef::QuadScreen==tvssCommand->cmdType)
				{
					strMsg.Format("Train %d assign quad (%s) on %s",tvssCommand->trainId,quadId.c_str(),monitorDisplayName);
				}
				else
				{
					strMsg.Format("Train %d acknowledge quad (%s) alarm on %s",tvssCommand->trainId,quadId.c_str(),monitorDisplayName);
				}
				if(wnd != NULL)
				{
					wnd->ShowWindow(SW_RESTORE);
					wnd->SetForegroundWindow();
				}
			}
			break;
		case TA_Base_Bus::TVSSManagerCorbaDef::SequnceScreen:
		case TA_Base_Bus::TVSSManagerCorbaDef::SequnceScreenAlarm:
			{
				std::string strInput;
				char chInput[32] = {0};
				sprintf(chInput,"%d",tvssCommand->videoInput[0]);
				strInput.append(chInput);
				screenDetails->setScreenInfo(SCREEN_OPERATION,"2");
				screenDetails->setScreenInfo(SCREEN_SELSCREEN,strInput);
				strMsg.Format("Train %d assign sequence (%d) on %s",tvssCommand->trainId,tvssCommand->videoInput[0],monitorDisplayName );
				if(wnd != NULL)
				{
					wnd->ShowWindow(SW_RESTORE);
					wnd->SetForegroundWindow();
				}
			}
			break;
		case TA_Base_Bus::TVSSManagerCorbaDef::ClearImage:
			{
				screen->elementID=0;
        m_mapTrainsInScreen.erase(screen->name);
				if(screenDetails == m_trainControllerDlg->GetMonitorScreen())
				{
					m_trainControllerDlg->ShowWindow(SW_HIDE);
				}
				strMsg.Format("Train %d clear %s",tvssCommand->trainId,monitorDisplayName);
			}
      break;
		case TA_Base_Bus::TVSSManagerCorbaDef::MappingMonitor:
			{
				screen->elementID=0;
				std::map<std::string,TA_IRS_Bus::TrainInformationTypes::TrainDetails>::iterator it = m_mapTrainsInScreen.find(screen->name);
				if(it != m_mapTrainsInScreen.end())
				{
					m_mapTrainsInScreen.erase(it);
					if(trainId > 0)
					{
						SwitchAgentCommunicator::getInstance().clearMonitor(trainId, monitorId);
					}
				}
				if(screenDetails == m_trainControllerDlg->GetMonitorScreen())
				{
					m_trainControllerDlg->ShowWindow(SW_HIDE);
				}
				strMsg.Format("Another system has mapping to %s",monitorDisplayName );
			}
			break;
		default:
			break;
		}
		m_screen.draw(*screen);
        if(tvssCommand->cmdType != TA_Base_Bus::TVSSManagerCorbaDef::ClearImage && tvssCommand->trainId > 0)
		{
			TA_IRS_Bus::TrainInformationTypes::TrainDetails train;
			train.trainId = tvssCommand->trainId; 
			TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap trainList = m_parentSelector->getTrainList();
			for(TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap::iterator it = trainList.begin(); it!=trainList.end(); ++it)
			{
				if(it->second.trainId == train.trainId)
				{
					train = it->second;
					break;
				}
			}
			m_mapTrainsInScreen[screen->name]=train;
		}
		if(!m_trainControllerDlg->IsWindowVisible())
		{
            m_trainControllerDlg->ClearMonitorScreen();
		}
		ClearMessageList(screen);
		CListBox* listBox = (CListBox*)( m_wnd->GetDlgItem(m_msgCtrlID));
		listBox->AddString(strMsg);
	}

	void VideoDisplayPresenter::OnMouseMove(UINT nFlags, CPoint point,bool bMsgFromParent)
	{
		if(!bMsgFromParent)
			return;
		stuMonitorScreen* screen = m_screen.getScreenFromPointer(point);
		m_screen.setSelScreen(screen);
		
	}

	void VideoDisplayPresenter::OnLButtonDown(UINT nFlags, CPoint point,bool bMsgFromParent)
	{
	}

	void VideoDisplayPresenter::OnLButtonUp(UINT nFlags, CPoint point,bool bMsgFromParent)
	{
		stuMonitorScreen* screen = m_screen.getScreenFromPointer(point);
		m_screen.setSelScreen(screen);
		stuMonitorScreen* selScreen = m_screen.getSelScreen();

			//SwitchAgentCommunicator::getInstance().setSequence((*it).trainId,TA_IRS_Bus::ProtocolCctvTypes::SequenceOne);
		if(selScreen!=NULL)
		{	
			if(bMsgFromParent)		////message from parent window,drag message
			{
				TA_IRS_Bus::TrainInformationTypes::TrainDetailsList trainList=m_parentGUI->getSelectedTrainData( );
				if(1!=trainList.size())
				 return;
				TA_IRS_Bus::TrainInformationTypes::TrainDetailsList::iterator it=trainList.begin();
				//if(ScreenOverAll!=selScreen->type)
				{
					if(NULL==selScreen->monitor)
						return;
					
					if(!AssignSequence(1,(*it).trainId))
					//unsigned char monitorID = atoi(selScreen->monitor->getAddress().c_str());
					//if(!SwitchAgentCommunicator::getInstance().setSequence((*it).trainId,monitorID, 1))
						return;
				}
			//	else
			//	{
			//		if(!AssignTrainToAllMonitors(*it))
			//			return;
			//	}	

				
				m_screen.setSelElementID((*it).trainId);
				CString strMsg;
				switch(selScreen->type)
				{
				case ScreenWall:
					strMsg.Format("Train %s assigned to Wall Monitor %d",(*it).serviceNumber.c_str(),m_screen.getSelScreen()->index); 
					break;
				case ScreenConsole:
					strMsg.Format("Train %s assigned to Console Monitor %d",(*it).serviceNumber.c_str(),m_screen.getSelScreen()->index); 
					break;
				case ScreenOverAll:
					strMsg.Format("Train %s assigned to OverAll Display System",(*it).serviceNumber.c_str()); 
					break;
				default:
					break;
				}
				m_mapTrainsInScreen[selScreen->name]=*it;
				//TA_IRS_Bus::TrainInformationTypes::TrainDetails train=(*it);
				MonitorScreen* details=m_mapScreenDetails[selScreen->name];
				details->setScreenInfo(SCREEN_OPERATION,"2");
				details->setScreenInfo(SCREEN_SELSCREEN,"1");
				//details->setScreenInfo(SCREEN_TRAININFO,(void*)(&train));
				ClearMessageList(selScreen);
				CListBox* listBox = (CListBox*)( m_wnd->GetDlgItem(m_msgCtrlID));
				listBox->AddString(strMsg);
			}
			m_trainControllerDlg->ClearMonitorScreen();
			if(m_trainControllerDlg->IsWindowVisible())
			{
				ShowTrainDetails();
			}
		}
		
	}

	void VideoDisplayPresenter::OnRButtonDown(UINT nFlags, CPoint point,bool bMsgFromParent)
	{
		;
	}

	void VideoDisplayPresenter::OnRButtonUp(UINT nFlags, CPoint point,bool bMsgFromParent)
	{
		if(NULL==m_wnd)
			return;
		stuMonitorScreen* screen = m_screen.getScreenFromPointer(point);
		m_screen.setSelScreen(screen);
		if(screen==NULL)
			return;
	//	void* pTrain = m_mapScreenDetails[screen->name]->getScreenInfo(SCREEN_TRAININFO);
		if(m_mapTrainsInScreen.end()==m_mapTrainsInScreen.find(screen->name))
			return;
		CMenu menu;
		menu.LoadMenuA(IDR_VD_MENU_TRAIN_CONTROLLER);
		CMenu* pTrainMenu = menu.GetSubMenu( 0 );
		//m_wnd->ClientToScreen(&point);
		UINT nCmd = (UINT) pTrainMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,point.x, point.y, m_wnd);

		switch(nCmd)
		{
		case IDM_DV_TR_SHOW_DETAILS:
			ShowTrainDetails();
			break;
		case IDM_DV_TR_CLEAR_MONITOR:
			ClearMonitor();
		
			break;
		default:
			break;
		}
	}

	void VideoDisplayPresenter::ShowTrainDetails()
	{
		stuMonitorScreen* selScreen = m_screen.getSelScreen();
		if(selScreen!=NULL && selScreen->elementID>0)
		{
			m_trainControllerDlg->SetTrainDetails(&m_mapTrainsInScreen[selScreen->name]);
			m_trainControllerDlg->SetMonitorScreen(m_mapScreenDetails[selScreen->name]);
			m_trainControllerDlg->ShowWindow(SW_SHOW);
		}
	}
}
