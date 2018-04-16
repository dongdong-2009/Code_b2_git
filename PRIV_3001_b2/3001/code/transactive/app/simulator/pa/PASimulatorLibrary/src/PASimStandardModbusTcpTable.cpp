/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimStandardModbusTcpTable.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  * Definition file for class PASimTablePATrain.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/simulator/pa/PASimulatorLibrary/src/PASimStandardModbusTcpTable.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableQueryProcessor.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimFlexiMask.h"
#include "app/simulator/pa/PASimulatorLibrary/src/CommonScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PlayBackDVAOneMsgScenario.h"
#include "app/simulator/pa/PASimulatorLibrary/src/DVAMsgLibDownloadScenario.h"
#include <TIME.H>
#include <stdexcept>
#include <iostream>

// using namespace TA_IRS_App;
using namespace TA_IRS_App::PA_Sim;

PASimTableStandardModbus::PASimTableStandardModbus(int startAddress, 
                                                     int endAddress, 
                                                     PASimTableQueryProcessor* parent, 
                                                     LocationType loc)
:PASimTable(INVALID_IDENTIFIER,parent, loc),
m_startAddress(startAddress),
m_endAddress(endAddress),
m_data(startAddress, endAddress),
m_leftChild(NULL),
m_rightChild(NULL)
{
     m_data.resize((m_endAddress - m_startAddress + 1) * 2);
     for (ByteVector::iterator it = m_data.begin(); it != m_data.end(); ++it)
     {
         *it = 0;
     }
}


PASimTableStandardModbus::~PASimTableStandardModbus()
{
}

unsigned short PASimTableStandardModbus::getTableSize() const
{
    return 0;
}



UserResponse * PASimTableStandardModbus::processUserRead(UserQuery * qry)
{
#if defined PASIMTABLEPATRAIN_PROCESSUSERREAD_FIXED

#endif // #if defined PASIMTABLEPATRAIN_PROCESSUSERREAD_FIXED

    
    if (qry->m_endAddress >= qry->m_startAddress )
    {
        ByteVector::iterator first = m_data.begin() + (qry->m_startAddress - m_startAddress) * 2;
        ByteVector::iterator last = (qry->m_endAddress > m_endAddress) ? m_data.end() : (first + (qry->m_endAddress - qry->m_startAddress + 1) * 2);
        int i = 0;
        std::cout << std::endl;
        for ( ByteVector::iterator it = first; it != last; it+=2)
        {
            printf("[%d]: %02x%02x \n", qry->m_startAddress + i, *it, *(it + 1));
            ++i;
        }
    }
    else
    {
        std::cout << "Invaild args: EndAddress must greater than StartAddress" 
                  << "start address = " << qry->m_startAddress
                  << "end address = " << qry->m_endAddress << std::endl;
    }
    return 0;
}


UserResponse * PASimTableStandardModbus::processUserWrite(UserQuery * qry)
{
    if (qry->m_endAddress >= qry->m_startAddress )
    {
        ByteVector::iterator it = m_data.begin() + (qry->m_startAddress - m_startAddress) * 2;
        int size = qry->m_data.size();
        for ( ByteVector::iterator i = qry->m_data.begin(); i != qry->m_data.end(); ++i)
        {
            *it = *i;
            ++it;
        }
    }
    else
    {
        std::cout << "Invaild args: EndAddress must greater than StartAddress" 
            << "start address = " << qry->m_startAddress
            << "end address = " << qry->m_endAddress << std::endl;
    }
    return 0;
}


void PASimTableStandardModbus::toStream(std::ostream& os) const
{

}


void PASimTableStandardModbus::fromStream(const std::ostream& os)
{
}


void PASimTableStandardModbus::toNetwork(ByteVector & n)
{

}

void PASimTableStandardModbus::processReadQuery(ModbusQuery * query)
{
    ModbusReadResponse * response = new ModbusReadResponse;
    response->m_transactionIdentifier = query->m_transactionIdentifier;
    response->m_protocolIdentifier = query->m_protocolIdentifier;
    response->m_length = 3 + query->m_registersQuantity * 2;
    response->m_unitIdentifier = query->m_unitIdentifier;
    response->m_functionCode = query->m_functionCode;
    if (query->m_registersQuantity * 2 > 255)
    {
        response->m_byteCount = 0;
    }
    else
    {
        response->m_byteCount = query->m_registersQuantity * 2;
    }
    response->m_registersValue.resize(query->m_registersQuantity * 2);
	response->setSocket(query->getSocket());
    std::copy(m_data.begin() + (query->m_startAddress - m_startAddress) * 2, 
              m_data.begin() + (query->m_startAddress - m_startAddress) * 2 + query->m_registersQuantity * 2, 
              response->m_registersValue.begin());
    m_parent->sendReadResponse(response);

    delete response;
}

