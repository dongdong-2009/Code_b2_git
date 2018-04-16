#ifndef _MONITOR_SCREEN_H_
#define _MONITOR_SCREEN_H_

#include "afxwin.h"
#include "Monitor.h"
#include "VideoInput.h"
#include <list>
#include <map>
#include <string>


#define SCREEN_TRAININFO			"ScreenTrainInfo"
#define SCREEN_OPERATION			"ScreenOperation"
#define SCREEN_SELSCREEN			"ScreenSelScreen"


class CLabel;
namespace TA_IRS_App
{
	enum ScreenType
	{
		ScreenWall,
		ScreenConsole,
		ScreenOverAll,
		ScreenSingleMonitor,
		ScreenQuad
	};

	enum ElementType
	{
		ElementTrain,
		ElementCamera,
		ElementQuad,
		ElementSequence
	};

	struct stuMonitorScreen
	{
		std::string name;
		UINT resMap,resText;
		UINT index;
		Monitor* monitor;
		CLabel* monitorTextCtl;
		ScreenType type;
		bool bSelected;
		ElementType elementType;
		unsigned char elementID;
		bool operator == (const stuMonitorScreen& screen );
		bool operator != (const stuMonitorScreen& screen );
		stuMonitorScreen();
	} ;

	class MonitorScreen
	{
	public:
		MonitorScreen();
		~MonitorScreen();
		void setWnd(CWnd* wnd);
		void insertScreen(std::string name,UINT resMap,UINT resText,UINT index,ScreenType type,ElementType elementType = ElementTrain,unsigned char elementID=0);
		void setMonitor(UINT index,ScreenType type,Monitor* monitor);
		void clearScreen();
		void draw(stuMonitorScreen screen);
		void setMaps(HBITMAP mapWall,HBITMAP mapWallH,HBITMAP mapConsole,HBITMAP mapConsoleH,HBITMAP mapOverAll,HBITMAP mapOverAllH);
		void setMaps(ScreenType type,HBITMAP map,HBITMAP mapH);
		void drawAll();
		stuMonitorScreen* getScreen(UINT index,ScreenType type);
		std::list<stuMonitorScreen*> getScreenList();
		stuMonitorScreen* getSelScreen();
		Monitor* getSelScreenMonitor();
		stuMonitorScreen* getScreenByMonitorAddress(UINT addr);
		void setSelScreen(stuMonitorScreen* screen);
		stuMonitorScreen* getScreenFromPointer(CPoint pt);
		void setElementID(stuMonitorScreen* screen,char trainID);
		void setSelElementID(char trainID);
		void setScreenInfo(std::string name,std::string info);
		std::string getScreenInfo(std::string name);
		bool isEmpty();
	private:
		std::list<stuMonitorScreen*> m_monitorList;
		UINT m_numConsole,m_numWall,m_numOverall;
		CWnd* m_wnd;
		stuMonitorScreen* m_selScreen;
		HBITMAP m_mapWall,m_mapWallH,m_mapConsole,m_mapConsoleH,m_mapOverAll,m_mapOverAllH;
		HBITMAP m_mapSingleMonitor,m_mapSingleMonitorH,m_mapQuad,m_mapQuadH;
		std::map<std::string,std::string> m_screenInfo;
	};
}
#endif
