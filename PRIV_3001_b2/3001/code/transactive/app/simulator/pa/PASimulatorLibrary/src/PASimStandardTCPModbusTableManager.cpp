#include "app/simulator/pa/PASimulatorLibrary/src/PASimStandardModbusTcpTable.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimStandardTCPModbusTableManager.h"
#include "app/simulator/pa/PASimulatorLibrary/src/CommonScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PlayBackDVAOneMsgScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/BroadcastBackgroundMusicScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/BroadcastScheduledDVAMsgScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/BroadcastDvaMessageScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/DVAMsgLibDownloadScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/LiveAnnouncementScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/AcknowledgeFireAlarmScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/RecordAdHocMsgScenario.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#define TRUE (1)
#define FALSE (0)
namespace TA_IRS_App
{
namespace PA_Sim
{
    //{start address, end address}
    int PADataBlock[][2] = 
    {
        {    0,     0},
        {    7,    10},
        {   14,    16},
        {  100, 19000},
        {19840, 19847},
        {20700, 20700},
        {22586, 22593},
        {24574, 24584},
        {24585, 24592},
		{24593, 24596},
		{24597, 24597},
		{24598, 24598},
		{24697, 24697},
		{24698, 24698},
        {24715, 24716},
        {24717, 24769},
        {24775, 24894},
		{24900, 24999},//lixiaoxia
        {25000, 25263},
        {25264, 26023},
        {26380, 27000},
		{27200, 27763},
		{27795, 28000},
		{43788, 48011}
    };

	const int DATA_BLOCK_NUMBER = sizeof(PADataBlock)/sizeof(PADataBlock[0]);
    /**
      * Preferred constructor
      * @param parent the owner of this object
      */
    PASimStandardTcpModbusTablesManager::PASimStandardTcpModbusTablesManager(PASimTableQueryProcessor * parent, LocationType loc)
    :m_PASimTableQueryProcessor(parent)
    ,m_location(loc)
    ,m_tableBTreeHeader(NULL)
    ,m_scenario(NULL)
    {
        // Init scenario chain
        m_scenario = new TA_IRS_App::CommonScenario(this);
        (m_scenario->setSuccessor(new TA_IRS_App::PlayBackDVAOneMsgScenario(this))).
            setSuccessor(new TA_IRS_App::BroadcastDvaMessageScenario(this)).
            setSuccessor(new TA_IRS_App::DVAMsgLibDownloadScenario(this)).
            setSuccessor(new TA_IRS_App::AcknowledgeFireAlarmScenario(this)).
            setSuccessor(new TA_IRS_App::BroadcastBackgroundMusicScenario(this)).
            setSuccessor(new TA_IRS_App::BroadcastScheduledDVAMsgScenario(this)).
            setSuccessor(new TA_IRS_App::RecordAdHocMsgScenario(this)).
            setSuccessor(new TA_IRS_App::LiveAnnouncementScenario(this)).
            setSuccessor(m_scenario);
    }

    /**
      * Destructor
      */
    PASimStandardTcpModbusTablesManager::~PASimStandardTcpModbusTablesManager()
    {
        //release header
        destroyTree(m_tableBTreeHeader);
    }

    void PASimStandardTcpModbusTablesManager::destroyTree(PASimTableStandardModbus* header)
    {
        if (header != NULL)
        {
            destroyTree(header->m_leftChild);
            destroyTree(header->m_rightChild);
            delete header;
            return;
        }
        else
            return;
    }

    bool PASimStandardTcpModbusTablesManager::addTable(PASimTableStandardModbus** header, PASimTableStandardModbus* newNode)
    {
        bool rt = FALSE;
        if (*header == NULL)
        {
            *header = newNode;
            rt = TRUE;
        } 
        else
        {
            if (newNode->m_startAddress <= newNode->m_endAddress) // check new node validity
            {
                if ((*header)->m_endAddress < newNode->m_startAddress)
                {
                    rt = addTable(&((*header)->m_rightChild), newNode);
                } 
                else if ((*header)->m_startAddress < newNode->m_endAddress)
                {
                    rt = addTable(&((*header)->m_leftChild), newNode);
                }
                else
                {
                    rt = FALSE;
                }
            } //end of check new node validity
        }
        return rt;
    }

