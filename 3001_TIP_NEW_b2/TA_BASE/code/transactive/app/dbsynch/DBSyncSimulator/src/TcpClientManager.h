#if !defined(__C_TCP_CLIENT_MANAGER_H__)
#define __C_TCP_CLIENT_MANAGER_H__
			   

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


#include "CommonDef.h"
#include "CommonData.h"
#include "Buffer.h"




NS_BEGIN(TA_DB_Sync)

using namespace TA_Base_Core;

class IObserverRcvMsg;
class CTcpClientWrapper;


class CTcpClientManager 
{

public:
	static CTcpClientManager& getInstance();
	static void removeInstance();

private:
	static CTcpClientManager*								   m_pInstance;

	static TA_Base_Core::NonReEntrantThreadLockable			   m_instanceLock;

private:
	CTcpClientManager(void);	
	~CTcpClientManager(void);

	DEF_DISABLE_COPY_AND_ASSIGNMENT(CTcpClientManager);



public:	
	std::string		createNewTcpClient( const std::string& strServerIp, const std::string& strServerPort, const std::string& strQName );
	int				registerObserver(IObserverRcvMsg* Handler);

	int				connectToServer(const std::string& strServerID);//connect Res   0-ok
	int             sendDataToServer(const std::string& strServerID, PMSGPKG pSendMsgPakage);//send res 0-ok 

	int			    removeTcpClient(const std::string& strServerID);
private:
	CTcpClientWrapper*			m_pTcpClientWrapper;
};

NS_END(TA_DB_Sync)

#endif//__C_TCP_CLIENT_MANAGER_H__

