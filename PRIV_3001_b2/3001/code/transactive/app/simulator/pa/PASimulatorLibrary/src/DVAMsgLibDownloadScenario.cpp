#include "app/simulator/pa/PASimulatorLibrary/src/DVAMsgLibDownloadScenario.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/exception.hpp"
#include "boost/regex/v4/regex.hpp"
#include <ctime>

std::vector<TA_IRS_App::DvaMsg> TA_IRS_App::DVAMsgLibDownloadScenario::DvaMessageVec;
int TA_IRS_App::DVAMsgLibDownloadScenario::MassageChangeCounter = -1;
ByteVector TA_IRS_App::DVAMsgLibDownloadScenario::LibVersionNumber;
TA_IRS_App::DVAMsgLibDownloadScenario::~DVAMsgLibDownloadScenario()
{
    
}

bool TA_IRS_App::DVAMsgLibDownloadScenario::isMySpecialScenario( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    checkAndUpdateDVAMsgLib();
    if (MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);

        if ( (PAS_ADDRESS_OCC_COMMAND_REGISTER_START == p->m_startAddress || 
			PAS_ADDRESS_STN_COMMAND_REGISTER_START  == p->m_startAddress ) && 
			1 == p->m_registersQuantity && 
			p->m_registersValue[0] == 0x1D && 
			p->m_registersValue[1] == 0x00) // read the 6 first message
        {
            m_tcpModbusTablesManager->setRegisterValue(PAS_ADDRESS_EXECUTION_REGISTER_START, 1);
            m_msgIndex = 0;
            m_isInCurrentScenario = true;
            updateRegistersAccordingToDvaMsgLib(m_msgIndex);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Entering DVAMsgLibDownloadScenario ...");
            return true;
        }

    }
    return false;
}

void TA_IRS_App::DVAMsgLibDownloadScenario::reponseReadQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if (MODBUS_READ_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusReadQuery* p = dynamic_cast<ModbusReadQuery*>(query);
        if ( (PAS_ADDRESS_OCC_DVA_LIB_VERSION_START == p->m_startAddress ||
			 PAS_ADDRESS_STN_DVA_LIB_VERSION_START == p->m_startAddress) && 
			 3 == p->m_registersQuantity)
        {
            m_isInCurrentScenario = false;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finishing DVAMsgLibDownloadScenario ...");
        }
    }
}

void TA_IRS_App::DVAMsgLibDownloadScenario::reponseWriteQuery( TA_IRS_App::PA_Sim::ModbusQuery* query )
{
    if ( MODBUS_WRITE_REGISTER_FUNCTION_CODE == query->m_functionCode)
    {
        ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
       if ((PAS_ADDRESS_OCC_COMMAND_REGISTER_START == p->m_startAddress ||
				PAS_ADDRESS_STN_COMMAND_REGISTER_START == p->m_startAddress) && 
				1 == p->m_registersQuantity && 
				0x1E == p->m_registersValue[0] && 
				0x00 == p->m_registersValue[1]) // read 6 next messages
        {
            m_tcpModbusTablesManager->setRegisterValue(PAS_ADDRESS_EXECUTION_REGISTER_START, 1);
            m_msgIndex+=6;
            updateRegistersAccordingToDvaMsgLib(m_msgIndex);
        }
        if ( IS_EXE_REG_BE_SET(p) )
        {
            //setExecutionRegister2Zero();
        }
    }
}

TA_IRS_App::DVAMsgLibDownloadScenario::DVAMsgLibDownloadScenario( PASimStandardTcpModbusTablesManager* parent )
    :AbstractScenario(parent)
    ,m_msgIndex(0)
{
    // init crc
    for (int i = 1; i <= ((PAS_ADDRESS_OCC_DVA_LIB_VERSION_END - PAS_ADDRESS_OCC_DVA_LIB_VERSION_START) + 1); ++i)
    {
        LibVersionNumber.push_back(0x00);
        LibVersionNumber.push_back(i<<1);
    }

    checkAndUpdateDVAMsgLib();
}

