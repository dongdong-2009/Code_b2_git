
#include "ace/Log_Msg.h"
#include"SimulatorManager.h"

std::map<DWORD,std::string> SimulatorManager::m_maps;
std::ofstream     SimulatorManager::m_logFile;
int SimulatorManager::totalWind=0;
std::string	SimulatorManager::m_titleName = "";


SimulatorManager::SimulatorManager(TCHAR*name)
{
	if(name==NULL)
		m_processName="ProcessKill.exe";
	else
		m_processName = name;
	m_titleName="";
	m_msg="";
	m_logFile.open("SimulatorManager.log",ios::in|ios::out|ios::app);
}

void SimulatorManager::setProcessName(const std::string &name)
{
	m_processName = name;
}

void SimulatorManager::setTitleName(const std::string &title)
{
	m_titleName= title;
}

void SimulatorManager::setMessage(const std::string &msg)
{
	m_msg = msg;
}

int	SimulatorManager::getAllMatchProcessPid(std::string &name)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
    if (!hSnapshot)
    {
		cout<<"CreateToolhelp32Snapshot ERROR!"<<endl;
		return 0;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32 );

    if(!Process32First (hSnapshot, &pe32))
	{
		cout<<"Process32First ERROR!"<<endl;
	}

	m_maps.clear();
	do
	{
		if (name == pe32.szExeFile)
		{
			m_maps[pe32.th32ProcessID] = m_processName;
		}
	}
	while(Process32Next (hSnapshot, &pe32));
	return m_maps.size();
}

BOOL CALLBACK SimulatorManager::lpEnumFunc(HWND hwnd, LPARAM lParam)
{
	 char str[100];
	 DWORD mpid;
	::GetWindowText(hwnd, str, sizeof(str));
	GetWindowThreadProcessId(hwnd ,&mpid);

	std::string windowTile = str;

	if(windowTile.find(m_titleName)== std::string::npos)
		return true;

	DWORD dRet=0;
	std::map<DWORD, std::string>::iterator it;

	if( (it=m_maps.find(mpid) ) != m_maps.end() )
	{
		totalWind++;
		int ret = SendMessageTimeout(hwnd, WM_COPYDATA, 1, lParam,SMTO_ABORTIFHUNG,3000,&dRet);
		
		//	m_logFile<<static_cast<std::string>(it->second);
		m_logFile<<"--";
		m_logFile<<it->first;
		m_logFile<<std::endl;
		m_logFile<<"return:="<<ret<<"  paraRet:="<<dRet<<"  lastErr:="<<GetLastError()<<std::endl;
	}
	return true;
}

void  SimulatorManager::sendMessageToPorcessWindow(std::string &msg)
{
	//std::string sendMsg=m_location+"  "+msg;
	m_logFile<<"SendMsg < name:="<<m_processName<<"  title like:"<<m_titleName<<"  msg:="<<msg<<">"<<std::endl;
	COPYDATASTRUCT cpd;   
    cpd.dwData = 0;
	cpd.cbData = 0;
   	cpd.cbData = msg.size()+1;
    cpd.lpData =(void*)msg.c_str();
	EnumWindows(lpEnumFunc, (LPARAM)&cpd);
	m_logFile<<"Total "<<totalWind<<" Windows found"<<std::endl;
}


void SimulatorManager::sendMessage()
{
	//this->getProcessCmdLine();
	getAllMatchProcessPid(m_processName);
	sendMessageToPorcessWindow(m_msg);
}