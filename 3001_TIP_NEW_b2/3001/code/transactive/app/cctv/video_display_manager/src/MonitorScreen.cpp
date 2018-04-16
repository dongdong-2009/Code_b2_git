#include "stdafx.h"
#include "MonitorScreen.h"
#include "ToolTipLabel.h"
#define LAB_TEXT_HIGH       16

namespace TA_IRS_App
{
    bool stuMonitorScreen::operator == (const stuMonitorScreen& screen)
    {
        return (screen.name == name && screen.index == index && screen.resMap == resMap && screen.resText == resText && screen.type == type
                && screen.elementType == elementType && screen.elementID == elementID && screen.monitor == monitor && screen.monitorTextCtl == monitorTextCtl);
    }

    bool stuMonitorScreen::operator != (const stuMonitorScreen& screen)
    {
        return !((*this) == screen);
    }

	stuMonitorScreen::stuMonitorScreen():monitor(NULL), monitorTextCtl(NULL)
	{
	}

    MonitorScreen::MonitorScreen()
        : m_wnd(NULL), m_selScreen(NULL)
    {
    }

    MonitorScreen::~MonitorScreen()
    {
        clearScreen();
    }

    void MonitorScreen::setWnd(CWnd* wnd)
    {
        m_wnd = wnd;
    }

    void MonitorScreen::insertScreen(std::string name, UINT resMap, UINT resText, UINT index, ScreenType type, ElementType elementType, unsigned char elementID)
    {
        stuMonitorScreen* screen = new stuMonitorScreen() ;
        screen->name = name;
        screen->resMap = resMap;
        screen->resText = resText;
        screen->index = index;
        screen->type = type;
        screen->elementType = elementType;
        screen->bSelected = false;
        screen->elementID = elementID;
        screen->monitor = NULL;
        screen->monitorTextCtl = new CLabel;
        screen->monitorTextCtl->SubclassWindow(m_wnd->GetDlgItem(resText)->m_hWnd);
        m_monitorList.push_back(screen);
        CStatic* map = (CStatic*)(m_wnd->GetDlgItem(resMap));
        CRect rectMap , rectWnd;
        m_wnd->GetWindowRect(&rectWnd);
        map->GetWindowRect(&rectMap);
        CPoint pt(rectMap.left, rectMap.top);
        ScreenToClient(m_wnd->m_hWnd, &pt);
        if(ScreenConsole == type)
        {
            screen->monitorTextCtl->MoveWindow(pt.x + 16, pt.y + rectMap.Height() / 2 - LAB_TEXT_HIGH / 2, rectMap.Width() - 32, LAB_TEXT_HIGH);
        }
        else
        {
            screen->monitorTextCtl->MoveWindow(pt.x + 4, pt.y + rectMap.Height() / 2 - LAB_TEXT_HIGH / 2, rectMap.Width() - 8, LAB_TEXT_HIGH);
        }
        screen->monitorTextCtl->SetTransparent(TRUE);
        screen->monitorTextCtl->ModifyStyle(0, SS_CENTER | SS_WORDELLIPSIS);
        screen->monitorTextCtl->SetFontName("Microsoft Sans Serif");
        screen->monitorTextCtl->SetFontSize(8);
    }

    void MonitorScreen::setMonitor(UINT index, ScreenType type, Monitor* monitor)
    {
        std::list<stuMonitorScreen*>::iterator it = m_monitorList.begin();
        for(; it != m_monitorList.end(); it++)
        {
            if(index == (*it)->index && type == (*it)->type)
            {
                (*it)->monitor = monitor;
                break;
            }
        }
    }

    void MonitorScreen::clearScreen()
    {
        std::list<stuMonitorScreen*>::iterator it = m_monitorList.begin();
        for(; it != m_monitorList.end(); it++)
        {
            delete(*it)->monitorTextCtl;
            delete(*it);
        }
        m_monitorList.clear();
    }

