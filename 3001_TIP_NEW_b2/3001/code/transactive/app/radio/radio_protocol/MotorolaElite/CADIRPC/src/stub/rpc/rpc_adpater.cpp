/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC/src/stub/rpc/rpc_adpater.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */
#include "rpc_adpater.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpSocket.h"
#include "ace/OS.h"

class CADIRPCManager : public TA_Base_Core::ITcpSocketObserver
{
public:
	static CADIRPCManager* getInstance()
	{
		if(_instance == 0)
		{
			_instance = new CADIRPCManager;
		}
		return _instance;
	}

	/**
	 * Establish socket connection to SDR
	 */
	void connect(const std::string& m_host, const std::string& m_port);
	
	/**
	 * Terminate socket connection with SDR
	 */
	void disconnect();

	// ITcpSocketObserver methods
	virtual void processReceivedData( std::vector<unsigned char> & pData,
                                 	  TA_Base_Core::ISocket * pSocket );

	virtual void writeFailed();

	virtual bool connectionLost();

	virtual void connectionEstablished();

	void processData(std::vector<unsigned char> & pData);

	TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpSocket>*	m_Socket;	//the observed socket

	bool m_loginSuccess;

private:
	CADIRPCManager();
	static CADIRPCManager* _instance;


    std::vector<unsigned char> m_receivedData;
};

CADIRPCManager* CADIRPCManager::_instance = NULL;

CADIRPCManager::CADIRPCManager()
: m_Socket(NULL), m_loginSuccess(false)
{

}

void CADIRPCManager::connect(const std::string& m_host, const std::string& m_port)
{
     m_Socket = new TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpSocket>(m_host,m_port);
     m_Socket->addObserver(*this);
     m_Socket->start();
}

void CADIRPCManager::disconnect()
{
    if (m_Socket != NULL)
    {
        // close socket connection
        m_Socket->removeObserver(*this);
        if ( m_Socket != NULL)
        {
            m_Socket->terminateAndWait();
            delete m_Socket;
            m_Socket = NULL;
        }
    }
}

void CADIRPCManager::processReceivedData( std::vector<unsigned char> & pData,
                                 	  TA_Base_Core::ISocket * pSocket )
{
	//1 byte length + data
	try{
		m_receivedData.insert(m_receivedData.end(), pData.begin(), pData.end());

		pData.clear();

		while (m_receivedData.size() >= 1)
		{
			int length = (int)m_receivedData[0];

			if (length > (int) m_receivedData.size() + 1)
			{
				// assume length cannot be more than 4K
				if (length > 4000)
				{
					m_receivedData.clear();
				}

				// wait till more data comes
				return;
			}

			std::vector<unsigned char> temp;
			temp.assign(m_receivedData.begin(), m_receivedData.begin() + length + 1);
			processData(temp);

			m_receivedData.erase(m_receivedData.begin(), m_receivedData.begin() + length + 1);
		}
	}
	catch(...)
	{
		
	}
}

void CADIRPCManager::processData(std::vector<unsigned char> & pData)
{
	try
	{
		switch(pData[CADI_PDUTYPE_INDEX])
		{
		case CADI_LOGIN_RESP:
			if(pData[CADI_PDUTYPE_INDEX+1])
				m_loginSuccess = true;
			else
				m_loginSuccess = false; 
			break;
		}
	}
	catch (...)
	{
	}
}
void CADIRPCManager::writeFailed()
{

}

bool CADIRPCManager::connectionLost()
{
	return false;
}

void CADIRPCManager::connectionEstablished()
{
}


/////////////////////////////////////////////////////////////////
// Adapter
void CADIConnect(char* host)
{
	CADIRPCManager::getInstance()->connect(std::string(host), "5001");
}

void CADIRegroup(char * pData)
{
	CADI_MSG msg;
	msg.length = sizeof(CADI_MSG)-1;
	msg.iMsgType = CADI_MSG::CADI_MSG_TYPE::CADI_REGROUP;
	msg.data.regroup.target_radio_id = ((CADI_REGROUP_DATA*)pData)->target_radio_id;
	msg.data.regroup.target_group_id = ((CADI_REGROUP_DATA*)pData)->target_group_id;
	CADIRPCManager::getInstance()->m_Socket->write((char*)&msg,sizeof(CADI_MSG));
}

void CADICancelRegroup(char * pData )
{
	CADI_MSG msg;
	msg.length = sizeof(CADI_MSG)-1;
	msg.iMsgType = CADI_MSG::CADI_MSG_TYPE::CADI_XREGROUP;
	msg.data.regroup.target_radio_id = ((CADI_REGROUP_DATA*)pData)->target_radio_id;
	msg.data.regroup.target_group_id = ((CADI_REGROUP_DATA*)pData)->target_group_id;
	CADIRPCManager::getInstance()->m_Socket->write((char*)&msg,sizeof(CADI_MSG));
}

bool CADILogin(char * pData)
{
	CADI_MSG msg;
	msg.length = sizeof(CADI_MSG)-1;
	msg.iMsgType = CADI_MSG::CADI_MSG_TYPE::CADI_LOGIN;
	memcpy(&msg.data.login,pData,sizeof(CADI_LOGIN_DATA));

	CADIRPCManager::getInstance()->m_loginSuccess = false;
	CADIRPCManager::getInstance()->m_Socket->write((char*)&msg,sizeof(CADI_MSG));

	int i = 0;
	while (i < 5)
	{
		if (CADIRPCManager::getInstance()->m_loginSuccess == true)
		{
			return true;
		}

		ACE_OS::sleep(2);
		++i;
	}
	return false;
}

void CADILogout()
{
	CADIRPCManager::getInstance()->disconnect();
}
