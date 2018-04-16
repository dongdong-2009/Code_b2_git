#include "MonitorScreen.h"
#define LAB_TEXT_WIDTH		100
#define LAB_TEXT_HIGH		16
namespace TA_IRS_App
{
	bool stuMonitorScreen::operator == (const stuMonitorScreen& screen)
	{
		if(screen.name!=name || screen.index!=index || screen.resMap!=resMap || screen.resText!=resText || screen.type!=type
			|| screen.elementType!=elementType || screen.elementID!=elementID || screen.monitor!=monitor)
			return false;
		return true;
	}

	bool stuMonitorScreen::operator != (const stuMonitorScreen& screen)
	{
		if(screen.name!=name || screen.index!=index || screen.resMap!=resMap || screen.resText!=resText || screen.type!=type
			|| screen.elementType!=elementType || screen.elementID!=elementID || screen.monitor!=monitor)
			return true;
		return false;
	}

	MonitorScreen::MonitorScreen()
	 :m_wnd(NULL),m_selScreen(NULL),m_name("MainScreen")
	{
	}

	MonitorScreen::MonitorScreen(std::string name)
		:m_wnd(NULL),m_selScreen(NULL),m_name(name)
	{
	}

	MonitorScreen::~MonitorScreen()
	{
		clearScreen();
	}

	bool MonitorScreen::operator == (const MonitorScreen& monitorScreen)
	{
		if(monitorScreen.m_name==m_name )
			return false;
		return true;
	}

	void MonitorScreen::setWnd(CWnd* wnd)
	{
		m_wnd=wnd;
	}

	void MonitorScreen::insertScreen(std::string name,UINT resMap,UINT resText,UINT index,ScreenType type,ElementType elementType,unsigned char elementID)
	{
		stuMonitorScreen* screen=new stuMonitorScreen() ;
		screen->name=name;
		screen->resMap=resMap;
		screen->resText=resText;
		screen->index=index;
		screen->type=type;
		screen->elementType=elementType;
		screen->bSelected=false;
		screen->elementID=elementID;
		screen->monitor=NULL;
		m_monitorList.push_back(screen);
	}

	void MonitorScreen::setMonitor(UINT index,ScreenType type,TrainBorneMonitor* monitor)
	{
		std::list<stuMonitorScreen*>::iterator it=m_monitorList.begin();
		for(;it!=m_monitorList.end();it++)
		{
			if(index==(*it)->index && type == (*it)->type )
			{
				(*it)->monitor=monitor;
				break;
			}
		}
	}

	void MonitorScreen::clearScreen()
	{
		std::list<stuMonitorScreen*>::iterator it=m_monitorList.begin();
		for(;it!=m_monitorList.end();it++)
		{
			delete (*it);
		}
		m_monitorList.clear();
	}

	void MonitorScreen::draw(stuMonitorScreen screen)
	{
		if(NULL==m_wnd)
			return;
		CStatic* map=(CStatic*)(m_wnd->GetDlgItem(screen.resMap));
		CWnd* lab=m_wnd->GetDlgItem(screen.resText);
		CRect rectMap ,rectWnd;
		CString str;
		m_wnd->GetWindowTextA(str);
		m_wnd->GetWindowRect(&rectWnd);
		map->GetWindowRect(&rectMap);
		CPoint pt(rectMap.left,rectMap.top);
		ScreenToClient(m_wnd->m_hWnd,&pt);
		lab->ModifyStyle(0,SS_CENTER);
		lab->MoveWindow(pt.x+rectMap.Width()/2-LAB_TEXT_WIDTH/2,pt.y+rectMap.Height()/2-LAB_TEXT_HIGH/2,LAB_TEXT_WIDTH,LAB_TEXT_HIGH );
		
		if(NULL==map)
			return;
		switch(screen.type)
		{
		case ScreenWall:
			map->SetBitmap((screen.bSelected && (screen.monitor!=NULL || screen.type>=ScreenOverAll))?m_mapWallH:m_mapWall);
			break;
		case ScreenConsole:
			map->SetBitmap((screen.bSelected && (screen.monitor!=NULL || screen.type>=ScreenOverAll))?m_mapConsoleH:m_mapConsole);
			break;
		case ScreenOverAll:
			map->SetBitmap((screen.bSelected && (screen.monitor!=NULL || screen.type>=ScreenOverAll))?m_mapOverAllH:m_mapOverAll);
			break;
		case ScreenSingleMonitor:
			map->SetBitmap((screen.bSelected && (screen.monitor!=NULL || screen.type>=ScreenOverAll))?m_mapSingleMonitorH:m_mapSingleMonitor);
			break;
		case ScreenQuad:
			map->SetBitmap((screen.bSelected && (screen.monitor!=NULL || screen.type>=ScreenOverAll))?m_mapQuadH:m_mapQuad);
			break;
		default:
			break;
		}
		CString strText="";
		if(screen.elementID>0)
		{
			switch(screen.elementType)
			{
			case ElementTrain: 
				strText.Format("Train %d",screen.elementID);
				break;
			case ElementCamera:
				strText.Format("Camera %d",screen.elementID);
				break;
			case ElementQuad:
				strText.Format("Quad %d",screen.elementID);
				break;
			case ElementSequence:
				strText.Format("Sequence %d",screen.elementID);
				break;
			default:
				break;
			}
		}
		((CDialog*)m_wnd)->SetDlgItemText(screen.resText,strText);
		
	}