    bool PASimStandardTcpModbusTablesManager::doInitialize()
    {
        switch(m_location)
        {
        case OCC:
        case DEPOT:
        case STATION:
            {
                int i = 0;
                for (i = 0; i < DATA_BLOCK_NUMBER; ++i)
                {
                    PASimTableStandardModbus* p = new PASimTableStandardModbus(PADataBlock[i][0], PADataBlock[i][1], m_PASimTableQueryProcessor, m_location);
                    if ( !addTable(&m_tableBTreeHeader, p))
                    {
                         TA_ASSERT( false, "PASimStandardTcpModbusTablesManager::doInitialize() == failed");
                    }
                }
                break;
            }
        default:
            TA_ASSERT( false, "Invalid location specified");
            break;
        }
        return true;
    }

    PASimTable* PASimStandardTcpModbusTablesManager::find(int startAddress)
    {
        return search(m_tableBTreeHeader, startAddress);
    }

    PASimTableStandardModbus* PASimStandardTcpModbusTablesManager::search(PASimTableStandardModbus* node, int address)
    {
        if (node != NULL)
        {
            if (address >= node->m_startAddress && address <= node->m_endAddress)
            {
                return node;
            }
            else if ( address < node->m_startAddress )
            {
                return search(node->m_leftChild, address);
            }
            else
            {
                return search(node->m_rightChild, address);
            }
        }
        else
            return NULL;
    }
    
    void PASimStandardTcpModbusTablesManager::setScenario( AbstractScenario* p )
    {
        if (NULL != p)
        {
            m_scenario = p;
        }
    }
    
    PASimTableQueryProcessor* PASimStandardTcpModbusTablesManager::getQueryProcessor()
    {
        return m_PASimTableQueryProcessor;
    }
    
    TA_IRS_App::PA_Sim::LocationType PASimStandardTcpModbusTablesManager::getLocationType()
    {
        return m_location;
    }
    
    AbstractScenario* PASimStandardTcpModbusTablesManager::getScenario()
    {
        return m_scenario;
    }    
    void PASimStandardTcpModbusTablesManager::getPAZoneStatusTableAddressIsOne(std::vector<unsigned long>& address)
    {
        unsigned long matrixRoutingTableStartAddress = 0UL;
        unsigned long matrixRoutingTableEndAddress = 0UL;
        unsigned long paStatusMatrixTableStartAddress = 0UL;

        if ( OCC == getLocationType())
        {
            matrixRoutingTableStartAddress = PAS_ADDRESS_OCC_MATRIX_TABLE_START;
            matrixRoutingTableEndAddress = PAS_ADDRESS_OCC_MATRIX_TABLE_END;
            paStatusMatrixTableStartAddress = PAS_ADDRESS_OCC_PA_STATUS_MATRIX_TABLE_START;
        }
        else if (STATION == getLocationType())
        {
            matrixRoutingTableStartAddress = PAS_ADDRESS_STN_MATRIX_TABLE_START;
            matrixRoutingTableEndAddress = PAS_ADDRESS_STN_MATRIX_TABLE_END;
            paStatusMatrixTableStartAddress = PAS_ADDRESS_STN_PA_STATUS_MATRIX_TABLE_START;
        }

        PASimTable* pTable = find(matrixRoutingTableStartAddress);
        ByteVector matrixRoutingTable;
        if ( NULL != pTable)
        {
            pTable->getByteVector(matrixRoutingTable);
            if (0 != matrixRoutingTable.size())
            {
                ByteVector::iterator it;
                unsigned long beginAddress = matrixRoutingTableStartAddress - 1;
                for (it = matrixRoutingTable.begin(); it != matrixRoutingTable.end(); ++it)
                {
                    unsigned char tmp = *(it+1);
                    for (int i = 0; i < 8; ++i)
                    {
                        ++beginAddress;
                        (((tmp) & 0x01) == 0x01)? address.push_back(beginAddress - matrixRoutingTableStartAddress + paStatusMatrixTableStartAddress) : false;
                        tmp>>=1;
                    }
                    tmp = *it;
                    for (int i = 0; i < 8; ++i)
                    {
                        ++beginAddress;
                        (((tmp) & 0x01) == 0x01)? address.push_back(beginAddress - matrixRoutingTableStartAddress + paStatusMatrixTableStartAddress) : false;
                        tmp>>=1;
                    }
                    ++it;
                }
            }
        }
    }
    void PASimStandardTcpModbusTablesManager::getMatrixRoutingTableAddressIsOne( std::vector<int>& address )
    {
        unsigned long matrixRoutingTableStartAddress = 0UL;
        unsigned long matrixRoutingTableEndAddress = 0UL;
        unsigned long routingStatusTableStartAddress = 0UL;

        if ( OCC == getLocationType())
        {
            matrixRoutingTableStartAddress = PAS_ADDRESS_OCC_MATRIX_TABLE_START;
            matrixRoutingTableEndAddress = PAS_ADDRESS_OCC_MATRIX_TABLE_END;
            routingStatusTableStartAddress = PAS_ADDRESS_OCC_ROUTING_STATUS_MATRIX_TABLE_START;
        }
        else if (STATION == getLocationType())
        {
            matrixRoutingTableStartAddress = PAS_ADDRESS_STN_MATRIX_TABLE_START;
            matrixRoutingTableEndAddress = PAS_ADDRESS_STN_MATRIX_TABLE_END;
            routingStatusTableStartAddress = PAS_ADDRESS_STN_ROUTING_STATUS_MATRIX_TABLE_START;
        }

        PASimTable* pTable = find(matrixRoutingTableStartAddress);
        ByteVector matrixRoutingTable;
        if ( NULL != pTable)
        {
            pTable->getByteVector(matrixRoutingTable);
            if (0 != matrixRoutingTable.size())
            {
                ByteVector::iterator it;
                int beginAddress = matrixRoutingTableStartAddress - 1;
                for (it = matrixRoutingTable.begin(); it != matrixRoutingTable.end(); ++it)
                {
                    unsigned char tmp = *(it+1);
                    for (int i = 0; i < 8; ++i)
                    {
                        ++beginAddress;
                        (((tmp) & 0x01) == 0x01)? address.push_back(beginAddress - matrixRoutingTableStartAddress + routingStatusTableStartAddress) : false;
                        tmp>>=1;
                    }
                    tmp = *it;
                    for (int i = 0; i < 8; ++i)
                    {
                        ++beginAddress;
                        (((tmp) & 0x01) == 0x01)? address.push_back(beginAddress - matrixRoutingTableStartAddress + routingStatusTableStartAddress) : false;
                        tmp>>=1;
                    }
                    ++it;
                }
            }
        }
    }
    