void TA_IRS_App::DVAMsgLibDownloadScenario::checkAndUpdateDVAMsgLib()
{
    boost::filesystem::path pFile("DVA_Message_Lib.csv");
    std::time_t t = 0;
    try
    {
        t = boost::filesystem::last_write_time(pFile);
    }
    catch (boost::filesystem::filesystem_error &e)
    {
        std::cerr << e.what() << std::endl;
    }

    if (MassageChangeCounter < 0 || m_lastModDvaMsgLibTime != t)
    {
        m_lastModDvaMsgLibTime = t;
        
        // Update m_dvaMsgVec
        // read dva lib file
        std::ifstream readFile("DVA_Message_Lib.csv",  std::ios::binary | std::ios::in);
        std::string fileContent;
        boost::smatch what;
        if ( readFile)
        {
            readFile.seekg(0,std::ios::end);
            unsigned long length = readFile.tellg();
	        readFile.seekg(0,std::ios::beg);
            fileContent.resize(length + 1, '\0');
            std::copy( std::istreambuf_iterator<char>(readFile) ,
                std::istreambuf_iterator<char>() , 
                fileContent.begin() );            
	        readFile.close();
	    }

        // parse dva lib file
        DvaMessageVec.clear();
        boost::regex expression("([\\d]+),([\\d]+),([\\d]+),([^\\n]+)");
        boost::sregex_iterator it( fileContent.begin(), fileContent.end(), expression );
        boost::sregex_iterator end;
        DvaMsg tmpDvaMsg;
        for ( ; it != end; ++it )
        {
            tmpDvaMsg.index = atoi(it->str(1).c_str());
            tmpDvaMsg.msgId = atoi(it->str(1).c_str());
            tmpDvaMsg.duration = atoi(it->str(2).c_str());
            tmpDvaMsg.msgBank = atoi(it->str(3).c_str());
            tmpDvaMsg.msgName = it->str(4).c_str();
            DvaMessageVec.push_back(tmpDvaMsg);
        }

        // make the last msg which has an 0 msg Id
        tmpDvaMsg.index = 0;
        tmpDvaMsg.msgId = 0;
        tmpDvaMsg.msgBank = 0;
        tmpDvaMsg.duration = 0;
        tmpDvaMsg.msgName = "Kingjnu";
        DvaMessageVec.push_back(tmpDvaMsg);

        updateMessageLibraryVersion(this);
        
        updateMessageChangeCounter(this);
    }
}

void TA_IRS_App::DVAMsgLibDownloadScenario::updateRegistersAccordingToDvaMsgLib(int index)
{
    size_t startReg = 0;
    size_t endReg = 0;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DVAMsgLibDownloadScenario updateIndex = %d", index);

	if ( OCC == m_tcpModbusTablesManager->getLocationType())
	{
		startReg = PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_START;
		endReg = PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_END;
	} 
	else
	{
		startReg = PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_START;
		endReg = PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_END;
	}

    ByteVector data;
    std::vector<DvaMsg>::iterator it;
    size_t dataSize = 0;
    int counter = 6;

    if ( index >= DvaMessageVec.size() )
    {
        dataSize+=(20 * 12);
        while (data.size() < dataSize)
        {
            data.push_back(0x00);
        }
    }
    else
    {
        for (it = (DvaMessageVec.begin() + index); it != DvaMessageVec.end() && counter > 0; ++it)
        {
            --counter;
            dataSize+=(20 * 2);
            endReg+=9;
            // fill in other info except message name
            // fill in index
            data.push_back(MSB(it->index));
            data.push_back(LSB(it->index));

            // fill in reserved
            data.push_back(0x00);
            data.push_back(0x00);

            // fill in msg id
            data.push_back(MSB(it->msgId));
            data.push_back(LSB(it->msgId));

            // fill in bank id
            data.push_back(MSB(it->msgBank));
            data.push_back(LSB(it->msgBank));

            // fill in message name
            for (size_t i = 0; i < (it->msgName.size() - 1); ++i)
            {
                data.push_back(it->msgName[i]);
                if (data.size() >= dataSize)
                {
                    break;
                }
            }
            while (data.size() < dataSize)
            {
                data.push_back(0x00);
            }
        }
    }

    deliveryUserQuery(Set, startReg, endReg, &data, 1);
    ByteVector dataExe;
    dataExe.push_back(0x00);
    dataExe.push_back(0x00);
    deliveryUserQuery(Set, PAS_ADDRESS_EXECUTION_REGISTER_START, PAS_ADDRESS_EXECUTION_REGISTER_START, &dataExe, 3);
}