    void MonitorScreen::draw(stuMonitorScreen screen)
    {
        CStatic* map = (CStatic*)(m_wnd->GetDlgItem(screen.resMap));
        CLabel* lab = (CLabel*)(m_wnd->GetDlgItem(screen.resText));
        if(NULL != m_wnd)
        {
            if(NULL != map)
            {
                switch(screen.type)
                {
                    case ScreenWall:
                        map->SetBitmap((screen.bSelected && (screen.monitor != NULL || screen.type >= ScreenOverAll)) ? m_mapWallH : m_mapWall);
                        break;
                    case ScreenConsole:
                        map->SetBitmap((screen.bSelected && (screen.monitor != NULL || screen.type >= ScreenOverAll)) ? m_mapConsoleH : m_mapConsole);
                        break;
                    case ScreenOverAll:
                        map->SetBitmap((screen.bSelected && (screen.monitor != NULL || screen.type >= ScreenOverAll)) ? m_mapOverAllH : m_mapOverAll);
                        break;
                    case ScreenSingleMonitor:
                        map->SetBitmap((screen.bSelected && (screen.monitor != NULL || screen.type >= ScreenOverAll)) ? m_mapSingleMonitorH : m_mapSingleMonitor);
                        break;
                    case ScreenQuad:
                        map->SetBitmap((screen.bSelected && (screen.monitor != NULL || screen.type >= ScreenOverAll)) ? m_mapQuadH : m_mapQuad);
                        break;
                    default:
                        break;
                }
                CString strText = "";
                if(screen.monitor != NULL)
                {
                    if(screen.monitor->getTriggeringAlarmMode())
                    {
                        strText.Format("%s", "ALARM MODE");
                    }
                    else if(screen.monitor->getShowingAckAlarm())
                    {
                        std::vector<VideoOutput::CCTVTriggeredAlarm> alarms = screen.monitor->getAlarmStackItems();
                        if(!alarms.empty())
                        {
                            VideoOutput::CCTVTriggeredAlarm ackAlarm = *(alarms.begin());
                            strText.Format("%s_EVT_%d", ackAlarm.locationName.c_str(), ackAlarm.eventId);
                        }
                    }
                    else if(screen.monitor->isQuadInput())
                    {
                        strText.Format("%s", "QUAD");
                    }
                    else if(screen.monitor->getInput() != NULL)
                    {
                        VideoInput* input = screen.monitor->getInput();
                        strText.Format("%s", input->getName().c_str());
                    }
                }
                lab->SetText(strText);
            }
        }
    }

    void MonitorScreen::setMaps(HBITMAP mapWall, HBITMAP mapWallH, HBITMAP mapConsole, HBITMAP mapConsoleH, HBITMAP mapOverAll, HBITMAP mapOverAllH)
    {
        m_mapWall = mapWall;
        m_mapWallH = mapWallH;
        m_mapConsole = mapConsole;
        m_mapConsoleH = mapConsoleH;
        m_mapOverAll = mapOverAll;
        m_mapOverAllH = mapOverAllH;
    }

    void MonitorScreen::setMaps(ScreenType type, HBITMAP map, HBITMAP mapH)
    {
        switch(type)
        {
            case ScreenWall:
                m_mapWall = map;
                m_mapWallH = mapH;
                break;
            case ScreenConsole:
                m_mapConsole = map;
                m_mapConsoleH = mapH;
                break;
            case ScreenOverAll:
                m_mapOverAll = map;
                m_mapOverAllH = mapH;
                break;
            case ScreenSingleMonitor:
                m_mapSingleMonitor = map;
                m_mapSingleMonitorH = mapH;
                break;
            case ScreenQuad:
                m_mapQuad = map;
                m_mapQuadH = mapH;
                break;
            default:
                break;
        }
    }

