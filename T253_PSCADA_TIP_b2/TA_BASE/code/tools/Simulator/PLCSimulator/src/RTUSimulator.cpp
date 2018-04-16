// RTUSimulator.cpp: implementation for the RTUSimulator class.
//
////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PLCSimulator.h"
#include "RTUSimulator.h"
#include "FileLoader.h"
#include "StressTaskManager.h"
#include "ReactionControl.h"
#include "PLCSimulatorDLG.h"
#include "Run.h"
#include "ace/OS_NS_netdb.h"

const char RTUSimulator::MODBUS_ERROR_CODE = 0x80;
const int RTUSimulator::MODBUS_PDU_FUNCTION_INDEX = 0;
const int RTUSimulator::MODBUS_R_RESPONSE_PDU_BTYE_NUM_INDEX = 1;
const int RTUSimulator::MODBUS_R_RESPONSE_PDU_DATA_INDEX = 2;
const int RTUSimulator::MODBUS_W_RESPONSE_PDU_START_ADDR_INDEX = 1;
const int RTUSimulator::MODBUS_W_RESPONSE_PDU_QUANTITY_OUTPUT_INDEX = 3;
const int RTUSimulator::MODBUS_ERROR_CODE_PDU_INDEX = 0;
const int RTUSimulator::MODBUS_ERROR_SUB_CODE_PDU_INDEX = 1;
const int RTUSimulator::MODBUS_REQUEST_PDU_START_ADDR_INDEX = 1;
const int RTUSimulator::MODBUS_REQUEST_PDU_QUANTITY_OUTPUT_INDEX = 3;
const int RTUSimulator::MODBUS_W_REQUEST_PDU_DATA_BYTE_NUM_INDEX = 5;
const int RTUSimulator::MODBUS_W_REQUEST_PDU_DATA_INDEX = 6;
const int RTUSimulator::STRANDARD_MODBUS_MBAP_LEN_H_INDEX = 4;
const int RTUSimulator::STRANDARD_MODBUS_MBAP_LEN_L_INDEX = 5;
const int RTUSimulator::MODBUS_MPAP_HEAD_SIZE = 7;
const int RTUSimulator::C830_MODBUS_HEAD_SIZE = 1;
char* RTUSimulator::m_configFileName = "config.txt";
RTUSimulator* RTUSimulator::m_instance = NULL;

static CReactionControl sReactionCtl;

RTUSimulator::RTUSimulator( const std::string & CfgFileName ) : 
m_log( "PLCSimulatorlog.txt" ),
m_hstress( NULL ),
m_logWnd( NULL ),
m_outputWnd( NULL ),
m_blogToFile( false ),
m_checkClientRequestAddress(false),
m_eProtocalType( STANDARDMODBUS ),
m_stressTaskMgr(NULL),
m_myDlg(NULL)
{
	m_allParameters.clear();
	m_allParameters[CONFIG_RPARAM]		= m_configFileName;
	m_allParameters[STRESS_RPARAM]		= "StressConf.txt";
	m_allParameters[SAVEDATA_PARAM]		= "SaveData.txt";
	m_allParameters[STRESSON_PARAM]		= "0";
	m_allParameters[PLCON_PARAM]		= "0";
	m_allParameters[LOGFILE_PARAM]		= "PLCSimulatorlog.txt";
	m_allParameters[REACTIONFILE_PARAM]	= "reaction.txt";
	m_allParameters[PAUSE_OUTPUT_DISPLAY]			= "0";
	m_allParameters[PAUSE_COMMUNICATION_DISPLAY]	= "0";
    m_allParameters[MODBUSTYPE_PARAM] = "standard";
    
	ParseCmdLineParameters();

	std::map<std::string, std::string>::iterator it = m_allParameters.begin();
	while ( it != m_allParameters.end())
	{
		m_log << it->first.c_str() << "=" << it->second.c_str() << std::endl;
		++it;
	}
	for ( int i = 0; i < 6; ++i )
	{
		m_pLogFile[i] = NULL;
	}
	InitSimulator();
	sReactionCtl.activate();
}

RTUSimulator::~RTUSimulator( void )
{
	for ( int i = 0; i < 6; ++i )
	{
		if( m_pLogFile[i] != NULL )
		{
			m_pLogFile[i]->Close();
			delete m_pLogFile[i];
			m_pLogFile[i] = NULL;
		}
	}
	if (m_stressTaskMgr)
	{
		delete m_stressTaskMgr;
		m_stressTaskMgr = 0;
	}

	m_log.close();
}

void RTUSimulator::InitSimulator()
{
	std::map<std::string, std::string>::iterator it = m_allParameters.find(CONFIG_RPARAM);
	if (m_allParameters.end() != it)
	{
		InitConfigFile(it->second);
	}
	else
	{
		m_log << "no config file." << std::endl;
	}

    it = m_allParameters.find(MODBUSTYPE_PARAM);
    if (m_allParameters.end() != it && "c830" == it->second)
    {
        m_eProtocalType = C830MODBUS;
		m_log << "set modbus protocal type is c830" << std::endl;
    }
    else if (m_allParameters.end() == it || "standard" == it->second)
    {
        m_eProtocalType = STANDARDMODBUS;
		m_log << "set modbus protocal type is standard" << std::endl;
    }
    else
    {
        m_log << "modbus type setting is warong. only valid with c830 or standard" << std::endl;
		MessageBox(NULL, "Modbus protocal type setting is warong. only valid with \"c830\" or \"standard\"", "Error", MB_ICONERROR);
		exit(1);
    }
	
    InitReaction();
}

int RTUSimulator::InitConfigFile( const std::string& FileName )
{
	m_mapBlocks.clear();
	m_mapExpressions.clear();
	m_cfgFile.assign(FileName);

	std::string text;
	std::ifstream ifs(FileName.c_str());

	FileLoader()(text, ifs);

	std::string::const_iterator start = text.begin();
	std::string::const_iterator end = text.end();

	boost::regex lineexpr("(\[[a-z_A-Z0-9]+])[^\n]*([^[]*)");
	boost::match_results<std::string::const_iterator> line;
	boost::match_flag_type flags = boost::match_default;

	while(boost::regex_search(start, end, line, lineexpr, flags))   
	{
		std::map<std::string, std::string>::value_type value(std::string(line[1].first, line[1].second),
			std::string(line[2].first, line[2].second));
		m_mapBlocks.insert(value);
		start = line[0].second;      
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}

	std::map<std::string, std::string>::iterator it;
	ParseBlocks( m_mapBlocks["[RTU_IDENTITY]"] );
	m_identification = m_mapExpressions["RTU_IDENTIFICATION"];
	m_location = m_mapExpressions["RTU_LOCATION"];
	ParseBlocks(m_mapBlocks["[RTU1]"]);

	std::string Address = m_mapExpressions["RTU_LAN1_IP"];

	ParseBlocks( m_mapBlocks["[RTU_POLLING]"]);
	m_portsNum = atoi(m_mapExpressions["RTU_POLLING_SOCKET_NUMBER"].c_str());

	char buffer[10];

	for( int i=1; i<=CMD_PORT; i++ )
	{
		std::string strTempA = "RTU_POLLING_SOCKET_ID";
		strTempA += std::string( itoa(i, buffer, 10) );
		/*m_portsToListen[i - 1].set( atoi(m_mapExpressions.find(strTempA)->second.c_str()),
			Address.c_str());*/
		m_portsToListen[i - 1].set( atoi(m_mapExpressions[strTempA].c_str()),
			Address.c_str());
	}

	// ParseBlocks( m_mapBlocks.find("[RTU_COMMAND]")->second );
	ParseBlocks( m_mapBlocks["[RTU_COMMAND]"] );
	// m_portsToListen[CMD_PORT].set(atoi(m_mapExpressions.find("RTU_COMMAND_SOCKET_ID")->second.c_str()), Address.c_str());
	m_portsToListen[CMD_PORT].set(atoi(m_mapExpressions["RTU_COMMAND_SOCKET_ID"].c_str()), Address.c_str());

	for ( int i=CMD_PORT+1; i<m_portsNum; i++ )
	{
		std::string strTempB = "[SWC";
		strTempB += std::string( itoa(i-CMD_PORT, buffer, 10) );
		strTempB += "]";
 		// ParseBlocks( m_mapBlocks.find(strTempB)->second );
		ParseBlocks( m_mapBlocks[strTempB] );

		std::string strTempC = "SOCKET_ID_";
		strTempC += std::string( itoa(i-CMD_PORT, buffer, 10) );
		// m_portsToListen[i].set(atoi(m_mapExpressions.find(strTempC)->second.c_str()), Address.c_str());
		m_portsToListen[i].set(atoi(m_mapExpressions[strTempC].c_str()), Address.c_str());
	}

	// m_svrTBgn = atoi( m_mapExpressions.find("RTU_TABLE_BGN")->second.c_str() );
	m_svrTBgn = atoi( m_mapExpressions["RTU_TABLE_BGN"].c_str() );
	// m_svrTEnd = atoi( m_mapExpressions.find("RTU_TABLE_END")->second.c_str() );
	m_svrTEnd = atoi( m_mapExpressions["RTU_TABLE_END"].c_str() );

	m_vtinternalTable.erase( m_vtinternalTable.begin(), m_vtinternalTable.end() );
	m_vtinternalTable.reserve( m_svrTEnd );

	for( int i=0; i<m_svrTEnd; i++ )
	{
		m_vtinternalTable.push_back(0);
	}
	LoadData(m_allParameters[SAVEDATA_PARAM]);

	// LoadDeviceMap( m_mapBlocks.find("[RTU_SWC]")->second );
	LoadDeviceMap( m_mapBlocks["[RTU_SWC]"] );

	m_blockStartAddress.clear();
	if (m_mapBlocks.end() != m_mapBlocks.find("[DATA_BLOCK_ADDRESS]"))
	{
		// ParseBlocks( m_mapBlocks.find("[DATA_BLOCK_ADDRESS]")->second );
		ParseBlocks( m_mapBlocks["[DATA_BLOCK_ADDRESS]"] );
		LoadBlockAddress();
	}
	else
	{
		m_log << "no [DATA_BLOCK_ADDRESS] config." << std::endl;
	}

	return 0;
}

