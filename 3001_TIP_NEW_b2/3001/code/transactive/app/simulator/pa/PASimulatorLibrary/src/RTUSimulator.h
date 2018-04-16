// RTUSimulator.h: interface for the RTUSimulator class.
//
////////////////////////////////////////////////////////

#pragma once

#include <MATH.H>
#include <functional>
#include <map>
#include <string>
#include <VECTOR>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Log_Msg.h"
#include "ace/Task.h"
#include "ace/ACE.h"

#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommsObserver.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableQueryProcessor.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimModbusFrames.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommon.h"

typedef std::pair<int, int> index;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
enum CMDCLASS { READNB, READNW, WRITENB, WRITENW,READCOIL,READ_HOLD_REGISTERS,FAILED, WRITE1B, WRITE1W};
enum EXPTYPE {NOTALLOWACT = 1, NOTALLOWADDR, NOTALLOWVAL};
enum SETTYPE {BYBIT, BYBYTE, BYWORD, BYDWORD};
enum PROTOCALTYPE {C830MODBUS, STANDARDMODBUS};


template<class T>
struct dummy : public std::binary_function<T, T, bool> 
{
    bool operator()(const T& x, const T& y) const
    {
        return false;
    }
};

typedef std::multimap<std::string, std::string, dummy<std::string> > smap_type;
typedef smap_type::value_type smap_value;

typedef std::map< std::string, std::string > RegexMap;
typedef std::map< std::string, int > DeviceMap;

const unsigned int RTUISHEALTHY = 0x393;
const unsigned short CMD_PORT = 5;

class RTUSimulator
{
public:
	static RTUSimulator & getInstance()
	{
		if ( m_instance==NULL )
		{
			m_instance = new RTUSimulator( m_configFileName );
		}
		return * m_instance;
	}

	~RTUSimulator( void );

	void SetProtocalType( PROTOCALTYPE protocalType );
/*	void SetLogWnd( CWnd* );*/
/*	void SetOutputWnd( CWnd* );*/
	void Send( ACE_SOCK_Stream& peer, unsigned char* buffer, int size );
	void SendExp( ACE_SOCK_Stream& peer, unsigned char* buffer, EXPTYPE type );
	void WriteAndSend( ACE_SOCK_Stream& peer, unsigned char* buffer, bool bybit = false );
	void Write1AndSend( ACE_SOCK_Stream& peer, unsigned char* buffer, bool bybit = false );
	void SetLogToFile( bool flag );
	void WriteLogFile( const ACE_SOCK_Stream& peer, std::string text );
	void InsCrc( const int& length, unsigned char* );
	void StressSet();
	void PauseStress();
	void ResumeStress();
	void GetStressConfig();
	void BgnStress( void );
	void SetDeviceStatus( std::string dname, bool status );
	void SetStatus();
	void ResetAllData();
	void SaveData();
	void LoadData( std::string FileName );
	void LoadDeviceMap( const std::string& );
	int InitSimulator( const std::string& FileName );
	void InitReaction();

	std::string ConvertToAssic( unsigned char c );
	std::string GetIdentification( void );
	std::string GetLocation( void );
	std::string GetAddress();
	std::string GetPollPort();
	std::string GetCmdPort( void );
	const std::string& GetCfgName();

	int StartSvr( const int i );
	int GetPortsNum( void );

	bool SetData( int address,unsigned int data, SETTYPE type, int bit, bool flag );
	bool SetData( int address,unsigned int data, SETTYPE type, int bit = 0 );
	bool GetData( const int& address, unsigned int& data, SETTYPE type, int bit = 0 );

	unsigned short Write( ACE_SOCK_Stream& peer, const int& addr, const int& words, unsigned char* buffer, bool bybit = false );
	unsigned short GetCrc( const int& length, unsigned char* value );
	unsigned short GetInternalTableValue( int );

	static DWORD WINAPI Stress( LPVOID );

	DeviceMap& GetDeviceMap();
	std::vector< std::string >& GetValidDevice();

    void addListenAddress( int port, std::string ipAddress );
    void processComingData( UINT8* buf, ssize_t dataSize, ACE_SOCK_Stream * sockStream);
    void setLocation(TA_IRS_App::PA_Sim::LocationType loc);
    void setQueryProcessor(TA_IRS_App::PA_Sim::PASimTableQueryProcessor* processor);
    TA_IRS_App::PA_Sim::ModbusQuery * processReceivedRead(std::vector<unsigned char>::iterator & iter);
    TA_IRS_App::PA_Sim::ModbusQuery * processReceivedWrite(std::vector<unsigned char>::iterator & iter);
    void isMsgComplete(unsigned char* buf, ssize_t size, ssize_t* offset);
private:
	RTUSimulator(const std::string& CfgFileName);
	bool Query( ACE_SOCK_Stream& peer,const int&, const int&, unsigned char*, bool bybit = false );
	CMDCLASS ParseCmdClass( ACE_SOCK_Stream& peer, unsigned char* buffer, const int );
	unsigned char GetABit( const unsigned char* buff, const int start, const int buflen );
	unsigned char* GetNBits( const unsigned char* buff, const int start, const int length, const int bufflen );
	int GetLogFileIndex( const ACE_INET_Addr& addr );
	void ParseBlocks( std::string& );
	void ParseReactionBlocks( std::string& );
	void ParseReactionBlocksAI( std::string& );
	void QueryAndSend( ACE_SOCK_Stream&, unsigned char* buffer, bool bybit = false );
private:
// 	CWnd * m_logWnd;
// 	CWnd * m_outputWnd;
// 	bool m_blogToFile;
/*	CFile * m_pLogFile[6];*/
	int m_svrTBgn;
	int m_svrTEnd;
	int m_portsNum;
	DeviceMap m_mapDevices;
	RegexMap m_mapBlocks;
	RegexMap m_mapExpressions;
	RegexMap m_mapReactionBlocks;
	RegexMap m_mapReactionBlocksAI;
	RegexMap m_mapReactionExpressions;
	RegexMap m_mapReactionExpressionsAI;
	std::string m_identification;
/*	std::string m_location;*/
	std::string m_cfgFile;
	ACE_INET_Addr m_portsToListen[22];
	smap_type m_mulmapStress;
	HANDLE m_hstress;
	std::vector< unsigned short > m_vtinternalTable;
	std::vector< std::string > m_vtvalidDeviceName;
/*	std::ofstream m_log;*/
	PROTOCALTYPE m_eProtocalType;
	static RTUSimulator * m_instance;
	static char * m_configFileName;

    /// the location
    TA_IRS_App::PA_Sim::LocationType m_location;
    
    /// Query processor to handle incoming packets.    
    TA_IRS_App::PA_Sim::PASimTableQueryProcessor * m_queryProcessor;
};