    void MonitorScreen::drawAll()
    {
        std::list<stuMonitorScreen*>::iterator it = m_monitorList.begin();
        for(; it != m_monitorList.end(); it++)
        {
            stuMonitorScreen* screen = (*it);
            draw(*screen);
        }
    }

    stuMonitorScreen* MonitorScreen::getScreen(UINT index, ScreenType type)
    {
        std::list<stuMonitorScreen*>::iterator it = m_monitorList.begin();
		stuMonitorScreen* screen = NULL;
        for(; it != m_monitorList.end(); it++)
        {
            stuMonitorScreen* screenTmp = (*it);
            if(index == screenTmp->index && type == screenTmp->type)
			{
				screen = screenTmp;
				break;
			}
        }
		return screen;
    }

    stuMonitorScreen* MonitorScreen::getScreenByMonitorAddress(UINT addr)
    {
        std::list<stuMonitorScreen*>::iterator it = m_monitorList.begin();
        stuMonitorScreen* screen = NULL;
        for(; it != m_monitorList.end(); it++)
        {
            stuMonitorScreen* screenTmp = (*it);
            if(NULL != screenTmp->monitor && addr == atoi(screenTmp->monitor->getAddress().c_str()))
			{
				screen = screenTmp;
				break;
			}
        }
		return screen;
    }

    std::list<stuMonitorScreen*> MonitorScreen::getScreenList()
    {
        return m_monitorList;
    }

    stuMonitorScreen* MonitorScreen::getSelScreen()
    {
        return m_selScreen;
    }

    Monitor* MonitorScreen::getSelScreenMonitor()
    {
		Monitor* monitor = NULL;
        if(m_selScreen != NULL)
		{
			monitor = m_selScreen->monitor;
		}
		return monitor;
    }

    void MonitorScreen::setSelScreen(stuMonitorScreen* screen)
    {
        if(NULL == screen ||  screen->monitor == NULL)
        {
            if(m_selScreen != NULL)
            {
                m_selScreen->bSelected = false;
                draw(*m_selScreen);
                m_selScreen = NULL;
            }
        }
        else
        {
            if(m_selScreen != NULL)
            {
                //if(m_selScreen->index!=screen->index || m_selScreen->type!=screen->type)
                if(m_selScreen != screen)
                {
                    m_selScreen->bSelected = false;
                    draw(*m_selScreen);
                    m_selScreen = screen;
                    m_selScreen->bSelected = true;
                    draw(*m_selScreen);
                }
            }
            else
            {
                m_selScreen = screen;
                m_selScreen->bSelected = true;
                draw(*m_selScreen);
            }
        }
    }

    stuMonitorScreen* MonitorScreen::getScreenFromPointer(CPoint pt)
    {
        std::list<stuMonitorScreen*>::iterator it = m_monitorList.begin();
        CWnd* ctrl;
        CRect rect;
        stuMonitorScreen* screen = NULL;
        for(; it != m_monitorList.end(); it++)
        {
            stuMonitorScreen* screenTmp = (*it);
            ctrl = m_wnd->GetDlgItem(screenTmp->resMap);
            ctrl->GetWindowRect(&rect);
            if(rect.PtInRect(pt))
			{
				screen = screenTmp;
				break;
			}
        }
        return screen;
    }

    void MonitorScreen::setElementID(stuMonitorScreen* screen, char elementID)
    {
        if(m_wnd != NULL && screen != NULL)
		{
	        screen->elementID = elementID;
	        draw(*screen);
		}
    }

    void MonitorScreen::setSelElementID(char elementID)
    {
		setElementID(m_selScreen, elementID);
    }

    void MonitorScreen::setScreenInfo(std::string name, std::string info)
    {
        m_screenInfo[name] = info;
    }

    std::string MonitorScreen::getScreenInfo(std::string name)
    {
        return m_screenInfo[name];
    }

    bool MonitorScreen::isEmpty()
    {
        return m_monitorList.size() == 0;
    }
}
