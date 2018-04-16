// RTUSimulator.cpp: implementation for the RTUSimulator class.
//
////////////////////////////////////////////////////////

#include "RTUSimulator.h"
#include <sstream>
#include <iomanip>
#define MAXHOSTNAMELEN (256)
using namespace TA_IRS_App::PA_Sim;
char* RTUSimulator::m_configFileName = "config.txt";
RTUSimulator* RTUSimulator::m_instance = NULL;

RTUSimulator::RTUSimulator( const std::string & CfgFileName ) : 
m_eProtocalType( STANDARDMODBUS )
,m_portsNum(0)
{
    ACE::init();
}

RTUSimulator::~RTUSimulator( void )
{
    ACE::fini();
}

int RTUSimulator::InitSimulator( const std::string& FileName )
{
	return 0;
}

void RTUSimulator::InitReaction()
{
}

void RTUSimulator::ParseBlocks( std::string& str )
{
}

void RTUSimulator::ParseReactionBlocks( std::string& str )
{
}

void RTUSimulator::ParseReactionBlocksAI( std::string& str )
{

}

int RTUSimulator::StartSvr( const int i )
{
	ACE_SOCK_Acceptor acceptor;

	if ( acceptor.open(m_portsToListen[i], 1) == -1 )
	{
		ACE_ERROR_RETURN ((LM_ERROR,
			ACE_TEXT ("%p\n"),
			ACE_TEXT ("acceptor.open")),
			100);	
	}

	while (1)
	{
		ACE_SOCK_Stream peer;
		int m_tcpNoDelay = 0;
		peer.set_option( IPPROTO_TCP, TCP_NODELAY, &m_tcpNoDelay, sizeof( m_tcpNoDelay ) );

		ACE_INET_Addr peer_addr;
		ACE_Time_Value timeout;
		timeout.msec(10000);
		timeout.usec(0);

		if (acceptor.accept (peer, &peer_addr, NULL, 0) == -1)
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
		}
		else
		{
            std::cout<<"New connection ..." <<std::endl;
			//ACE_TCHAR peer_name[MAXHOSTNAMELEN];
			//peer_addr.addr_to_string (peer_name, MAXHOSTNAMELEN);

			unsigned char buffer[ 122880 ];

			size_t bytes_received;

            ssize_t off_set = 0;
            
            while(1)
            {
                ACE_Time_Value read_timeout;
				read_timeout.msec(0);
				read_timeout.usec(0);
                peer.recv_n(buffer, 6, NULL, &bytes_received);// read header
                if (6 == bytes_received)
                {
                    ssize_t len = buffer[4] * 16 * 16 + buffer[5];
                    peer.recv_n(buffer + 6, len, NULL, &bytes_received); // read body
                    if (bytes_received == len)
					{                                  
                        //std::cout << "Receive data:" << std::endl;
                        //for (ssize_t i = 0; i < (len + 6); ++i )
                        //{
                        //    printf("%02x ", buffer[i]);
                        //}
                        //std::cout<< std::endl;
                        processComingData( buffer, len + 6, &peer);
                    }
                    else
                    {
                        std::cout<<"Read error ..." <<std::endl;
                        break;
                    }
                }
                else
                {
                    std::cout<<"Read error ..." <<std::endl;
                    break;
                }
            }
            peer.close ();
            m_queryProcessor->resetModbusQuery();
		}
	}
	return 0;
}

void RTUSimulator::QueryAndSend( ACE_SOCK_Stream& peer, unsigned char* buffer,
								bool bybit )
{
    
    unsigned char buffer2[11] = {0x00, 0x06, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x02, 0x00, 0x00};
    buffer2[0] = buffer[0];
    buffer2[1] = buffer[1];
    std::cout<< "Send data:" <<std::endl;
    for (ssize_t i = 0; i < 11; ++i )
    {
        printf("%02x ", buffer2[i]);
    }
    std::cout<< std::endl;
    Send(peer, buffer2, 11);
}

int RTUSimulator::GetPortsNum( void )
{
	return m_portsNum;
}

CMDCLASS RTUSimulator::ParseCmdClass( ACE_SOCK_Stream& peer, unsigned char* buffer, const int length )
{
    return FAILED;
}