    void PASimStandardTcpModbusTablesManager::resetMatrixRoutingTable()
    {
        PASimTableStandardModbus* pTable = dynamic_cast<PASimTableStandardModbus* >(this->find(25000));
        pTable->resetAllData2Zero();
    }
    
    int PASimStandardTcpModbusTablesManager::getRegisterValue( int address )
    {
        PASimTableStandardModbus* pTable = dynamic_cast<PASimTableStandardModbus* >(this->find(address));
		if (NULL != pTable)
		{
			return pTable->getRegisterValue(address);
		}
		else
			return -1;
        
    }
    
    int PASimStandardTcpModbusTablesManager::setRegisterValue( int address, int val )
    {
        PASimTableStandardModbus* pTable = dynamic_cast<PASimTableStandardModbus* >(this->find(address));
        return pTable->setRegisterValue(address, val);
    }
    
    void PASimStandardTcpModbusTablesManager::getSpeakerLineTableAddressIsOne( std::vector<int>& address )
    {
        PASimTable* pTable = find(25000);
        ByteVector matrixRoutingTable;
        if ( NULL != pTable)
        {
            pTable->getByteVector(matrixRoutingTable);
            if (0 != matrixRoutingTable.size())
            {
                ByteVector::iterator it;
                int beginAddress = 25000 - 1;
                for (it = matrixRoutingTable.begin(); it != matrixRoutingTable.begin() + 64; ++it)
                {
                    unsigned char tmp = *(it+1);
                    for (int i = 0; i < 8; ++i)
                    {
                        ++beginAddress;
                        (((tmp) & 0x01) == 0x01)? address.push_back(beginAddress - 25000 + 27732) : false;
                        tmp>>=1;
                    }
                    tmp = *it;
                    for (int i = 0; i < 8; ++i)
                    {
                        ++beginAddress;
                        (((tmp) & 0x01) == 0x01)? address.push_back(beginAddress - 25000 + 27732) : false;
                        tmp>>=1;
                    }
                    ++it;
                }
            }
        }
    }

	int PASimStandardTcpModbusTablesManager::setMultiRegistersValues( int startAddress, int endAddress, ByteVector& data )
	{
		PASimTableStandardModbus* pTable = dynamic_cast<PASimTableStandardModbus* >(this->find(startAddress));
		return pTable->setMultiRegistersValues(startAddress, endAddress, data);
	}
}
}