void RTUSimulator::InitReaction()
{
	std::string text;
	std::ifstream ifs(m_allParameters[REACTIONFILE_PARAM].c_str());
	FileLoader()(text, ifs);

	m_mapReactionBlocks.clear();
	m_mapReactionBlocksAI.clear();

	std::string::const_iterator start = text.begin();
	std::string::const_iterator end = text.end();
	
	//boost::regex lineexpr("(D[^T]*)(T[^\n]*)");
	boost::regex lineexpr("(D[^T]*)(T[^\n]*[S]?)");
	boost::match_results<std::string::const_iterator> line;
	boost::match_flag_type flags = boost::match_default;
	
	while(boost::regex_search(start, end, line, lineexpr, flags))   
	{
		std::map<std::string, std::string>::value_type value(std::string(line[1].first, line[1].second),
			std::string(line[2].first, line[2].second));
		m_mapReactionBlocks.insert(value);
		start = line[0].second;      
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
//		m_log << std::string(line[1].first, line[1].second) << std::endl;
//		m_log << std::string(line[2].first, line[2].second) << std::endl;
	}

	start = text.begin();
	
	boost::regex lineexprAI("(A[^T]*)(T[^\n]*)");
	boost::match_results<std::string::const_iterator> lineAI;
	flags = boost::match_default;

	while(boost::regex_search(start, end, lineAI, lineexprAI, flags))   
	{
		std::map<std::string, std::string>::value_type value(std::string(lineAI[1].first, lineAI[1].second),
			std::string(lineAI[2].first, lineAI[2].second));
		m_mapReactionBlocksAI.insert(value);
		start = lineAI[0].second;      
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
//		m_log << std::string(lineAI[1].first, lineAI[1].second) << std::endl;
//		m_log << std::string(lineAI[2].first, lineAI[2].second) << std::endl;
	}
}

void RTUSimulator::ParseBlocks( std::string& str )
{
	std::string text = str;
	std::string::const_iterator start = text.begin();
	std::string::const_iterator end = text.end();

	boost::regex lineexpr("^([a-z_A-Z0-9]+)=([\.a-z_A-Z0-9]+);");
	boost::match_results<std::string::const_iterator> line;
	boost::match_flag_type flags = boost::match_default;

	while(boost::regex_search(start, end, line, lineexpr, flags))   
	{
		std::map<std::string, std::string>::value_type value(std::string(line[1].first, line[1].second),
			std::string(line[2].first, line[2].second));
		m_mapExpressions.insert(value);
		start = line[0].second;      
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}
}

void RTUSimulator::ParseReactionBlocks( std::string& str )
{
	std::string text = str;
	std::string::const_iterator start = text.begin();
	std::string::const_iterator end = text.end();

	char buffer[10];
	int i = 0;
	std::string strTempA;

	boost::regex lineexpr("(D[^D]*)");
	boost::match_results<std::string::const_iterator> line;
	boost::match_flag_type flags = boost::match_default;
	
	while(boost::regex_search(start, end, line, lineexpr, flags))   
	{
		strTempA = std::string( itoa(i, buffer, 10) );
		std::map<std::string, std::string>::value_type value(strTempA,
			std::string(line[1].first, line[1].second));
		m_mapReactionExpressions.insert(value);
		start = line[0].second;      
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
		i++;
// 		m_log << std::string(line[1].first, line[1].second) << std::endl;
	}
}

void RTUSimulator::ParseReactionBlocksAI( std::string& str )
{
	std::string text = str;
	std::string::const_iterator start = text.begin();
	std::string::const_iterator end = text.end();
	
	char buffer[10];
	int i = 0;
	std::string strTempA;
	
	boost::regex lineexpr("(A[^A]*)");
	boost::match_results<std::string::const_iterator> line;
	boost::match_flag_type flags = boost::match_default;
	
	while(boost::regex_search(start, end, line, lineexpr, flags))   
	{
		strTempA = std::string( itoa(i, buffer, 10) );
		std::map<std::string, std::string>::value_type value(strTempA,
			std::string(line[1].first, line[1].second));
		m_mapReactionExpressionsAI.insert(value);
		start = line[0].second;      
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
		i++;
// 		m_log << std::string(line[1].first, line[1].second) << std::endl;
	}
}

int RTUSimulator::StartSvr(const int i )
{
	ACE_SOCK_Acceptor acceptor;
	if ( acceptor.open (m_portsToListen[i], 1) == -1 )
	{
		m_log << i << ": listen on port:" << m_portsToListen[i].get_port_number()<< " failed." << std::endl;
		ACE_ERROR_RETURN ((LM_ERROR,
			ACE_TEXT ("%p\n"),
			ACE_TEXT ("acceptor.open")),
			100);	
	}
	
	m_log << i << ": listen on port:" << m_portsToListen[i].get_port_number() << " successful." << std::endl;
	int listenPort = m_portsToListen[i].get_port_number();
	std::map<std::string, ACE_SOCK_Stream*> clientSocks;
	std::vector<ClientRun*> clientConnections;
	while (1)
	{
		ACE_SOCK_Stream* peer = new ACE_SOCK_Stream();
		int m_tcpNoDelay = 0;
		peer->set_option( IPPROTO_TCP, TCP_NODELAY, &m_tcpNoDelay, sizeof( m_tcpNoDelay ) );

		ACE_INET_Addr peer_addr;
		ACE_Time_Value timeout (10000, 0);
//		SetData(6, 0, BYBIT, i + 11);

		if (acceptor.accept (*peer, &peer_addr, &timeout, 0) == -1)
		{
			if (ACE_OS::last_error() == EINTR)
			{
				ACE_DEBUG ((LM_DEBUG,
					ACE_TEXT ("(%P|%t) Interrupted while ")
					ACE_TEXT ("waiting for connection\n")));
			}
			else if (ACE_OS::last_error() == ETIMEDOUT)
			{
				ACE_DEBUG ((LM_DEBUG,
					ACE_TEXT ("(%P|%t) Timeout while ")
					ACE_TEXT ("waiting for connection\n")));
			}
			else if (ENOTSOCK == ACE_OS::last_error())
			{
				ACE_DEBUG ((LM_DEBUG,
					ACE_TEXT ("(%P|%t) NOTSOCK while ")
					ACE_TEXT ("waiting for connection\n")));
				break;
			}
		}
		else
		{
//			SetData(6, 1, BYBIT, i + 11);
			ACE_TCHAR peer_name[MAXHOSTNAMELEN];
			peer_addr.addr_to_string (peer_name, MAXHOSTNAMELEN);
			std::string peerAddr(peer_name);
			std::map<std::string, ACE_SOCK_Stream*>::iterator itClients;
			itClients = clientSocks.find(peerAddr);
			if (itClients != clientSocks.end() && clientSocks.size() > 6)
			{
				delete peer;
				m_log << "client:" << peerAddr.c_str() << "exist or exceed max client connections:";
				m_log << clientSocks.size()<< std::endl;
			}
			else
			{
				m_log << "add client:" << peerAddr.c_str() << std::endl;
				clientSocks[peerAddr] = peer;
				ClientRun* pClient = new ClientRun(peer, peerAddr,*this, listenPort);
				clientConnections.push_back(pClient);
				pClient->activate(THR_NEW_LWP |
					THR_JOINABLE |
					THR_SUSPENDED);
				m_clientsConnectionMutex.acquire();
				m_clientsOnListenPorts[listenPort].push_back(pClient);
				m_clientsConnectionMutex.release();
				pClient->resume();
			}

			/*unsigned char buffer[ 122880 ];

			ssize_t bytes_received;

			while ((bytes_received =
				peer.recv (buffer, sizeof(buffer))) != -1)
			{
				if(bytes_received != 0)
				{
					int cmdType = ParseCmdClass(peer, buffer, bytes_received, m_portsToListen[i].get_port_number());
					int okCmdType = -1;
					if (i < CMD_PORT && 
						(READNW == cmdType || READNB == cmdType || READCOIL == cmdType || READ_HOLD_REGISTERS == cmdType))
					{
						okCmdType = cmdType;
					}
					else if (i >= CMD_PORT && 
						(WRITENW == cmdType || WRITENB == cmdType || WRITE1W == cmdType || WRITE1B == cmdType))
					{
						okCmdType = cmdType;
					}

					switch( cmdType )
					{
					case READNW:
						{
							QueryAndSend(peer, buffer);
							break;
						}
					case WRITENW:
						{
							WriteAndSend(peer, buffer);
							break;
						}
					case WRITENB:
						{
							WriteAndSend(peer, buffer, true);
							break;
						}
					case READNB:
						{
							QueryAndSend(peer, buffer, true);
							break;
						}
					case READCOIL:
						{
							QueryAndSend(peer, buffer, true);
							break;
						}
					case READ_HOLD_REGISTERS:
						{
							QueryAndSend(peer, buffer);
							break;
						}
					case WRITE1W:
						{
							Write1AndSend(peer, buffer);
							break;
						}
					case WRITE1B:
						{
							Write1AndSend(peer, buffer, true);
							break;
						}
					default:
						{
							m_log << "error on parse command class" << endl;
						}
					}
				}
				else
				{
					break;
				}
			}
			peer.close ();*/
		}
	}

	m_clientsConnectionMutex.acquire();
	m_clientsOnListenPorts.erase(listenPort);
	m_clientsConnectionMutex.release();

	for( std::vector<ClientRun*>::iterator it = clientConnections.begin(); 
		it != clientConnections.end(); it++)
	{
		delete *it;
		*it = NULL;
	}
	
	return 0;
}