bool RTUSimulator::Query( ACE_SOCK_Stream& peer,   const int& address, const int& length,
						  unsigned char*   buffer, bool bybit )
{
	int addrtmp		= address;
	int lengthtmp	= length;
	int bstart		= 0;

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
			if ( ( length<1 )||( length>0x7d ) )
			{
				SendExp( peer, buffer, NOTALLOWVAL );
				return 0;
			}
			
			buffer[2] = length*2;
		}
		else
		{
			buffer[2] = 0;
		}

		bstart = 2;

		while(-- lengthtmp >= 0)
		{
			if ( STANDARDMODBUS==m_eProtocalType )
			{
				buffer[++ bstart] = m_vtinternalTable[addrtmp + 1] >> 8;
				buffer[++ bstart] = m_vtinternalTable[addrtmp + 1] & 0xff;
			}
			else
			{
				buffer[++ bstart] = m_vtinternalTable[addrtmp] >> 8;
				buffer[++ bstart] = m_vtinternalTable[addrtmp] & 0xff;
			}

			++ addrtmp;
		}
	
		InsCrc(3 + length*2, (unsigned char *)buffer);
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
			if ( (length<1) || (length>0x7f8) )
			{
				SendExp( peer, buffer, NOTALLOWVAL );
				return 0;
			}
			
			buffer[2] = static_cast<int>(ceil(static_cast<float>(lengthtmp)/8));
		}
		else
		{
			buffer[2] = 0;
		}

		int intTempA = static_cast<int>(ceil(static_cast<float>(lengthtmp)/8));
		lengthtmp = static_cast<int>(ceil(static_cast<float>(lengthtmp)/16));
		addrtmp = static_cast<int>(floor(static_cast<float>(addrtmp)/16));

		unsigned char* tmpbuf = new unsigned char[(lengthtmp+1) * 2];
		bstart = -1;

		while( --lengthtmp>=-1 )
		{
			tmpbuf[++ bstart] = m_vtinternalTable[addrtmp] & 0xff;
			tmpbuf[++ bstart] = m_vtinternalTable[addrtmp] >> 8;
			++ addrtmp;
		}

		unsigned char* tmpr = GetNBits( tmpbuf, address % 16, length, intTempA+1 );
		delete[] tmpbuf;
		tmpbuf = NULL;
	
		lengthtmp = ( 0==length%8 ) ? ( length/8 ) : ( length/8+1 );

		for( bstart=3; bstart<3+lengthtmp; bstart++ )
		{
			buffer[bstart] = tmpr[bstart-3];
		}

		delete[] tmpr;
		tmpr = NULL;
	
		InsCrc( 3+lengthtmp, (unsigned char*)buffer );
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

	for(int i = 0; i < m_portsNum; ++i)
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
    if (0x10 == buffer[7])
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send write data..");
    }      
	peer.send_n ( buffer, size );
}

void RTUSimulator::SetDeviceStatus( std::string dname, bool status )
{
}

void RTUSimulator::SendExp( ACE_SOCK_Stream& peer, unsigned char* buffer, EXPTYPE type )
{
	buffer[1] = (char)0x80;
	buffer[2] = type;

	InsCrc(3, (unsigned char*)buffer);
	Send(peer, buffer, 5);
}

bool RTUSimulator::SetData( int address, unsigned int data, SETTYPE type, int bit )
{
	return false;
}

void RTUSimulator::WriteAndSend( ACE_SOCK_Stream& peer, unsigned char* buffer, bool bybit )
{
	unsigned char ah = *( buffer+2 );
	unsigned char al = *( buffer+3 );
	unsigned char wh = *( buffer+4 );
	unsigned char wl = *( buffer+5 );

	int Addr  = ( ah<<8 ) | al;
	int Words = ( wh<<8 ) | wl;
	unsigned short Res = Write( peer, Addr, Words, buffer, bybit );

	if ( 0 != Res )
	{
		buffer[4] = Res >> 8;
		buffer[5] = Res & 0xff;

		InsCrc(6, (unsigned char*)buffer);
		Send(peer, buffer, 8);
//		Send(peer, buffer, 6);
	}
}