	void MonitorScreen::setMaps(HBITMAP mapWall,HBITMAP mapWallH,HBITMAP mapConsole,HBITMAP mapConsoleH,HBITMAP mapOverAll,HBITMAP mapOverAllH)
	{
		m_mapWall=mapWall;
		m_mapWallH=mapWallH;
		m_mapConsole=mapConsole;
		m_mapConsoleH=mapConsoleH;
		m_mapOverAll=mapOverAll;
		m_mapOverAllH=mapOverAllH;
	}
	
	void MonitorScreen::setMaps(ScreenType type,HBITMAP map,HBITMAP mapH)
	{
		switch(type)
		{
		case ScreenWall:
			m_mapWall=map;
			m_mapWallH=mapH;
			break;
		case ScreenConsole:
			m_mapConsole=map;
			m_mapConsoleH=mapH;
			break;
		case ScreenOverAll:
			m_mapOverAll=map;
			m_mapOverAllH=mapH;
			break;
		case ScreenSingleMonitor:
			m_mapSingleMonitor=map;
			m_mapSingleMonitorH=mapH;
			break;
		case ScreenQuad:
			m_mapQuad=map;
			m_mapQuadH=mapH;
			break;
		default:
			break;
	
		}
	}

	void MonitorScreen::drawAll()
	{
		std::list<stuMonitorScreen*>::iterator it=m_monitorList.begin();
		for(;it!=m_monitorList.end();it++)
		{
			stuMonitorScreen* screen=(*it);
			draw(*screen);
		}
	}

	stuMonitorScreen* MonitorScreen::getScreen(UINT index,ScreenType type)
	{
		std::list<stuMonitorScreen*>::iterator it=m_monitorList.begin();
		for(;it!=m_monitorList.end();it++)
		{
			stuMonitorScreen* screen=(*it);
			if(index==screen->index && type == screen->type )
				return screen;
		}
		return NULL;
	}

	stuMonitorScreen* MonitorScreen::getSelScreenByMonitorAddress(UINT addr)
	{
		std::list<stuMonitorScreen*>::iterator it=m_monitorList.begin();
		for(;it!=m_monitorList.end();it++)
		{
			stuMonitorScreen* screen=(*it);
			if(NULL!=screen->monitor && addr==atoi(screen->monitor->getAddress().c_str()) )
				return screen;
		}
		return NULL;
	}

	stuMonitorScreen* MonitorScreen::getSelScreenByMonitorPkey(unsigned long pkey)
	{
		std::list<stuMonitorScreen*>::iterator it=m_monitorList.begin();
		for(;it!=m_monitorList.end();it++)
		{
			stuMonitorScreen* screen=(*it);
			if(NULL!=screen->monitor && pkey==screen->monitor->getKey() )
				return screen;
		}
		return NULL;
	}

	std::list<stuMonitorScreen*> MonitorScreen::getScreenList()
	{
		return m_monitorList;
	}

	stuMonitorScreen* MonitorScreen::getSelScreen()
	{
		return m_selScreen;
	}

	void MonitorScreen::setSelScreen(stuMonitorScreen* screen)
	{
		if(NULL==screen || screen->monitor==NULL)
		{	
			if(m_selScreen!=NULL)
			{
				m_selScreen->bSelected=false;
				draw(*m_selScreen);
				m_selScreen=NULL;
			}
		}
		else
		{
			if(m_selScreen!=NULL)
			{
				//if(m_selScreen->index!=screen->index || m_selScreen->type!=screen->type)
				if(m_selScreen!=screen )
				{	
					m_selScreen->bSelected=false;
					draw(*m_selScreen);
					m_selScreen=screen;
					m_selScreen->bSelected=true;
					draw(*m_selScreen);
				}
			}
			else
			{
				
				m_selScreen=screen;
				m_selScreen->bSelected=true;
				draw(*m_selScreen);
			}
		}
	}

	stuMonitorScreen* MonitorScreen::getScreenFromPointer(CPoint pt)
	{
		std::list<stuMonitorScreen*>::iterator it=m_monitorList.begin();
		CWnd* ctrl;
		CRect rect;
		for(;it!=m_monitorList.end();it++)
		{
			stuMonitorScreen* screen=(*it);
			ctrl=m_wnd->GetDlgItem(screen->resMap);
			ctrl->GetWindowRect(&rect);
			if(rect.PtInRect(pt))
				return screen;
		}
		return NULL;
	}

	void MonitorScreen::setElementID(stuMonitorScreen* screen,char elementID)
	{
		if(m_wnd==NULL || screen==NULL)
			return;
		screen->elementID=elementID;
		draw(*screen);
	}
	
	void MonitorScreen::setSelElementID(char elementID)
	{
		if(m_wnd==NULL || m_selScreen==NULL)
			return;
		m_selScreen->elementID=elementID;
		draw(*m_selScreen);
	}

	void MonitorScreen::setScreenInfo(std::string name,std::string info)
	{
		m_screenInfo[name]=info;
	}

	std::string MonitorScreen::getScreenInfo(std::string name)
	{
		return m_screenInfo[name];
	}
}