void RTUSimulator::QueryAndSend( ACE_SOCK_Stream& peer, unsigned char* buffer,
								bool bybit )
{
	unsigned char* pBuffer = buffer;
	if (m_eProtocalType == C830MODBUS)
	{
		//slave id:1Byte, function code: 1Byte
		pBuffer += C830_MODBUS_HEAD_SIZE;
		pBuffer += MODBUS_REQUEST_PDU_START_ADDR_INDEX;
	}
	else if (m_eProtocalType == STANDARDMODBUS)
	{
		//MBPA: 7Byte, function code: 1Byte
		pBuffer += MODBUS_MPAP_HEAD_SIZE;
		pBuffer += MODBUS_REQUEST_PDU_START_ADDR_INDEX;
	}
	unsigned char ah = *( pBuffer );
	unsigned char al = *( pBuffer+1 );
	unsigned char wh = *( pBuffer+2 );
	unsigned char wl = *( pBuffer+3 );

	int Addr  = ( ah<<8 ) | al;
	int Words = ( wh<<8 ) | wl;
	bool Res = Query( peer, Addr, Words, buffer, bybit );

	if( Res )
	{
		if (!bybit)
		{
			if (m_eProtocalType == STANDARDMODBUS)
			{
				//MPAP header(7) + data byte(words*2) + PDU(2)
				Send(peer, buffer, Words * 2 + 2 + MODBUS_MPAP_HEAD_SIZE);
			}
			else if (m_eProtocalType == C830MODBUS)
			{
				//HEADER(1) + data byte(words*2) + PDU(2) + CRC FOOTER(2)
				Send(peer, buffer, 3 + Words * 2 + 2);
			}
		} 
		else
		{
			int intTempA = static_cast<int>(ceil(static_cast<float>(Words)/8));
			if (m_eProtocalType == STANDARDMODBUS)
			{
				//MPAP header(7) + data byte(words*2) + PDU(2)
				Send(peer, buffer, intTempA + 2 + MODBUS_MPAP_HEAD_SIZE);
			}
			else if (m_eProtocalType == C830MODBUS)
			{
				//HEADER(1) + data byte(words*2) + PDU(2) + CRC FOOTER(2)
				Send(peer, buffer, 3 + intTempA + 2);
			}
		}
	}
}

int RTUSimulator::GetPortsNum( void )
{
	return m_portsNum;
}

CMDCLASS RTUSimulator::ParseCmdClass( ACE_SOCK_Stream& peer, unsigned char* buffer, const int length, int port)
{
	if( m_logWnd )
	{
		CWnd* pMainWnd = AfxGetApp()->GetMainWnd();
		if ( ((CButton*)(pMainWnd->GetDlgItem( IDC_PAUSECOMMDISPLAY )))->GetCheck() == 0 )
		{
			std::string text;
			char temp[10];
			text = "on port:";
			text += itoa(port, temp, 10);
			text += "; ";
			text += CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
			text += " Command : ";
			text += "\r\n";
			
			for(int i = 0; i < length; ++i)
			{
				if( (0 == i % 40) && (i != 0) )
				{
					text += "\r\n";
				}
				
				text += ConvertToAssic(buffer[i]);
			}
			
			text += "\r\n";
			text += "\r\n";
			
			CString tmp; 
			m_logWnd->GetWindowText(tmp);
			
			tmp.Insert(0, text.c_str());
			m_logWnd->SetWindowText(tmp);
			
			WriteLogFile(peer, text);
		} 
	}

	char functionCode = FAILED;
	if (m_eProtocalType == STANDARDMODBUS)
	{
		functionCode = buffer[MODBUS_MPAP_HEAD_SIZE + MODBUS_PDU_FUNCTION_INDEX];
	}
	else if (m_eProtocalType == C830MODBUS)
	{
		functionCode = buffer[C830_MODBUS_HEAD_SIZE + MODBUS_PDU_FUNCTION_INDEX];
	}

	switch( functionCode )
	{
	case 4:
		{
			return READNW;
			break;
		}
	case 16:
		{
			return WRITENW;
			break;
		}
	case 2:
		{
			return READNB;
			break;
		}
	case 15:
		{
			return WRITENB;
			break;
		}
	case 1:
		{
			return READCOIL;
			break;
		}
	case 3:
		{
			return READ_HOLD_REGISTERS;
			break;
		}
	case 5:
		{
			return WRITE1B;
			break;
		}
	case 6:
		{
			return WRITE1W;
			break;
		}
	default:
		{
			return FAILED;
		}
	}
}

