
#include "iostream.h"
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <ace/Process_Mutex.h>


class SimulatorManager
{
public:
	SimulatorManager(TCHAR *name);
	void killSimulator();
	void showCmdline();
	void sendMessage();
	void setProcessName(const std::string &name);
	void setTitleName(const std::string &location);
	void setMessage(const std::string &msg);
private:
	void sendMessageToPorcessWindow(std::string &msg);
	int	getAllMatchProcessPid(std::string &name);
	static  BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam);
	

private:
	ACE_Process_Mutex m_processMutex;
	static std::ofstream	m_logFile;

	std::string m_processName;
	static std::string m_titleName;
	std::string m_msg;

	static std::map<DWORD, std::string> m_maps;
	static int totalWind;
};