void RTUSimulator::Write1AndSend( ACE_SOCK_Stream& peer, unsigned char* buffer, bool bybit )
{
	unsigned char ah = *( buffer+2 );
	unsigned char al = *( buffer+3 );
	unsigned char wh = *( buffer+4 );
	unsigned char wl = *( buffer+5 );

	int Addr  = ( ah<<8 ) | al;
	int Words = ( wh<<8 ) | wl;

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
	
		if ( Words==0xff00 )
		{
			bTemp = SetData( addrtmp, 1, BYBIT, bit );
		}
		else if ( Words==0 )
		{
			bTemp = SetData( addrtmp, 0, BYBIT, bit );
		}
	
		if ( bTemp )
		{
			InsCrc(6, (unsigned char*)buffer);
			Send(peer, buffer, 8);
//			Send(peer, buffer, 6);
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
	
		if( SetData( Addr+1, Words, BYWORD, 0 ) )
		{
			InsCrc( 6, (unsigned char*)buffer );
			Send(peer, buffer, 8);
//			Send(peer, buffer, 6);
		}
	}
}

unsigned short RTUSimulator::Write( ACE_SOCK_Stream& peer,  const int&	   addr,
									const int&		 words, unsigned char* buffer,
									bool			 bybit )
{
    int datastart = 6;
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
		
		if ( STANDARDMODBUS==m_eProtocalType )
		{
			if ( ( words<1 )||( words>0x7f8 ) )
			{
				SendExp( peer, buffer, NOTALLOWVAL );
				return 0;
			}
		}
		
		int bit = addr % 16;
		unsigned char* tmpbuf = new unsigned char[buffer[datastart]];
		int tmplen = buffer[datastart];
	
		for(int i = 0; i < tmplen; ++ i)
		{
			tmpbuf[i] = buffer[++datastart];
		}
	
		for (int i = 0; i < words; ++ i )
		{
			SetData( addrtmp,GetABit(tmpbuf, i, tmplen),BYBIT,++bit );
	
			if(bit == 15)
			{
				bit = -1;
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
			if ( (words<1) || (words>125) )
			{
				SendExp( peer, buffer, NOTALLOWVAL );
				return 0;
			}
			
			if ( buffer[6] != words*2 )
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

			if( SetData(addr+i+1, data, BYWORD, 0) )
			{
				++ writewords;
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
/*	m_blogToFile = flag;*/
}

void RTUSimulator::WriteLogFile( const ACE_SOCK_Stream& peer, std::string text )
{
// 	if( m_blogToFile )
// 	{
// 		ACE_INET_Addr localaddr;
// 		peer.get_local_addr(localaddr);
// 		int index = GetLogFileIndex(localaddr);
// 
// 		if( index != -1 )
// 		{
// 			if( m_pLogFile[index] != NULL )
// 			{
// 				m_pLogFile[index]->Write(text.c_str(), text.size());
// 	
// 				if( m_pLogFile[index]->GetPosition() > 0xfffff ) //2^20 bytes for 1mb size;
// 				{
// 					m_pLogFile[index]->Flush();
// 					m_pLogFile[index]->Close();
// 				
// 					delete m_pLogFile[index];
// 					m_pLogFile[index] = NULL;
// 				}
// 			}
// 			else
// 			{
// 				ACE_INET_Addr addr;
// 				char port[8];
// 				peer.get_remote_addr(addr);
// //				std::string name = "log\\";
// //				std::string name = "..\\";
// 				std::string name;
// 		
// 				name += std::string(addr.get_host_addr());
// 				name += "p";
// 				name += std::string(itoa(addr.get_port_number(),port,10));
// 				name += "-";
// 			
// 				peer.get_local_addr(addr);
// 			
// 				name += std::string(addr.get_host_addr());
// 				name += "p";
// 				name += std::string(itoa(addr.get_port_number(),port,10));
// 				name += "@";
// //				name += CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
// 				name += "log.txt";
// 			
// 				m_pLogFile[index] = new CFile(name.c_str(), CFile::modeCreate|CFile::shareExclusive|CFile::modeWrite);
// 				m_pLogFile[index]->Write(text.c_str(), text.size());
// 			}
// 		}
// 	}
}

std::string RTUSimulator::GetIdentification( void )
{
	return m_identification;
}

std::string RTUSimulator::GetLocation( void )
{
/*	return m_location;*/
    return "";
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
	//for( int i = 0; i < 6; ++ i )
	//{
	//	if( addr == m_portsToListen[i] )
	//	{
	//		return i;
	//	}
	//}

	return -1;
}

std::vector< std::string > & RTUSimulator::GetValidDevice()
{
	return m_vtvalidDeviceName;
}

void RTUSimulator::SetStatus()
{
// 	unsigned int address = m_portsToListen[CMD_PORT].get_ip_address();
// 	unsigned short tmp = ( ( address&0xff0000 )>>16 ) + ( address&0xff );
// 	//set identification
// 	SetData( 3, tmp, BYWORD );
// 
// 	//set RTU location (H)
// 	const char* l = m_location.c_str();
// 	tmp = ( *l<<8 ) | ( *(l+1) );
// 	SetData( 4, tmp, BYWORD );
// 
// 	//set RTU location (L)
// 	tmp = ( *(l+2)<<8 ) | ( *(l+3) );
// 	SetData( 5, tmp, BYWORD );
// 
// 	//set RTU status
// 	SetData( 6, RTUISHEALTHY, BYWORD );
// 
// 	//for all other devices;
// 	for( int i=13; i<33; ++i )
// 	{
// 		SetData( i, 0xffff, BYWORD );
// 	}
}

std::map< std::string, int > & RTUSimulator::GetDeviceMap()
{
	return m_mapDevices;
}

void RTUSimulator::GetStressConfig()
{
//	if(m_hstress != NULL)
//	{
//		TerminateThread(m_hstress, 0);
//		m_hstress = NULL;
//	}
// 	std::string file = "StressConf.txt";
// 	std::string	pattern	="^([Xx0-9a-fA-F.]+)[[:space:]]*=[[:space:]]*([Xx0-9a-fA-F]*)|^sleep[[:space:](]*([0-9]+)[[:space:])]*";
// 
// 	int varpos = 1;
// 	int valpos = 2;
// 	int sleeppos = 3;
// 
// 	std::ifstream is(file.c_str());
// 	std::string s;
// 
// 	FileLoader()(s, is);
// 
// 	boost::regex expression( pattern.c_str() );
// 	std::string::const_iterator start, end;
// 
// 	start = s.begin();
// 	end = s.end();  
// 	m_mulmapStress.erase( m_mulmapStress.begin(), m_mulmapStress.end() );
// 
// 	boost::match_results<std::string::const_iterator> what;
// 	boost::match_flag_type flags = boost::match_default;
// 
// 	while(boost::regex_search(start, end, what, expression, flags))   
// 	{
// 		if(what[sleeppos].first != what[sleeppos].second)
// 		{
// 			m_mulmapStress.insert(smap_value("-1",std::string(what[sleeppos].first, what[sleeppos].second)));
// 		}
// 		else
// 		{
// 			m_mulmapStress.insert(smap_value(std::string(what[varpos].first, what[varpos].second),
// 				std::string(what[valpos].first, what[valpos].second)));
// 		}
// 
// 		start = what[0].second;      
// 		flags |= boost::match_prev_avail;
// 		flags |= boost::match_not_bob;
// 	}
}

DWORD WINAPI RTUSimulator::Stress( LPVOID ptr )
{
	((RTUSimulator*)ptr)->StressSet();

	return 0;
}

void RTUSimulator::BgnStress( void )
{
	if( m_hstress != NULL )
	{
		TerminateThread(m_hstress, 0);
		m_hstress = NULL;
	}
//	GetStressConfig();

	if( m_hstress == NULL )
	{
		m_hstress = CreateThread(NULL, 0, Stress, this, CREATE_SUSPENDED, NULL);
	}
 
	ResumeStress();
}

void RTUSimulator::ResumeStress()
{
	ResumeThread( m_hstress );
}

void RTUSimulator::PauseStress()
{
	SuspendThread( m_hstress );
}

void RTUSimulator::StressSet()
{
// 	while( 1 )
// 	{
// 		GetStressConfig();
// 	 
// 		smap_type::iterator bgn = m_mulmapStress.begin() ;
// 		smap_type::iterator end = m_mulmapStress.end();
// 	
// 		while( 1 )
// 		{
// 			int bit = -1;
// 			int addr = 0;
// 			int val = 0;
// 		
// 			std::string addrstr = bgn->first;
// 			std::string valstr = bgn->second;
// 			std::string bitstr("");
// 		
// 			if( addrstr.find('.') != std::string::npos )
// 			{
// 				bitstr = std::string(addrstr.begin() + addrstr.find('.') + 1, addrstr.end());
// 				addrstr.erase(addrstr.find('.'));
// 			
// 				if((bitstr.find("0X") != std::string::npos) || (bitstr.find("0x") != std::string::npos))
// 				{
// 					bitstr.erase(0,2);
// 					bit = _tcstoul(bitstr.c_str(), 0, 16);
// 				}
// 				else
// 				{
// 					bit = _tcstoul(bitstr.c_str(), 0, 10);
// 				}
// 			}
// 			
// 			if( (addrstr.find("0X") != std::string::npos) || (addrstr.find("0x") != std::string::npos) )
// 			{
// 				addrstr.erase(0,2);
// 				addr = _tcstoul(addrstr.c_str(), 0, 16);
// 			}
// 			else
// 			{
// 				addr = _tcstoul(addrstr.c_str(), 0, 10);
// 			}
// 	
// 			if( (valstr.find("0X") != std::string::npos) || (valstr.find("0x") != std::string::npos) )
// 			{
// 				valstr.erase(0,2);
// 				val = _tcstoul(valstr.c_str(), 0, 16);
// 			}
// 			else
// 			{
// 				val = _tcstoul(valstr.c_str(), 0, 10);
// 			}
// 	
// 			if( -1 == addr )
// 			{
// 				Sleep(val);
// 			}
// 			else
// 			{
// 				if( -1 == bit )
// 				{
// 					SetData(addr, val, BYWORD);
// 				}
// 				else
// 				{
// 					SetData(addr, val, BYBIT, bit);
// 				}
// 			}
// 			//		if(++bgn == end)
// 			//		{
// 			//			bgn = m_mulmapStress.begin();
// 			//		}
// 			if( ++bgn == end )
// 			{
// 				break;
// 			}
// 		}
// 	}
}

const std::string& RTUSimulator::GetCfgName()
{
	return m_cfgFile;
}

void RTUSimulator::LoadDeviceMap( const std::string& str )
{
// 	std::string s = str;
// 	boost::regex expression("^([a-z_A-Z0-9[:space:]]+)=([.a-z_A-Z0-9]+);[^\n]*");
// 	std::string::const_iterator start, end;
// 
// 	start = s.begin();
// 	end = s.end();  
// 	m_mapDevices.erase( m_mapDevices.begin(), m_mapDevices.end() );
// 
// 	boost::match_results<std::string::const_iterator> what;
// 	boost::match_flag_type flags = boost::match_default;
// 
// 	while(boost::regex_search(start, end, what, expression, flags))   
// 	{
// 		std::map<std::string, int>::value_type tmp(std::string(what[1].first, what[1].second),
// 			atoi(std::string(what[2].first, what[2].second).c_str()));
// 		m_mapDevices.insert(tmp);
// 
// 		start = what[0].second;      
// 		flags |= boost::match_prev_avail;
// 		flags |= boost::match_not_bob;
// 	}
}

unsigned char* RTUSimulator::GetNBits( const unsigned char *buff, const int start, const int length, const int bufflen )
{
	int size = (0 == length % 8) ? (length / 8) : (length / 8 + 1);
	unsigned char *tmpbuff = new unsigned char[size];

	memset(tmpbuff, 0, size);

// 	int pos = -1;
// 	int j = 0;
// 
// 	for( int i=start+1; i<start+1+length; ++i )
// 	{
// 		tmpbuff[j] |= GetABit(buff, i, bufflen) << ++ pos; 
// 	
// 		if( pos==7 )
// 		{
// 			pos = -1;
// 			j++;
// 		}
// 	}

	return (tmpbuff);
}

unsigned char RTUSimulator::GetABit( const unsigned char *buff, const int start, const int buflen )
{
// 	if(start / 8 < buflen)
// 	{
// 		return (buff[start / 8] & static_cast<int>(pow(2, start % 8))) >> start % 8;
// 	}

	return 0xff;
}

unsigned short RTUSimulator::GetInternalTableValue( int n )
{
	return m_vtinternalTable[n];
}

void RTUSimulator::ResetAllData()
{
	LoadData( "SaveData.txt" );
	SetStatus();
}

void RTUSimulator::SaveData()
{
// 	std::fstream os( "SaveData.txt", std::ios::trunc | std::ios::out);
// 
// 	for( int i=0; i<m_svrTEnd; i++ )
// 	{
// 		os << m_vtinternalTable[i] << ",";
// 	}
// 
// 	os.close();
}

void RTUSimulator::LoadData( std::string filename )
{
// 	std::ifstream is( filename.c_str() );
// 	char c;
// 	int i = 0;
// 	int addr = 0;
// 
// 	if( is )
// 	{
// 		char databuf[6];
// 
// 		while( is.get(c) )
// 		{
// 			if( c==',' )
// 			{
// 				databuf[i] ='\0';
// 				unsigned short tmp;
// 				tmp = (unsigned short)_tcstoul( databuf, 0, 10 );
// 				m_vtinternalTable[addr++] = tmp;
// 				i = 0;
// 				
// 				if ( addr>=m_svrTEnd )
// 				{
// 					break;
// 				}
// 			}
// 			else
// 			{
// 				databuf[i] = c;
// 				i++;
// 			}
// 		}
// 		m_log << "load memory content form " << filename << std::endl;	
// 	}
// 	else
// 	{
// 		m_log << "can't load memory content form " << filename << std::endl;
// 	}
}

void RTUSimulator::SetProtocalType( PROTOCALTYPE protocalType )
{
	m_eProtocalType = protocalType;
}

void RTUSimulator::addListenAddress( int port, std::string ipAddress )
{
    m_portsToListen[m_portsNum].set(port, ipAddress.c_str());
    ++m_portsNum;
}

void RTUSimulator::processComingData( UINT8* buf, ssize_t dataSize, ACE_SOCK_Stream * sockStream )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Data receive");
    
	std::stringstream outputSS;
	std::vector<UINT8> pData;

	outputSS << "Receive data:" << std::endl;

    for (ssize_t i = 0; i < dataSize; ++i )
    {
        pData.push_back(buf[i]);
		outputSS << std::hex << std::setw(2) << std::setfill('0')<< buf[i] << " ";
    }
	outputSS << std::endl;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"%s", outputSS.str());

    bool keep_processing = true;
    std::vector<unsigned char>::iterator iter = pData.begin();


    Byte functionCode = iter[7];
    if ( ( functionCode == PASim_FC_Read ) || ( functionCode == PASim_FC_Write ) )
    {   
        ModbusQuery * query = 0;
        
        if ( ( PASim_FC_Read == functionCode ) && ( pData.size() >= 10 ) ) // read query's length at least 10 bytes
        {
            query = processReceivedRead(iter);
        }
        else if ( (PASim_FC_Write == functionCode) && (pData.size() > 13)) // write query's length at least 13 bytes
        {
            query = processReceivedWrite(iter);
        }
		else
			return;

        query->setSocket(sockStream);
		// Add the query to the processor
        m_queryProcessor->addModbusQuery(query);
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "bad data");
    }
}

void RTUSimulator::setLocation( LocationType loc )
{
    m_location = loc;
}

void RTUSimulator::setQueryProcessor( TA_IRS_App::PA_Sim::PASimTableQueryProcessor* processor )
{
    TA_ASSERT(processor != NULL,"");
    m_queryProcessor = processor;
}

TA_IRS_App::PA_Sim::ModbusQuery * RTUSimulator::processReceivedRead( std::vector<unsigned char>::iterator & iter )
{
    TA_IRS_App::PA_Sim::ModbusReadQuery * r = new ModbusReadQuery;
    r->fromNetwork(iter);
    
    return r;
}

TA_IRS_App::PA_Sim::ModbusQuery * RTUSimulator::processReceivedWrite( std::vector<unsigned char>::iterator & iter )
{
    TA_IRS_App::PA_Sim::ModbusWriteQuery * w = new ModbusWriteQuery;
    w->fromNetwork(iter);
    
    return w;
}

void RTUSimulator::isMsgComplete( unsigned char* buf, ssize_t size, ssize_t* offset )
{
    
}