bool RTUSimulator::Query( ACE_SOCK_Stream& peer,   const int& address, const int& length,
						  unsigned char*   buffer, bool bybit )
{
	int addrtmp		= address;
	int lengthtmp	= length;
	int bstart		= 0;

	SetTimeStamp(0);

	if( !bybit )
	{
		//request out of address boundary
		if(address + length - 1 > m_svrTEnd) 
		{
			SendExp(peer, buffer, NOTALLOWADDR);
			return false;
		}

		if ( STANDARDMODBUS==m_eProtocalType )
		{
			if ( m_checkClientRequestAddress && (( length < 1 )||( length > 0x7D )) )
			{
				SendExp( peer, buffer, NOTALLOWVAL );
				return 0;
			}
			//MBAP header data len field = slave id byte(1) + PDU byte
			int dataLens = 2*length + 3;
			buffer[STRANDARD_MODBUS_MBAP_LEN_H_INDEX] = MSB(dataLens);
			buffer[STRANDARD_MODBUS_MBAP_LEN_L_INDEX] = LSB(dataLens);

			//PDU Byte num field = data byte
			if (m_checkClientRequestAddress)
			{
				buffer[MODBUS_MPAP_HEAD_SIZE + MODBUS_R_RESPONSE_PDU_BTYE_NUM_INDEX] = 2 * length;
			}
			else 
			{
				//no address range limit, so we set this byte 0
				buffer[MODBUS_MPAP_HEAD_SIZE + MODBUS_R_RESPONSE_PDU_BTYE_NUM_INDEX] = 0;
			}

			//PDU data field
			bstart = MODBUS_MPAP_HEAD_SIZE + MODBUS_R_RESPONSE_PDU_DATA_INDEX;
			while(--lengthtmp >= 0)
			{
				buffer[bstart++] = m_vtinternalTable[addrtmp] >> 8;
				buffer[bstart++] = m_vtinternalTable[addrtmp] & 0xff;
				++addrtmp;
			}
		}
		else
		{
			//PDU byte num field = data byte
			//but here we set 0 for C830_MODBUS
			buffer[C830_MODBUS_HEAD_SIZE + MODBUS_R_RESPONSE_PDU_BTYE_NUM_INDEX] = 0;

			//PDU data field
			bstart = C830_MODBUS_HEAD_SIZE + MODBUS_R_RESPONSE_PDU_DATA_INDEX;
			while(--lengthtmp >= 0)
			{
				buffer[bstart++] = m_vtinternalTable[addrtmp] >> 8;
				buffer[bstart++] = m_vtinternalTable[addrtmp] & 0xff;
				++addrtmp;
			}
			//C830 crc footer
			InsCrc(3 + length*2, (unsigned char *)buffer);
		}
	}
	else if( bybit )
	{
		//request out of address boundary
		if( (address/16+length/16-1) > m_svrTEnd )
		{
			SendExp(peer, buffer, NOTALLOWADDR);
			return false;
		}
		
		if ( STANDARDMODBUS==m_eProtocalType )
		{
			if (m_checkClientRequestAddress && ((length<1) || (length>0x7D0)) )
			{
				SendExp( peer, buffer, NOTALLOWVAL );
				return 0;
			}

			int byteNum = static_cast<int>(ceil(static_cast<float>(lengthtmp)/8));

			if (m_checkClientRequestAddress)
			{
				//PDU Byte num field = data byte
				buffer[MODBUS_MPAP_HEAD_SIZE + MODBUS_R_RESPONSE_PDU_BTYE_NUM_INDEX] = byteNum;
			}
			else
			{
				//PDU Byte num field = data byte
				buffer[MODBUS_MPAP_HEAD_SIZE + MODBUS_R_RESPONSE_PDU_BTYE_NUM_INDEX] = 0;
			}
				
			//MBAP header data len field = slave id byte(1) + PDU byte
			int dataLens = byteNum + 3;
			buffer[STRANDARD_MODBUS_MBAP_LEN_H_INDEX] = MSB(dataLens);
			buffer[STRANDARD_MODBUS_MBAP_LEN_L_INDEX] = LSB(dataLens);

			//PDU data field
			bstart = MODBUS_MPAP_HEAD_SIZE + MODBUS_R_RESPONSE_PDU_DATA_INDEX;
		}
		else
		{
			int byteNum = static_cast<int>(ceil(static_cast<float>(lengthtmp)/8));

			//PDU byte num field = data byte
			//but c830 no byte number limit, just set 0
			buffer[C830_MODBUS_HEAD_SIZE + MODBUS_R_RESPONSE_PDU_BTYE_NUM_INDEX] = 0;

			//PDU data field
			bstart = C830_MODBUS_HEAD_SIZE + MODBUS_R_RESPONSE_PDU_DATA_INDEX;
		}

		int intTempA = static_cast<int>(ceil(static_cast<float>(lengthtmp)/8));
		lengthtmp = static_cast<int>(ceil(static_cast<float>(lengthtmp)/16));
		addrtmp = static_cast<int>(floor(static_cast<float>(addrtmp)/16));

		unsigned char* tmpbuf = new unsigned char[(lengthtmp+1) * 2];
	
		int i = -1;
		while( --lengthtmp >= -1 )
		{
			tmpbuf[++i] = m_vtinternalTable[addrtmp] & 0xff;
			tmpbuf[++i] = m_vtinternalTable[addrtmp] >> 8;
			++addrtmp;
		}

		unsigned char* tmpr = GetNBits( tmpbuf, address % 16, length, intTempA+1 );
		delete[] tmpbuf;
		tmpbuf = NULL;

		lengthtmp = ( 0==length%8 ) ? ( length/8 ) : ( length/8+1 );

		for(int j = 0; j < lengthtmp; j++ )
		{
			buffer[j + bstart] = tmpr[j];
		}
		delete[] tmpr;
		tmpr = NULL;

		if ( C830MODBUS == m_eProtocalType )
		{
			InsCrc( 3 + lengthtmp, (unsigned char*)buffer );
		}
	}

	return true;
}

void RTUSimulator::InsCrc( const int& length, unsigned char* value )
{
	unsigned short m_crc = GetCrc(length, value);
	value[length] = m_crc & 0xff;
	value[length + 1] = m_crc >> 8;
}

std::string RTUSimulator::GetAddress()
{
	return std::string( m_portsToListen[0].get_host_addr() );
}

std::string RTUSimulator::GetPollPort()
{
	std::string tmp;
	char buffer[8];

	for(int i = 0; i < CMD_PORT ; ++i)
	{
		tmp += itoa( m_portsToListen[i].get_port_number(), buffer, 10 ); 
		tmp += ", ";
	}

	tmp.erase(tmp.find_last_of(','), 1);

	return tmp;
}

std::string RTUSimulator::GetCmdPort( void )
{
	char buffer[8];
	return std::string( itoa(m_portsToListen[CMD_PORT].get_port_number(), buffer, 10) );
}

void RTUSimulator::SetLogWnd( CWnd* wnd )
{
	m_logWnd = wnd;
}

void RTUSimulator::SetOutputWnd( CWnd* wnd )
{
	m_outputWnd = wnd;
}

std::string RTUSimulator::ConvertToAssic( unsigned char c )
{
	std::string tmp;

	if(c >= 0 && c <= 9)
	{
		tmp = "0";
		tmp += c | 0x30;
		tmp += " ";
	}
	else if(c >= 10 && c <= 15)
	{
		tmp = "0";
		tmp += c - 10 + 'a';
		tmp += " ";
	}
	else
	{
		unsigned char hi = c >> 4;
		unsigned char lo = c & 0xf;

		if(hi >= 0 && hi <= 9)
		{
			tmp += hi | 0x30;
		}
		else
		{
			tmp += hi - 10 + 'a';
		}

		if(lo >= 0 && lo <= 9)
		{
			tmp += lo | 0x30;
		}
		else
		{
			tmp += lo - 10 + 'a';
		}

		tmp += " ";
	}

	return tmp;
}

void RTUSimulator::Send( ACE_SOCK_Stream& peer, unsigned char * buffer, int size )
{
	peer.send_n ( buffer, size );

	if( m_logWnd )
	{
		if (((CButton*)((AfxGetApp()->GetMainWnd())->GetDlgItem( IDC_PAUSECOMMDISPLAY )))->GetCheck() == 0 )
		{
			std::string text;
			text = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
			text += " Reply : \r\n";
			
			for( int i = 0; i < size; ++i )
			{
				if( (0 == i % 40) && (i != 0) )
				{
					text += "\r\n";
				}
				
				text += ConvertToAssic(buffer[i]);
			}
			
			text += "\r\n";
			CString tmp; 
			
			m_logWnd->GetWindowText(tmp);
			
			if( tmp.GetLength() > 122880 )
			{
				tmp.Delete(0, tmp.GetLength());
			}
			
			tmp.Insert(0, text.c_str());
			m_logWnd->SetWindowText(tmp);
			
			WriteLogFile(peer,text);
		}
	}
}

void RTUSimulator::SetDeviceStatus( std::string dname, bool status )
{
	if( m_mapDevices.find(dname) != m_mapDevices.end() )
	{
		SetData( m_mapDevices.find(dname)->second, status ? 0xffff : 0x7fff, BYWORD );
	}
	else
	{
		m_log << "warning: can't find device " << dname << " in the RTU table\n";
	}
}

void RTUSimulator::SendExp( ACE_SOCK_Stream& peer, unsigned char* buffer, EXPTYPE type )
{
//	buffer[0] = (char)0x80;
//	buffer[1] = type;
//	InsCrc(2, (unsigned char*)buffer);
//	Send(peer, buffer, 4);
//	Send(peer, buffer, 2);
	int size = 0;
	if (m_eProtocalType == C830MODBUS)
	{
		// SlaveID(1)-ErrorCode(1)-SubErrorCode(1)-CRC(2)
		buffer[C830_MODBUS_HEAD_SIZE + MODBUS_ERROR_CODE_PDU_INDEX] = MODBUS_ERROR_CODE;
		buffer[C830_MODBUS_HEAD_SIZE + MODBUS_ERROR_SUB_CODE_PDU_INDEX] = type;
		InsCrc(3, (unsigned char*)buffer);
		size = 5;
	}
	else if (m_eProtocalType == STANDARDMODBUS)
	{
		//ErrorCode(1)-SubErrorCode(1)
		//ErrorCode = 0X80 + function code
		buffer[MODBUS_MPAP_HEAD_SIZE + MODBUS_ERROR_CODE_PDU_INDEX] = 
			( MODBUS_ERROR_CODE + buffer[MODBUS_MPAP_HEAD_SIZE + MODBUS_PDU_FUNCTION_INDEX]);
		buffer[MODBUS_MPAP_HEAD_SIZE + MODBUS_ERROR_SUB_CODE_PDU_INDEX] = type;
		//exception code(2) + slave id(1)
		int dataSize = 3;
		buffer[STRANDARD_MODBUS_MBAP_LEN_H_INDEX] =  MSB(dataSize);
		buffer[STRANDARD_MODBUS_MBAP_LEN_L_INDEX] =  LSB(dataSize);
		size = MODBUS_MPAP_HEAD_SIZE + 2;
	}
	
	Send(peer, buffer, size);
}

bool RTUSimulator::SetData( int address, unsigned int data, SETTYPE type, int bit )
{
	// modify this function to delete reaction operator;
	return SetData(address, data, type, bit, true);
}

