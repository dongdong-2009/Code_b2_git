#include <signal.h>
 
#include "core/threads/src/Thread.h"

#include "CommonData.h"
#include "CommonDef.h"
#include "UtilityFun.h"	 
#include "dbSynTestClientManager.h"



using namespace TA_DB_Sync;

bool					g_b_main_continue = true;


void usr_signal(int SigNo)
{
	if (SigNo == SIGINT)
	{
		g_b_main_continue = false;
	}
	
#ifndef WIN32
	if (SigNo == SIGHUP)
	{
	}
#endif


}


int main( int argc, char* argv[] )
{ 
	int nIndex = 1;
	
	signal(SIGINT, usr_signal);
#ifndef WIN32
	signal(SIGHUP, usr_signal);	//close putty
#endif
	
	CDbSynTestClientManager* pManager = NULL;
	std::string strCfgFile;
	
	pManager = new CDbSynTestClientManager();
	strCfgFile = defconfig_Cfg_FileName;
	pManager->setConFigFile(strCfgFile);
	pManager->start();
	
	
	while (g_b_main_continue)
	{
		if (pManager->isFinishWork())
		{
			g_b_main_continue = false;
		}
		TA_Base_Core::Thread::sleep(10000);
	}
	
	if (NULL != pManager)
	{
		pManager->terminateAndWait();
		DEF_DELETE(pManager);
	}	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"exit main()");  

	return 0;
}

