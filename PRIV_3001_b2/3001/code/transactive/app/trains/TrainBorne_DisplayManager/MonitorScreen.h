#ifndef _MONITOR_SCREEN_H_
#define _MONITOR_SCREEN_H_

#include "afxwin.h"
#include "DisplayItem/Monitor.h"
#include <list>
#include <map>
#include <string>


#define SCREEN_TRAININFO			"ScreenTrainInfo"
#define SCREEN_OPERATION			"ScreenOperation"
#define SCREEN_SELSCREEN			"ScreenSelScreen"

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
		TrainBorneMonitor* monitor;
		ScreenType type;
		bool bSelected;
		ElementType elementType;
		unsigned char elementID;
		bool operator == (const stuMonitorScreen& screen );
		bool operator != (const stuMonitorScreen& screen );
	} ;

	class MonitorScreen
	{
	public:
		MonitorScreen();
		MonitorScreen(std::string name);
		~MonitorScreen();
		bool operator == (const MonitorScreen& monitorScreen );
		void setWnd(CWnd* wnd);
		void insertScreen(std::string name,UINT resMap,UINT resText,UINT index,ScreenType type,ElementType elementType = ElementTrain,unsigned char elementID=0);
		void setMonitor(UINT index,ScreenType type,TrainBorneMonitor* monitor);
		void clearScreen();
		void draw(stuMonitorScreen screen);
		void setMaps(HBITMAP mapWall,HBITMAP mapWallH,HBITMAP mapConsole,HBITMAP mapConsoleH,HBITMAP mapOverAll,HBITMAP mapOverAllH);
		void setMaps(ScreenType type,HBITMAP map,HBITMAP mapH);
		void drawAll();
		stuMonitorScreen* getScreen(UINT index,ScreenType type);
		std::list<stuMonitorScreen*> getScreenList();
		stuMonitorScreen* getSelScreen();
		stuMonitorScreen* getSelScreenByMonitorAddress(UINT addr);
		stuMonitorScreen* getSelScreenByMonitorPkey(unsigned long pkey);
		void setSelScreen(stuMonitorScreen* screen);
		stuMonitorScreen* getScreenFromPointer(CPoint pt);
		void setElementID(stuMonitorScreen* screen,char trainID);
		void setSelElementID(char trainID);
		void setScreenInfo(std::string name,std::string info);
		std::string getScreenInfo(std::string name);

	private:
		std::string m_name;
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