bool RTUSimulator::SetData( int address,unsigned int data, SETTYPE type, int bit, bool flag )
{
	if( address <= m_svrTEnd )
	{
		SetTimeStamp(address);	// add time stamp;
		switch( type )
		{
		case BYBIT:
			m_vtinternalTable[address] = ( m_vtinternalTable[address] & ~(1 << bit)) | ((data & 0x1) << bit );
			ProcessValueUpdate(address, type, bit);

			if( m_outputWnd && ( ((CButton*)(AfxGetApp()->GetMainWnd())->GetDlgItem( IDC_PAUSEOUTPUTDISPLAY ))->GetCheck() == 0 ) )
			{
				char buffer[10] = {0};
				std::string text;

				text = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
				text += " Output: ";
				text += itoa( address, buffer, 10 );
				text += ".";

				memset( buffer, 0, 10 );

				text += itoa( bit, buffer, 10 );
				text += "=";
				text += "0x";
	
				memset( buffer, 0, 10 );

				text += itoa( data & 0x1, buffer, 16 );
				text += "\r\n";
	
				CString tmp; 
				m_outputWnd->GetWindowText(tmp);
	
				if(tmp.GetLength() > 20000)
				{
					tmp.Delete(0, tmp.GetLength());
				}

				tmp.Insert(0, text.c_str());
				m_outputWnd->SetWindowText(tmp);
			}
	
			return true;
	
		case BYBYTE:
			m_vtinternalTable[address] = ( m_vtinternalTable[address] & 0xff00) | (data & 0xff);
			ProcessValueUpdate(address, type, bit);
		
			if( m_outputWnd && ( ((CButton*)(AfxGetApp()->GetMainWnd())->GetDlgItem( IDC_PAUSEOUTPUTDISPLAY ))->GetCheck() == 0 ))
			{
				char buffer[10] = {0};
				std::string text;
		
				text = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
				text += " Output: ";
				text += itoa( address, buffer, 10 );
				text += "=";
				text += "0x";
		
				memset( buffer, 0, 10 );
			
				text += itoa( m_vtinternalTable[address] & 0x00ff, buffer, 16 );
				text += "\r\n";
			
				CString tmp; 
				m_outputWnd->GetWindowText(tmp);
			
				if(tmp.GetLength() > 20000)
				{
					tmp.Delete(0, tmp.GetLength());
				}
			
				tmp.Insert(0, text.c_str());
				m_outputWnd->SetWindowText(tmp);
			}
		
			return true;
	
		case BYWORD:
			m_vtinternalTable[address] = data & 0xffff;
			ProcessValueUpdate(address, type, bit);
		
			if( m_outputWnd && ( ((CButton*)(AfxGetApp()->GetMainWnd())->GetDlgItem( IDC_PAUSEOUTPUTDISPLAY ))->GetCheck() == 0 ))
			{
				char buffer[10] = {0};
				std::string text;
			
				text = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
				text += " Output: ";
				text += itoa( address, buffer, 10 );
				text += "=";
				text += "0x";
			
				memset( buffer, 0, 10 );
			
				text += itoa( m_vtinternalTable[address], buffer, 16 );
				text += "\r\n";
			
				CString tmp; 
				m_outputWnd->GetWindowText(tmp);
			
				if(tmp.GetLength() > 20000)
				{
					tmp.Delete(0, tmp.GetLength());
				}
			
				tmp.Insert(0, text.c_str());
				m_outputWnd->SetWindowText(tmp);
			}
		
			return true;
	
		case BYDWORD:
			m_vtinternalTable[address] = data & 0xffff;
			m_vtinternalTable[address + 1] = data >> 16;
			ProcessValueUpdate(address, type, bit);
			ProcessValueUpdate(address + 1, type, bit);
		
			if( m_outputWnd && ( ((CButton*)(AfxGetApp()->GetMainWnd())->GetDlgItem( IDC_PAUSEOUTPUTDISPLAY ))->GetCheck() == 0 ))
			{
				char buffer[10] = {0};
				std::string text;
			
				text = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
				text += " Output: ";
				text += itoa( address, buffer, 10 );
				text += "=";
				text += "0x";
			
				memset( buffer, 0, 10 );
			
				text += itoa( m_vtinternalTable[address], buffer, 16 );
				text += " ";
				text += itoa( address+1, buffer, 10 );
				text += "=";
				text += "0x";
			
				memset( buffer, 0, 10 );
			
				text += itoa( m_vtinternalTable[address+1], buffer, 16 );
				text += "\r\n";
			
				CString tmp; 
				m_outputWnd->GetWindowText(tmp);
			
				if(tmp.GetLength() > 20000)
				{
					tmp.Delete(0, tmp.GetLength());
				}
			
				tmp.Insert(0, text.c_str());
				m_outputWnd->SetWindowText(tmp);
			}
		
			return true;
	
		default:
		
			return false;
		}
	}

	return false;
}

void RTUSimulator::WriteAndSend( ACE_SOCK_Stream& peer, unsigned char* buffer, bool bybit )
{
	unsigned char* pBuffer = buffer;
	if (m_eProtocalType == C830MODBUS)
	{
		//slave id:1Byte, function code: 1Byte
		pBuffer += C830_MODBUS_HEAD_SIZE;
		pBuffer += MODBUS_REQUEST_PDU_START_ADDR_INDEX;
	}
	else if (m_eProtocalType == STANDARDMODBUS)
	{
		//MBPA: 7Byte, function code: 1Byte
		pBuffer += MODBUS_MPAP_HEAD_SIZE;
		pBuffer += MODBUS_REQUEST_PDU_START_ADDR_INDEX;
	}
	unsigned char ah = *( pBuffer );
	unsigned char al = *( pBuffer+1 );
	unsigned char wh = *( pBuffer+2 );
	unsigned char wl = *( pBuffer+3 );

	int Addr  = ( ah<<8 ) | al;
	int Words = ( wh<<8 ) | wl;
	unsigned short Res = Write( peer, Addr, Words, buffer, bybit );

	//success to write, send response to client
	if ( 0 != Res )
	{
		int bufferLens = 0;

		if (m_eProtocalType == C830MODBUS)
		{
			int qualityOutputAddr = C830_MODBUS_HEAD_SIZE + MODBUS_W_RESPONSE_PDU_QUANTITY_OUTPUT_INDEX;
			buffer[qualityOutputAddr] = Res >> 8;
			buffer[qualityOutputAddr+1] = Res & 0xff;
			InsCrc(6, (unsigned char*)buffer);
			//HEADER(1) + Response PDU(5) + CRC(2)
			bufferLens = 8;
		}
		else
		{
			int qualityOutputAddr = MODBUS_MPAP_HEAD_SIZE + MODBUS_W_RESPONSE_PDU_QUANTITY_OUTPUT_INDEX;
			buffer[qualityOutputAddr] = Res >> 8;
			buffer[qualityOutputAddr+1] = Res & 0xff;
			
			//slave id(1) + Response PDU(5)
			int dataLens = 5 + 1;
			buffer[STRANDARD_MODBUS_MBAP_LEN_H_INDEX] = MSB(dataLens);
			buffer[STRANDARD_MODBUS_MBAP_LEN_L_INDEX] = LSB(dataLens);

			//HEADER(7) + Response PDU(5)
			bufferLens = MODBUS_MPAP_HEAD_SIZE + 5;
		}
		
		Send(peer, buffer, bufferLens);
	}
}

void RTUSimulator::Write1AndSend( ACE_SOCK_Stream& peer, unsigned char* buffer, bool bybit )
{
	//HEADER-FunctionCode(1B)-OutputAddress(2B)-OutputValue(2B)
	unsigned char* pBuffer = buffer;
	if (m_eProtocalType == C830MODBUS)
	{
		//slave id:1Byte, function code: 1Byte
		pBuffer += C830_MODBUS_HEAD_SIZE;
		pBuffer += MODBUS_REQUEST_PDU_START_ADDR_INDEX;
	}
	else if (m_eProtocalType == STANDARDMODBUS)
	{
		//MBPA: 7Byte, function code: 1Byte
		pBuffer += MODBUS_MPAP_HEAD_SIZE;
		pBuffer += MODBUS_REQUEST_PDU_START_ADDR_INDEX;
	}
	unsigned char ah = *( pBuffer );
	unsigned char al = *( pBuffer+1 );
	unsigned char wh = *( pBuffer+2 );
	unsigned char wl = *( pBuffer+3 );

	int Addr  = ( ah<<8 ) | al;
	int outputValue = ( wh<<8 ) | wl;

	if ( bybit )
	{
		bool bTemp   = false;
		int  addrtmp = static_cast<int>(floor(static_cast<float>(Addr)/16));

		//request out of address boundary
		if(addrtmp > m_svrTEnd)
		{
			SendExp(peer, buffer, NOTALLOWADDR);
			return;
		}
	
		int bit = Addr % 16 + 1;
	
		if ( outputValue == 0xff00 )
		{
			bTemp = SetReactionData( addrtmp, 1, BYBIT, bit );	// use new set.
		}
		else if ( outputValue == 0 )
		{
			bTemp = SetReactionData( addrtmp, 0, BYBIT, bit );	// use new set.
		}
	
		if ( bTemp )
		{
			int dataSize = 0;
			if (m_eProtocalType == STANDARDMODBUS)
			{
				dataSize = MODBUS_MPAP_HEAD_SIZE + 5;
			}
			else if (m_eProtocalType == C830MODBUS)
			{
				InsCrc(6, (unsigned char*)buffer);
				dataSize = C830_MODBUS_HEAD_SIZE + 5 + 2;
			}
			
			Send(peer, buffer, dataSize);
		}
	} 
	else
	{
		//request out of address boundary
		if( Addr - 1 > m_svrTEnd )
		{
			SendExp(peer, buffer, NOTALLOWADDR);
			return;
		}
	
		if( SetReactionData( Addr+1, outputValue, BYWORD, 0 ) )	// use new set.
		{
			int dataSize = 0;
			if (m_eProtocalType == STANDARDMODBUS)
			{
				dataSize = MODBUS_MPAP_HEAD_SIZE + 5;
			}
			else if (m_eProtocalType == C830MODBUS)
			{
				InsCrc(6, (unsigned char*)buffer);
				dataSize = C830_MODBUS_HEAD_SIZE + 5 + 2;
			}

			Send(peer, buffer, dataSize);
		}
	}
}