void PASimTableStandardModbus::processWriteQuery(ModbusQuery * query)
{
    int errCode;
    ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
//     if ((errCode = isWriteQueryIllegal(query))!= 0x00)
//     {
//         ModbusWriteResponse * response = new ModbusWriteResponse;
//         response->m_transactionIdentifier = query->m_transactionIdentifier;
//         response->m_protocolIdentifier = query->m_protocolIdentifier;
//         response->m_length = 0x04;
//         response->m_unitIdentifier = query->m_unitIdentifier;
//         response->m_functionCode = 0x81;
//         response->m_startAddress = errCode;
//         response->setSocket(query->getSocket());
//         m_parent->sendWriteResponse(response);
//     }
//     else
    {
        // suck all the data in
        size_t n = p->m_registersValue.size();
        std::copy(p->m_registersValue.begin(), 
            p->m_registersValue.end(),
            m_data.begin() + (p->m_startAddress - m_startAddress) * 2);
        
        // create and populate reponse
        ModbusWriteResponse * response = new ModbusWriteResponse;
        response->m_transactionIdentifier = query->m_transactionIdentifier;
        response->m_protocolIdentifier = query->m_protocolIdentifier;
        response->m_length = 6;
        response->m_unitIdentifier = query->m_unitIdentifier;
        response->m_functionCode = query->m_functionCode;
        response->m_startAddress = query->m_startAddress;
        response->m_registersQuantity = query->m_registersQuantity;
        response->setSocket(query->getSocket());
        m_parent->sendWriteResponse(response);
    }

}

void PASimTableStandardModbus::fromNetwork(const ByteVector & s)
{
    throw std::logic_error("PA Train Table should not be put to vector fromNetwork");
}


void PASimTableStandardModbus::getByteVector(ByteVector & bv) const
{
    bv.resize(m_data.size());
    std::copy(m_data.begin(), m_data.end(), bv.begin());
}

void TA_IRS_App::PA_Sim::PASimTableStandardModbus::resetAllData2Zero()
{
    std::fill(m_data.begin(), m_data.end(), 0x00);
}

int TA_IRS_App::PA_Sim::PASimTableStandardModbus::getRegisterValue( int address )
{
    size_t offSet = address - m_startAddress;
    return (m_data[offSet * 2] * 0x10 * 0x10 + m_data[offSet * 2 + 1]);
}

int TA_IRS_App::PA_Sim::PASimTableStandardModbus::setRegisterValue( int address, int val )
{
    if (address >= m_startAddress && address <= m_endAddress)
    {
        size_t offSet = address - m_startAddress;
        m_data[offSet*2] = MSB(val);
        m_data[offSet*2+1] = LSB(val);
        return (m_data[offSet * 2] * 0x10 * 0x10 + m_data[offSet * 2 + 1]);
    }
    else
        return -1;
}

int TA_IRS_App::PA_Sim::PASimTableStandardModbus::isWriteQueryIllegal( ModbusQuery * query )
{
    int rt = 0x00;
    // process message Id
    ModbusWriteQuery* p = dynamic_cast<ModbusWriteQuery*>(query);
    if (24769 >= p->m_startAddress && 24769 <= (p->m_startAddress + p->m_registersQuantity))
    {
        int regValue = p->m_registersValue[(24769 - p->m_startAddress) * 2] * 0x100 + p->m_registersValue[(24769 - p->m_startAddress) * 2 + 1];
        if (-1 == DVAMsgLibDownloadScenario::getDurationFromMsgId(regValue))
        {
            rt = 0x03;
        }
    }
    return rt;
}

int TA_IRS_App::PA_Sim::PASimTableStandardModbus::setMultiRegistersValues( int startAddress, int endAddress, ByteVector& data )
{
	UserQuery* q = new UserQuery(Set);
	q->m_startAddress = startAddress;
	q->m_endAddress = endAddress;
	q->m_data = data;
	q->setType(Set);
	processUserWrite(q);
	return 0;
}


UserResponse * TA_IRS_App::PA_Sim::PASimTableStandardModbus::processUserIncQuery( UserQuery * query )
{
	if (query->m_endAddress >= query->m_startAddress )
	{
		int oriVal;
		for(int i = query->m_startAddress; i <= query->m_endAddress; ++i)
		{
			oriVal = getRegisterValue(i);
			setRegisterValue(i, ++oriVal);
		}
	}
	else
	{
		std::cout << "Invaild args: EndAddress must greater than StartAddress" 
			<< "start address = " << query->m_startAddress
			<< "end address = " << query->m_endAddress << std::endl;
	}
	return 0;
}