void TA_IRS_App::DVAMsgLibDownloadScenario::ChangeMessageType( int msgId)
{
    std::vector<DvaMsg>::iterator it;
    for (it = DvaMessageVec.begin(); it != DvaMessageVec.end(); ++it)
    {
        if (msgId == it->msgId)
        {
            if (2 == it->msgBank)
            {
                it->msgBank = 4;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Change Dva message type: from Emergency to Normal");
            }
            else if (4 == it->msgBank)
            {
                it->msgBank = 2;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Change Dva message type: from Normal to Emergency");
            }
            break;
        }
    }
}

void TA_IRS_App::DVAMsgLibDownloadScenario::DeleteDvaMessage( int msgId )
{
    std::vector<DvaMsg>::iterator it;
    for (it = DvaMessageVec.begin(); it != DvaMessageVec.end(); ++it)
    {
        if (msgId == it->msgId)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Erase Dva message: id = %d, bankId = %d, name = %s", it->msgId, it->msgBank, it->msgName);
            DvaMessageVec.erase(it);
            break;
        }
    }
}

int TA_IRS_App::DVAMsgLibDownloadScenario::getDurationFromMsgId( int msgId )
{
    if (DvaMessageVec.size() > 0)
    {
        std::vector<DvaMsg>::iterator it;
        for (it = DvaMessageVec.begin(); it != DvaMessageVec.end(); ++it)
        {
            if (msgId == it->msgId)
            {
                return it->duration;
            }
        }
    }
	return -1;
}

void TA_IRS_App::DVAMsgLibDownloadScenario::updateMessageChangeCounter(TA_IRS_App::AbstractScenario* trigger)
{
    ++MassageChangeCounter;
    ByteVector counter;
    counter.push_back(0x00);
    counter.push_back(MassageChangeCounter);
    trigger->deliveryUserQuery(Set, PAS_POLLING_DVA_LIB_COUNTER_START, PAS_POLLING_DVA_LIB_COUNTER_END, &counter,0);
}

void TA_IRS_App::DVAMsgLibDownloadScenario::updateMessageLibraryVersion(TA_IRS_App::AbstractScenario* trigger)
{
    // change version number
    for (int i = 0; i < (((PAS_ADDRESS_OCC_DVA_LIB_VERSION_END - PAS_ADDRESS_OCC_DVA_LIB_VERSION_START)+1)*2); ++i)
    {
        LibVersionNumber[i] = (LibVersionNumber[i] << 1);
    }

    // set crc to registers
    if (OCC == trigger->getTablesManager()->getLocationType())
    {
        trigger->deliveryUserQuery(Set, PAS_ADDRESS_OCC_DVA_LIB_VERSION_START, PAS_ADDRESS_OCC_DVA_LIB_VERSION_END, &LibVersionNumber, 0);
    }
    else
    {
        trigger->deliveryUserQuery(Set, PAS_ADDRESS_STN_DVA_LIB_VERSION_START, PAS_ADDRESS_STN_DVA_LIB_VERSION_END, &LibVersionNumber, 0);
    }
}