unsigned short RTUSimulator::Write( ACE_SOCK_Stream& peer,  const int&	   addr,
									const int&		 words, unsigned char* buffer,
									bool			 bybit )
{
    int datastart = 0;
	if (STANDARDMODBUS == m_eProtocalType )
	{
		datastart = MODBUS_MPAP_HEAD_SIZE + MODBUS_W_REQUEST_PDU_DATA_BYTE_NUM_INDEX;
	}
	else
	{
		datastart = C830_MODBUS_HEAD_SIZE + MODBUS_W_REQUEST_PDU_DATA_BYTE_NUM_INDEX;
	}
	unsigned short writewords = 0;

	if( bybit )
	{
		int addrtmp = static_cast<int>(floor(static_cast<float>(addr)/16));

		//request out of address boundary
		if(addrtmp > m_svrTEnd)
		{
			SendExp(peer, buffer, NOTALLOWADDR);
			return 0;
		}
		
		if ( STANDARDMODBUS == m_eProtocalType )
		{
			if ( ( words<1 )||( words>0x7B0 ) )
			{
				SendExp( peer, buffer, NOTALLOWVAL );
				return 0;
			}
		}
		
		int bit = addr % 16;
		unsigned char* tmpbuf = new unsigned char[buffer[datastart]];
		int byteNum = buffer[datastart];
	
		for(int i = 0; i < byteNum; ++ i)
		{
			tmpbuf[i] = buffer[++datastart];
		}
	
		for ( int i = 0; i < words; ++ i )
		{
			//SetData( addrtmp,GetABit(tmpbuf, i, tmplen),BYBIT, bit++);
			SetReactionData(addrtmp, GetABit(tmpbuf, i, byteNum), BYBIT, bit++);	// use new set.
			if(bit == 16)
			{
				bit = 0;
				++addrtmp;	
			}
	
			++writewords;
		}
	
		delete[] tmpbuf;
		tmpbuf = NULL;
	}
	else if( !bybit )
	{
		//request out of address boundary
		if( addr + words - 1 > m_svrTEnd )
		{
			SendExp( peer, buffer, NOTALLOWADDR );
			return 0;
		}

		if ( STANDARDMODBUS==m_eProtocalType )
		{
			if ( (words<1) || (words > 0x7B) )
			{
				SendExp( peer, buffer, NOTALLOWVAL );
				return 0;
			}
			
			int indexByteNum = MODBUS_MPAP_HEAD_SIZE + MODBUS_W_REQUEST_PDU_DATA_BYTE_NUM_INDEX;
			if ( buffer[indexByteNum] != words*2 )
			{
				SendExp( peer, buffer, NOTALLOWVAL );
				return 0;
			}
		}

		for( int i=0; i<words; i++ )
		{
			unsigned char hi = buffer[ ++datastart];
			unsigned char lo = buffer[ ++datastart];
			unsigned short data = (hi << 8) | lo;

			if( SetReactionData(addr + i, data, BYWORD, 0) )	// use new set.
			{
				++writewords;
			}
		}
	}

	return writewords;
}

unsigned short RTUSimulator::GetCrc( const int& length, unsigned char* value )
{
	unsigned short m_crc = 0xffff;

	for(int i = 0; i < length; ++ i)
	{
		m_crc ^= value[i];
		int n = 0;
	
		while(n != 8)
		{
			int carry = m_crc&0x1;
			m_crc >>= 1;
		
			if(1 == carry)
			{
				m_crc^=0xa001;
			}
		
			++ n;
		}
	}

	return m_crc;
}

void RTUSimulator::SetLogToFile( bool flag )
{
	m_blogToFile = flag;
}

void RTUSimulator::WriteLogFile( const ACE_SOCK_Stream& peer, std::string text )
{
	if( m_blogToFile )
	{
		ACE_INET_Addr localaddr;
		peer.get_local_addr(localaddr);
		int index = GetLogFileIndex(localaddr);

		if( index != -1 )
		{
			if( m_pLogFile[index] != NULL )
			{
				m_pLogFile[index]->Write(text.c_str(), text.size());
	
				if( m_pLogFile[index]->GetPosition() > 0xfffff ) //2^20 bytes for 1mb size;
				{
					m_pLogFile[index]->Flush();
					m_pLogFile[index]->Close();
				
					delete m_pLogFile[index];
					m_pLogFile[index] = NULL;
				}
			}
			else
			{
				ACE_INET_Addr addr;
				char port[8];
				peer.get_remote_addr(addr);
//				std::string name = "log\\";
//				std::string name = "..\\";
				std::string name;
		
				name += std::string(addr.get_host_addr());
				name += "p";
				name += std::string(itoa(addr.get_port_number(),port,10));
				name += "-";
			
				peer.get_local_addr(addr);
			
				name += std::string(addr.get_host_addr());
				name += "p";
				name += std::string(itoa(addr.get_port_number(),port,10));
				name += "@";
//				name += CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
				name += "log.txt";
			
				m_pLogFile[index] = new CFile(name.c_str(), CFile::modeCreate|CFile::shareExclusive|CFile::modeWrite);
				m_pLogFile[index]->Write(text.c_str(), text.size());
			}
		}
	}
}

std::string RTUSimulator::GetIdentification( void )
{
	return m_identification;
}

std::string RTUSimulator::GetLocation( void )
{
	return m_location;
}

bool RTUSimulator::GetData( const int& address, unsigned int& data, SETTYPE type, int bit )
{
	if( address <= m_svrTEnd )
	{
		switch( type )
		{
		case BYBIT:
			data = ( m_vtinternalTable[address] >> bit ) & 1;
			return true;
	
		case BYBYTE:
			data = m_vtinternalTable[address] & 0xff;
			return true;
	
		case BYWORD:
			data = m_vtinternalTable[address];
			return true;
	
		case BYDWORD:
			data = m_vtinternalTable[address];
			data |= m_vtinternalTable[address + 1] << 16;
			return true;
	
		default:
			return false;
		}
	}
 
	return false;
}

int RTUSimulator::GetLogFileIndex( const ACE_INET_Addr& addr )
{
	for( int i = 0; i < 6; ++ i )
	{
		if( addr == m_portsToListen[i] )
		{
			return i;
		}
	}

	return -1;
}

void RTUSimulator::SetStatus()
{
	unsigned int address = m_portsToListen[CMD_PORT].get_ip_address();
	unsigned short tmp = ( ( address&0xff0000 )>>16 ) + ( address&0xff );
	//set identification
	SetData( 3, tmp, BYWORD );

	//set RTU location (H)
	const char* l = m_location.c_str();
	tmp = ( *l<<8 ) | ( *(l+1) );
	SetData( 4, tmp, BYWORD );

	//set RTU location (L)
	tmp = ( *(l+2)<<8 ) | ( *(l+3) );
	SetData( 5, tmp, BYWORD );

	//set RTU status
	SetData( 6, RTUISHEALTHY, BYWORD );

	//for all other devices;
	for( int i=13; i<33; ++i )
	{
		SetData( i, 0xffff, BYWORD );
	}
}

std::map< std::string, int > & RTUSimulator::GetDeviceMap()
{
	return m_mapDevices;
}

void RTUSimulator::BgnStress( void )
{
	if( m_stressTaskMgr == NULL )
	{
		m_stressTaskMgr = new StressTaskManager(this);
	}
	std::vector<std::string> vec_cfgs;
	vec_cfgs.clear();
	std::string cfg = GetStressCfgName();
	int sta = 0, end = 0, len = cfg.length();
	while(sta < len)
	{
		if (' ' == cfg[sta])
		{
			++sta;
			end = sta;
		}
		if (',' == cfg[end] || ';' == cfg[end] || end == len)
		{
			std::string f(cfg.substr(sta, end - sta));
			if (f != "")
			{
				vec_cfgs.push_back(f);
			}
			sta = end + 1;
			end = sta;
		}
		++end;
	}

	m_stressTaskMgr->ResetCfg(vec_cfgs);
	vec_cfgs.clear();

	m_stressTaskMgr->StartTasks();
}

