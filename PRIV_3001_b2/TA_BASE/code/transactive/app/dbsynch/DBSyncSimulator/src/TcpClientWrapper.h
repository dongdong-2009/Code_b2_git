#if !defined(__C_TCP_CLIENT_WRAPPER_H__)
#define __C_TCP_CLIENT_WRAPPER_H__


#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "CommonDef.h"
#include "CommonData.h"

#include "Buffer.h"




using namespace TA_Base_Core;

NS_BEGIN(TA_DB_Sync)

class IObserverRcvMsg;
class ITcpComType;


class CTcpClientWrapper
{
private:
	typedef std::map<std::string, ITcpComType*>              vecClientsManagerConT;
	typedef std::map<std::string, ITcpComType*>::iterator    vecClientsManagerConIterT;
	typedef std::map<std::string, ITcpComType*>::value_type  vecClientsManagerValueType;

public:
	CTcpClientWrapper();	
	~CTcpClientWrapper();

public:
	std::string		createNewTcpClient(const std::string& strServerIp, 
						const std::string& strServerPort, 
						const std::string& strQName);//return strServerID
	int				registerObserver(IObserverRcvMsg* Handler );

	int				connectToServer(const std::string& strServerID);//connect Res   0-ok
	int             sendDataToServer(const std::string& strServerID, PMSGPKG pSendMsgPakage);//send res 0-ok 

	int			    removeTcpClient(const std::string& strServerID);


private:
	std::string		_GetServerID(const std::string& strServerIp, 
								const std::string& strServerPort,
								const std::string& strQName);
	int            _FreeLstMsgItems(lstMsgItemsConT& lstMsgItems);
	int			   _FreeLstMsgItem(lstMsgItemConT& lstMsgItem);
private:  
	vecClientsManagerConT									m_mapAllClients;
	TA_Base_Core::NonReEntrantThreadLockable				m_AllClientsMapLock;

private:
	int												        m_nClientType;
	IObserverRcvMsg*                                        m_pDispatchMsgHandle;
};

NS_END(TA_DB_Sync)

#endif//__C_TCP_CLIENT_WRAPPER_H__
