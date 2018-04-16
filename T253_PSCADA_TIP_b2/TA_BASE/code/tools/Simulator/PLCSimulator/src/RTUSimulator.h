// RTUSimulator.h: interface for the RTUSimulator class.
//
////////////////////////////////////////////////////////

#pragma once

class StressTaskManager;
class CPLCSimulatorDlg;
class ClientRun;

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

// Command line variables
#define CONFIG_RPARAM		"config-file"
#define STRESS_RPARAM		"stress-file"
#define SAVEDATA_PARAM		"savedata-file"
#define STRESSON_PARAM		"stress-on"
#define PLCON_PARAM			"plc-on"
#define LOGFILE_PARAM		"log-file"
#define REACTIONFILE_PARAM	"reaction-file"
#define PAUSE_OUTPUT_DISPLAY		"pause_output_display"	//:0/1
#define PAUSE_COMMUNICATION_DISPLAY	"pause_communication_display"	//:0/1
#define MODBUSTYPE_PARAM "modbus-type"

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
	int  ParseWaitTime( std::string&str);
	void SetProtocalType( PROTOCALTYPE protocalType );
    PROTOCALTYPE getProtocalType();
	void SetLogWnd( CWnd* );
	void SetOutputWnd( CWnd* );
	void Send( ACE_SOCK_Stream& peer, unsigned char* buffer, int size );
	void SendExp( ACE_SOCK_Stream& peer, unsigned char* buffer, EXPTYPE type );
	void WriteAndSend( ACE_SOCK_Stream& peer, unsigned char* buffer, bool bybit = false );
	void Write1AndSend( ACE_SOCK_Stream& peer, unsigned char* buffer, bool bybit = false );
	void SetLogToFile( bool flag );
	void WriteLogFile( const ACE_SOCK_Stream& peer, std::string text );
	void InsCrc( const int& length, unsigned char* );
	void PauseStress();
	void ResumeStress();
	void BgnStress( void );
	void SetDeviceStatus( std::string dname, bool status );
	void SetStatus();
	void ReloadSaveData();
	void SaveData();
	void LoadData( std::string FileName );
	void LoadBlockAddress();
	void LoadDeviceMap( const std::string& );
	int  InitConfigFile( const std::string& FileName );	// and reload savedata.txt
	void InitReaction();
	void InitSimulator();

	std::string ConvertToAssic( unsigned char c );
	std::string GetIdentification( void );
	std::string GetLocation( void );
	std::string GetAddress();
	std::string GetPollPort();
	std::string GetCmdPort( void );
	const std::string& GetCfgName();
	bool GetPLCOn();
	bool GetStressOn();
	const std::string& GetStressCfgName();
	// more then one stress config, between configs add ';'
	void SetStressCfgName(const std::string& FileName);
	const std::string& GetPauseOutputDisplay();
	const std::string& GetPauseCommunicationDisplay();

	int StartSvr(const int i );
	int GetPortsNum( void );

	bool StressCallBack(int address,unsigned int data, int bit);
	bool SetData( int address,unsigned int data, SETTYPE type, int bit, bool flag );
	bool SetData( int address,unsigned int data, SETTYPE type, int bit = 0 );
	bool SetReactionData( int address,unsigned int data, SETTYPE type, int bit = 0 );
	bool GetData( const int& address, unsigned int& data, SETTYPE type, int bit = 0 );

	unsigned short Write( ACE_SOCK_Stream& peer, const int& addr, const int& words, unsigned char* buffer, bool bybit = false );
	unsigned short GetCrc( const int& length, unsigned char* value );
	unsigned short GetInternalTableValue( int );

	DeviceMap& GetDeviceMap();
	int  IsDeviceValid(const std::string& dev);
	void registerForUpdate(CPLCSimulatorDlg* reg);
	CMDCLASS ParseCmdClass( ACE_SOCK_Stream& peer, unsigned char* buffer, const int, int port);
	void QueryAndSend( ACE_SOCK_Stream&, unsigned char* buffer, bool bybit = false );
	void writeToLogLn(std::string& logStr);
	void resumeClientsOnListenPort(int index);
	void stopClientsOnListenPort(int index);
	void setCheckClientRequestAddr(bool isCheck);
private:
	RTUSimulator(const std::string& CfgFileName);
	bool Query( ACE_SOCK_Stream& peer,const int&, const int&, unsigned char*, bool bybit = false );
	unsigned char GetABit( const unsigned char* buff, const int start, const int buflen );
	unsigned char* GetNBits( const unsigned char* buff, const int start, const int length, const int bufflen );
	int GetLogFileIndex( const ACE_INET_Addr& addr );
	void ParseBlocks( std::string& );
	void ParseReactionBlocks( std::string& );
	void ParseReactionBlocksAI( std::string& );

	bool SetTimeStamp(int address);
	bool ParseCmdLineParameters();
	void ProcessValueUpdate(int address, SETTYPE type = BYWORD, int bit = 0);
	inline char MSB( int data) const { return data >> 8;}
	inline char LSB( int data) const { return data & 0xFF;}

private:
	CWnd * m_logWnd;
	CWnd * m_outputWnd;
	bool m_blogToFile;
	bool m_checkClientRequestAddress;
	CFile * m_pLogFile[6];
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
	std::string m_location;
	std::string m_cfgFile;
	ACE_INET_Addr m_portsToListen[22];
	smap_type m_mulmapStress;
	HANDLE m_hstress;
	ACE_Thread_Mutex m_threadMutex;
	std::vector< unsigned short > m_vtinternalTable;
	std::ofstream m_log;
	PROTOCALTYPE m_eProtocalType;
	static RTUSimulator * m_instance;
	static char * m_configFileName;
	std::map<unsigned short, unsigned short> m_blockStartAddress;	// for get block startaddress;
	std::map<std::string, std::string> m_allParameters;
	StressTaskManager* m_stressTaskMgr;
	std::map<int, std::vector<std::string> > m_devicesAddressMap;
	CPLCSimulatorDlg* m_myDlg;
	ACE_Thread_Mutex m_clientsConnectionMutex;
	std::map<int, std::vector<ClientRun*> > m_clientsOnListenPorts;
private:
	static const char MODBUS_ERROR_CODE;
	static const int MODBUS_PDU_FUNCTION_INDEX;
	static const int MODBUS_R_RESPONSE_PDU_BTYE_NUM_INDEX;
	static const int MODBUS_R_RESPONSE_PDU_DATA_INDEX;
	static const int MODBUS_W_RESPONSE_PDU_START_ADDR_INDEX;
	static const int MODBUS_W_RESPONSE_PDU_QUANTITY_OUTPUT_INDEX;
	static const int MODBUS_ERROR_CODE_PDU_INDEX;
	static const int MODBUS_ERROR_SUB_CODE_PDU_INDEX;
	static const int MODBUS_REQUEST_PDU_START_ADDR_INDEX;
	static const int MODBUS_REQUEST_PDU_QUANTITY_OUTPUT_INDEX;
	static const int MODBUS_W_REQUEST_PDU_DATA_BYTE_NUM_INDEX;
	static const int MODBUS_W_REQUEST_PDU_DATA_INDEX;
	static const int STRANDARD_MODBUS_MBAP_LEN_H_INDEX;
	static const int STRANDARD_MODBUS_MBAP_LEN_L_INDEX;
	static const int MODBUS_MPAP_HEAD_SIZE;
	static const int C830_MODBUS_HEAD_SIZE;
};