void RTUSimulator::ResumeStress()
{
	if (m_stressTaskMgr)
	{
		m_stressTaskMgr->StartTasks();
	}
}

void RTUSimulator::PauseStress()
{
	if (m_stressTaskMgr)
	{
		m_stressTaskMgr->RemoveTasks();
	}
}

bool RTUSimulator::StressCallBack(int address,unsigned int data, int bit)
{
	if( -1 == bit )
	{
		SetData(address, data, BYWORD);
	}
	else
	{
		SetData(address, data, BYBIT, bit);
	}
	return true;
}

const std::string& RTUSimulator::GetCfgName()
{
	return m_cfgFile;
}

bool RTUSimulator::GetPLCOn()
{
	return m_allParameters[PLCON_PARAM] == "1";
}

bool RTUSimulator::GetStressOn()
{
	return m_allParameters[STRESSON_PARAM] == "1";
}

const std::string& RTUSimulator::GetStressCfgName()
{
	return m_allParameters[STRESS_RPARAM];
}

void RTUSimulator::SetStressCfgName( const std::string& FileName )
{
	m_allParameters[STRESS_RPARAM] = FileName;
}

const std::string& RTUSimulator::GetPauseOutputDisplay()
{
	return m_allParameters[PAUSE_OUTPUT_DISPLAY];
}

const std::string& RTUSimulator::GetPauseCommunicationDisplay()
{
	return m_allParameters[PAUSE_COMMUNICATION_DISPLAY];
}

void RTUSimulator::LoadDeviceMap( const std::string& str )
{
	std::string s = str;
	boost::regex expression("^([a-z_A-Z0-9[:space:]]+)=([.a-z_A-Z0-9]+);[^\n]*");
	std::string::const_iterator start, end;

	start = s.begin();
	end = s.end();  
	m_mapDevices.erase( m_mapDevices.begin(), m_mapDevices.end() );

	boost::match_results<std::string::const_iterator> what;
	boost::match_flag_type flags = boost::match_default;

	while(boost::regex_search(start, end, what, expression, flags))   
	{
		std::map<std::string, int>::value_type tmp(std::string(what[1].first, what[1].second),
			atoi(std::string(what[2].first, what[2].second).c_str()));
		m_mapDevices.insert(tmp);

		start = what[0].second;      
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}
	
	m_devicesAddressMap.clear();
	std::map<std::string, int>::iterator mbgn = m_mapDevices.begin();
	while (m_mapDevices.end() != mbgn)
	{
		m_devicesAddressMap[mbgn->second].push_back(mbgn->first);
		++mbgn;
	}
}

unsigned char* RTUSimulator::GetNBits( const unsigned char *buff, const int start, const int length, const int bufflen )
{
	int size = (0 == length % 8) ? (length / 8) : (length / 8 + 1);
	unsigned char *tmpbuff = new unsigned char[size];

	memset(tmpbuff, 0, size);

	int pos = -1;
	int j = 0;

	for( int i=start+1; i<start+1+length; ++i )
	{
		tmpbuff[j] |= GetABit(buff, i, bufflen) << ++ pos; 
	
		if( pos==7 )
		{
			pos = -1;
			j++;
		}
	}

	return (tmpbuff);
}

unsigned char RTUSimulator::GetABit( const unsigned char *buff, const int start, const int buflen )
{
	if(start / 8 < buflen)
	{
		return (buff[start / 8] & static_cast<int>(pow((double)2, start % 8))) >> start % 8;
	}

	return 0xff;
}

unsigned short RTUSimulator::GetInternalTableValue( int n )
{
	return m_vtinternalTable[n];
}

void RTUSimulator::ReloadSaveData()
{
	LoadData( m_allParameters[SAVEDATA_PARAM].c_str() );
//	SetStatus();
	// for test.
//	unsigned char buffer[] = {0x21, 0x0f, 0x00, 0xc8, 0x00, 0x01, 0x01, 0x01, 0x0c, 0x9f};
//	ACE_SOCK_Stream peer;
//	WriteAndSend(peer, buffer, true );
}

void RTUSimulator::SaveData()
{
	std::fstream os(m_allParameters[SAVEDATA_PARAM].c_str(), std::ios::trunc | std::ios::out);

	for( int i=0; i<m_svrTEnd; i++ )
	{
		os << m_vtinternalTable[i] << ",";
	}

	os.close();
}

void RTUSimulator::LoadData( std::string filename )
{
	std::ifstream is( filename.c_str() );
	char c;
	int i = 0;
	int addr = 0;

	if( is )
	{
		char databuf[6];

		while( is.get(c) )
		{
			if( c==',' )
			{
				databuf[i] ='\0';
				unsigned short tmp;
				tmp = (unsigned short)_tcstoul( databuf, 0, 10 );
				m_vtinternalTable[addr++] = tmp;
				i = 0;
				
				if ( addr>=m_svrTEnd )
				{
					break;
				}
			}
			else
			{
				databuf[i] = c;
				i++;
			}
		}
		m_log << "load memory content form " << filename << std::endl;	
	}
	else
	{
		m_log << "can't load memory content form " << filename << std::endl;
	}
}

void RTUSimulator::LoadBlockAddress()
{
	char buffer[10];
	std::string strTempBgn = "DATA_BLOCK_BGN_ID";
	std::string strTempEnd = "DATA_BLOCK_END_ID";
	int dataBlocks = atoi(m_mapExpressions.find("DATA_BLOCK_NUMBER")->second.c_str());
	for( int id = 1; id <= dataBlocks; ++id)
	{
		std::string strBgn = strTempBgn + std::string( itoa(id, buffer, 10) );
		if (m_mapExpressions.end() != m_mapExpressions.find(strBgn))
		{
			std::string strEnd = strTempEnd + std::string( itoa(id, buffer, 10) );

			int bgnAddress = atoi(m_mapExpressions.find(strBgn)->second.c_str());
			int endAddress = atoi(m_mapExpressions.find(strEnd)->second.c_str());
			for (int i = bgnAddress; i <= endAddress; ++i)
			{
				m_blockStartAddress[i] = bgnAddress;
			}
			SetTimeStamp(bgnAddress);
		}
	}
}

void RTUSimulator::SetProtocalType( PROTOCALTYPE protocalType )
{
	m_eProtocalType = protocalType;
}

PROTOCALTYPE RTUSimulator::getProtocalType()
{
	return m_eProtocalType;
}

bool RTUSimulator::SetTimeStamp( int address )
{
	//only STEMODBUS has timestamp
	if(C830MODBUS != m_eProtocalType) return true;
	std::map<unsigned short, unsigned short>::iterator iter = m_blockStartAddress.find(address);
	if (iter == m_blockStartAddress.end())
	{
		m_log << "in SetTimeStamp, address not find in blocks, address =" << address << std::endl;
		return false;
	}
	address = iter->second;

	struct _timeb timebuffer;
	_ftime( &timebuffer );
	unsigned short hi = static_cast<unsigned short>(timebuffer.time >> 16);
	unsigned short lo = static_cast<unsigned short>(timebuffer.time & 0xffff);
	unsigned short mi = static_cast<unsigned short>(timebuffer.millitm);
	m_vtinternalTable[address++] = hi;
	m_vtinternalTable[address++] = lo;
	m_vtinternalTable[address++] = mi;
	
	if( m_outputWnd && ( ((CButton*)(AfxGetApp()->GetMainWnd())->GetDlgItem( IDC_PAUSEOUTPUTDISPLAY ))->GetCheck() == 0 ) )
	{
		char buffer[12] = {0};
		std::string text;
		
		text += "address: ";
		text += itoa( address - 3, buffer, 10 );
		text += ", timebuffer: ";
		text += itoa( timebuffer.time, buffer, 10 );
		text += "s; ";
		
		memset( buffer, 0, 12 );
		
		text += itoa(timebuffer.millitm, buffer, 10 );
		text += "ms\r\n";
		
		CString tmp; 
		m_outputWnd->GetWindowText(tmp);
		
		if(tmp.GetLength() > 20000)
		{
			tmp.Delete(0, tmp.GetLength());
		}
		
		tmp.Insert(0, text.c_str());
		m_outputWnd->SetWindowText(tmp);
	}

	return true;
}

bool RTUSimulator::ParseCmdLineParameters()
{
	int i = 0;
	for (i = 1; i < __argc; ++i)
	{
		std::string arg(__argv[i]);
		unsigned int indx = arg.find("=");
		if (std::string::npos == indx)
		{
			m_log << "cmd line parameters error.";
			continue;
		}
		m_allParameters[arg.substr(0, indx)] = arg.substr(indx + 1, arg.length() - indx - 1);
	}
	return true;
}


int RTUSimulator::ParseWaitTime( std::string&str)
{
	std::string::size_type  first = str.find( "S" );
	std::string::size_type  last  = str.find( " ", first+1 );
	if ( first!=std::string::npos&&last!=std::string::npos )
	{
		int value=atoi(str.substr(first+1, str.length()-first-1).c_str());
		if(value>0)
			return value;
	}
	return 0;
}

void RTUSimulator::ProcessValueUpdate( int address, SETTYPE type /*= BYWORD*/, int bit /*= 0*/ )
{
	if (NULL == m_myDlg)
	{
		return;
	}
	if (6 == address)
	{
		m_myDlg->ProcessPrimaryCheckState();
	}
	std::map<int, std::vector<std::string> >::iterator it = m_devicesAddressMap.find(address);
	if (m_devicesAddressMap.end() != it)
	{
		if (address >= 0 && address < m_svrTEnd)
		{
			// if both bits 14 and 15 are 1 --> ok;
			bool nValue = (1 == (0x0001 & (m_vtinternalTable[address] >> 14 ))) && (1 == (m_vtinternalTable[address] >> 15));
			std::vector<std::string>& vec = it->second;
			std::vector<std::string>::iterator itStr = vec.begin();
			while (vec.end() != itStr)
			{
				m_myDlg->SetDeviceCheckState(itStr->c_str(), nValue);	
				++itStr;
			}
		}
	}
}

void RTUSimulator::registerForUpdate( CPLCSimulatorDlg* reg )
{
	m_myDlg = reg;
}

int RTUSimulator::IsDeviceValid( const std::string& dev)
{
	int res = 0;
	std::map<std::string, int>::iterator it = m_mapDevices.find(dev);
	if (m_mapDevices.end() != it)
	{
		int nTemp = it->second;
		if (nTemp >= 0 && nTemp < m_svrTEnd)
		{
			// if both bits 14 and 15 are 1 --> add to map;
			if ((1 == (0x0001 & (m_vtinternalTable[nTemp] >> 14 ))) && (1 == (m_vtinternalTable[nTemp] >> 15)))
			{
				res = 1;
			}
		}
	}
	return res;
}

bool RTUSimulator::SetReactionData( int address,unsigned int data, SETTYPE type, int bit /*= 0 */ )
{
	if( address > m_svrTEnd || address < 0)
	{
		return false;
	}
	
	char buffer[10];
	std::string strTempA = "D ";
	RegexMap::iterator reactionBlocksIt;
	RegexMap::iterator reactionExpressionsIt;
	std::string::size_type first;
	std::string::size_type last;
	std::string::size_type dotPosition;

	char bufferAI[16];
	std::string strTempAAI = "A ";
	RegexMap::iterator reactionBlocksAIIt;
	RegexMap::iterator reactionExpressionsAIIt;
	std::string::size_type firstAI;
	std::string::size_type lastAI;

	switch( type )
	{
	case BYBIT:
		//////////////////////////////////////////////////////////////////////////
		// do reaction;
		
		sprintf( buffer,"%d.%d %d ", address, bit, data );
		strTempA += buffer;
		reactionBlocksIt = m_mapReactionBlocks.find( strTempA );
		
		if ( reactionBlocksIt!=m_mapReactionBlocks.end() )
		{
			m_mapReactionExpressions.clear();
			ParseReactionBlocks( reactionBlocksIt->second );

			reactionExpressionsIt = m_mapReactionExpressions.begin();

			for ( ; reactionExpressionsIt!=m_mapReactionExpressions.end(); reactionExpressionsIt++ )
			{
				first = reactionExpressionsIt->second.find( " " );
				last = reactionExpressionsIt->second.find( " ", first+1 );

				if ( first != std::string::npos && last != std::string::npos )
				{
					std::string adds = reactionExpressionsIt->second.substr(first+1, last-2);
					std::string val  = reactionExpressionsIt->second.substr(last+1);
					if ( (atoi(adds.c_str()) >= 0 && atoi(adds.c_str()) <= m_svrTEnd) &&
						(atoi(val.c_str()) >=0 && atoi(val.c_str()) <= 1))
					{
						dotPosition = adds.find( "." );
  						//m_log<<dotPosition<<std::endl;

						if ( atoi(adds.substr(dotPosition+1).c_str()) >= 0 && atoi(adds.substr(dotPosition + 1).c_str()) <= 16 )
						{
							int sleepMiliSec = ParseWaitTime( reactionBlocksIt->second );
							if (sleepMiliSec <= 0)
							{
								SetData( atoi(adds.substr(0, dotPosition).c_str()),
								atoi(val.c_str()),
								BYBIT, atoi(adds.substr(dotPosition + 1).c_str()),
								true );
							}
							else
							{
								SReactionNode node;
								node.address = atoi(adds.substr(0, dotPosition).c_str());
								node.data    = atoi(val.c_str());
								node.type    = BYBIT;
								node.bit     = atoi(adds.substr(dotPosition + 1).c_str());
								
								node.milisec = sleepMiliSec;
								sReactionCtl.queueNode(node);
							}
						}
					}
				}
			}
		}
		return true;

	case BYBYTE:
	
		return false;

	case BYWORD:
		//////////////////////////////////////////////////////////////////////////
		// do reaction;

		if (data >  65535)
		{
			return false;
		}
		
		sprintf( bufferAI,"%d %d ", address, data );
		strTempAAI += bufferAI;

		reactionBlocksAIIt = m_mapReactionBlocksAI.find( strTempAAI );
		if (m_mapReactionBlocksAI.end() == reactionBlocksAIIt)
		{
			sprintf( bufferAI,"%d ", address);
			strTempAAI = "A ";
			strTempAAI = strTempAAI + bufferAI;
			strTempAAI = strTempAAI + "<value> ";
			reactionBlocksAIIt = m_mapReactionBlocksAI.find( strTempAAI );
		}
		
		if ( reactionBlocksAIIt!=m_mapReactionBlocksAI.end() )
		{
			m_mapReactionExpressionsAI.clear();
			ParseReactionBlocksAI( reactionBlocksAIIt->second );
			
			reactionExpressionsAIIt = m_mapReactionExpressionsAI.begin();
			
			for ( ; reactionExpressionsAIIt!=m_mapReactionExpressionsAI.end(); reactionExpressionsAIIt++ )
			{
				firstAI = reactionExpressionsAIIt->second.find( " " );
				lastAI = reactionExpressionsAIIt->second.find( " ", firstAI+1 );
				
				if ( firstAI != std::string::npos && lastAI != std::string::npos )
				{
					std::string adds = reactionExpressionsAIIt->second.substr( firstAI+1, lastAI-firstAI );
					std::string val  = reactionExpressionsAIIt->second.substr(lastAI+1);
					if (0 == val.find("<value>"))
					{
						sprintf(bufferAI,"%d", data );
						val = bufferAI;
					}

					if ( (atoi(adds.c_str()) >=0 && atoi(adds.c_str()) <= m_svrTEnd) &&
						(atoi(val.c_str()) >= -65535 && atoi(val.c_str()) <= 65535))
					{
						int sleepMiliSec = ParseWaitTime( reactionBlocksAIIt->second );
						if (sleepMiliSec <= 0)
						{
							SetData( atoi(adds.c_str()),
 							atoi(val.c_str()),
 							BYWORD, 0, true );
						}
						else
						{
							SReactionNode node;
							node.address = atoi(adds.c_str());
							node.data    = atoi(val.c_str());
							node.type    = BYWORD;
							node.bit     = 0;
								
							node.milisec = sleepMiliSec;
							sReactionCtl.queueNode(node);	
						}
					}
				}
			}
		}
		return true;

	case BYDWORD:
	
		return false;

	default:
	
		return false;
	}
	return false;
}
void RTUSimulator::writeToLogLn( std::string& logStr )
{
	m_log << logStr.c_str() << std::endl;
}

void RTUSimulator::resumeClientsOnListenPort( int index )
{
	m_clientsConnectionMutex.acquire();
	int listenPort = m_portsToListen[index].get_port_number();
	std::vector<ClientRun*> & clients = m_clientsOnListenPorts[listenPort];
	std::vector<ClientRun*>::iterator iter;
	for (iter = clients.begin(); iter != clients.end(); ++iter)
	{
		(*iter)->resume();
	}
	m_clientsConnectionMutex.release();
}

void RTUSimulator::stopClientsOnListenPort( int index )
{
	m_clientsConnectionMutex.acquire();
	int listenPort = m_portsToListen[index].get_port_number();
	std::vector<ClientRun*> & clients = m_clientsOnListenPorts[listenPort];
	std::vector<ClientRun*>::iterator iter;
	for (iter = clients.begin(); iter != clients.end(); ++iter)
	{
		(*iter)->suspend();
	}
	m_clientsConnectionMutex.release();
}

void RTUSimulator::setCheckClientRequestAddr( bool isCheck )
{
	m_checkClientRequestAddress = isCheck;
}