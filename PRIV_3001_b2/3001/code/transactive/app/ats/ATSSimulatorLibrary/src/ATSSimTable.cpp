/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTable.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Definition file for ATSSimTable class
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTable.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableQueryProcessor.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimModbusFrames.h"
#include "core/utilities/src/DebugUtil.h"
#include <stdexcept>
#include <iostream>

//using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

ATSSimTable::ATSSimTable(TableIdentifier tableId, ATSSimTableQueryProcessor * parent, LocationType loc)
: m_numberOfRecords(0)
, m_tableIdentifier(tableId)
, m_tableAddress(0xFFFF)
, m_location(loc)
, m_parent(parent)
, m_dirty(false)
, m_exceptionCode(ATSSim_EC_NoException)
// , m_dependents(0)
{
    m_tableAddress = TableAddressRegister::getInstance().getAddress(m_tableIdentifier, m_location);
    parent->addTable(this);
}


ATSSimTable::~ATSSimTable()
{
    // all data members are by value, and the m_tableData stores bytes by
    // value, so there isn't any explicit destructing to be done here.
}

unsigned short ATSSimTable::getTableAddress() const
{
    return m_tableAddress;
}


unsigned short ATSSimTable::getTableWordSize() const
{
    return getTableSize()/2;
}


unsigned short ATSSimTable::getTableIdentifier() const
{
    return m_tableIdentifier;
}


ATSSimTableQueryProcessor * ATSSimTable::getParent() const
{
    return m_parent;
}


LocationType ATSSimTable::getLocation() const
{
    return m_location;
}


void ATSSimTable::processReadQuery(ModbusQuery * query)
{
	//
	// minor verification of the query
    if (getTableSize() != query->m_tableSize)
    {
        size_t ours = getTableSize();
        size_t theirs = query->m_tableSize;
/*
        std::cerr << "WARNING: read query for table " << getTableAddress() << " requests different size (expected="
                  << ours << ", received="
                  << theirs << ")" << std::endl;
        std::cerr << "SENDING correct local size" << std::endl;
*/      LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ATSSimTable::processReadQuery(%d) detected unequal table sizes (this=%d, that=%d)",
            query->m_tableAddress,ours,theirs);
    }

    // Create and populate a Read Query Response Frame
    //
    ModbusReadResponse * response = new ModbusReadResponse;
    response->m_tableAddress = getTableAddress();
    response->m_tableSize = getTableSize();
    response->m_tableData.reserve(response->m_tableSize);
	response->setSocket(query->getSocket());
	// TODO copy the data
    // std::copy(m_tableData.begin(),m_tableData.end(),response->m_tableData.begin());
    toNetwork(response->m_tableData);

    // send it.
    m_parent->sendReadResponse(response);

    // once sent, can be disposed of
    delete response;
}


void ATSSimTable::processWriteQuery(ModbusQuery * query)
{
    // assimilate the data.
    // 
    if (getTableSize() != query->m_tableData.size())
    {
        size_t ours = getTableSize();
        size_t theirs = query->m_tableData.size();

        std::cerr << "WARNING: received table " << getTableAddress() << " data different size (expected="
                  << ours << ", received="
                  << theirs << ")" << std::endl;
        std::cerr << "RESIZING to match foreign data size" << std::endl;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "ATSSimTable::processWriteQuery(%d) detected unequal table sizes (this=%d, that=%d)",
            query->m_tableAddress,ours,theirs);
        // m_tableData.reserve(query->m_tableData.size());
    }

    //
    // suck all the data in
    // std::copy(query->m_tableData.begin(), query->m_tableData.end(), m_tableData.begin());
    fromNetwork(query->m_tableData);
    
    // Create and populate a Write Query Response Frame
    ModbusWriteResponse * response = new ModbusWriteResponse;
    response->m_tableAddress = getTableAddress();
    response->m_tableSize = getTableSize();
    response->m_exceptionCode = m_exceptionCode;
	response->setSocket(query->getSocket());

    // reset the write response return value
    m_exceptionCode = ATSSim_EC_NoException;

    m_parent->sendWriteResponse(response);

    notifyDependents(iscs_write_query);

    delete response;
}


#ifdef ATSSIMTABLE_USEFUL_ACCESSORS
Byte ATSSimTable::getByte(unsigned int index) const
{
    if (index>getTableSize())
    {
        throw std::out_of_range("index out of range");
    }

    return m_tableData[index];
}


unsigned short ATSSimTable::getUShort(unsigned int index) const
{
    if ( index > (getTableSize()-1) )
    {
        throw std::out_of_range("index out of range");
    }

    const Byte * bval = &(m_tableData[index]);
    unsigned short * pval = (unsigned short *)((void *)bval);
    return *pval;
}


unsigned long ATSSimTable::getULong(unsigned int index) const
{
    if ( index > (getTableSize()-3) )
    {
        throw std::out_of_range("index out of range");
    }

    return 0;
}
#endif // #ifdef ATSSIMTABLE_USEFUL_ACCESSORS


#ifdef ATSSIMTABLE_USEFUL_MUTATORS
void ATSSimTable::setTableData(Byte value, unsigned int index)
{
    if (index>getTableSize())
    {
        throw std::out_of_range("index out of range");
    }

    throw std::logic_error("ATSSimTable::setTableData(Byte) not implemented");
}


void ATSSimTable::setTableData(unsigned short value, unsigned int index)
{
    if (index>getTableSize())
    {
        throw std::out_of_range("index out of range");
    }

    throw std::logic_error("ATSSimTable::setTableData(unsigned short) not implemented");
}


void ATSSimTable::setTableData(unsigned long value, unsigned int index)
{
    if (index>getTableSize())
    {
        throw std::out_of_range("index out of range");
    }

    throw std::logic_error("ATSSimTable::setTableData(unsigned long) not implemented");
}


void ATSSimTable::setTableData(void * address, int bytes, unsigned int index)
{
    if (bytes<0)
    {
        bytes = getTableSize();
    }

    if ( (bytes+index) > getTableSize() )
    {
		m_tableData.reserve(bytes+index);
        // throw std::out_of_range("too many bytes queryed to copy");
    }

    int b;
    unsigned int i;
    ByteArray ba;
    for ( b=0, i=index, ba=(ByteArray)address ;
          b < bytes ;
          ++i, ++b )
    {
        m_tableData[i] = ba[i];
    }
}
#endif // #ifdef ATSSIMTABLE_USEFUL_MUTATORS


void ATSSimTable::setExceptionCode(Byte value)
{
    m_exceptionCode = value;
}

UserResponse * ATSSimTable::processUserRead(UserQuery * ur)
{
    return 0;
}


UserResponse * ATSSimTable::processUserWrite(UserQuery * ur)
{
    notifyDependents(user_modification);

    return 0;
}

void ATSSimTable::fromStream(const std::ostream& os)
{
    throw std::logic_error("ATSSimTable::fromStream(const std::ostream &) not implemented");
}

void ATSSimTable::toStream(std::ostream& os) const
{
    std::string s;
	ByteVector v;
	getByteVector(v);

    ATSSimUtility::data2string(v, s);

    os << s.c_str() << std::endl;
}

std::ostream &operator<< (std::ostream &os, const ATSSimTable &table)
{
    table.toStream(os);
    return os;
}

void ATSSimTable::addDependent(ATSSimTable * t)
{
    m_dependents.push_back(t);
}

void ATSSimTable::setupDependents()
{
}

void ATSSimTable::notifyDependents(ChangeReasonCode reasonCode)
{
    if (m_dependents.empty())
    {
        // shortcut out if nothing to do
        return;
    }
    
    for ( DependentTableContainer::iterator iter=m_dependents.begin() ;
          iter != m_dependents.end() ; iter++
        )
    {
        (*iter)->providerChanged(this, reasonCode);
    }
}

void ATSSimTable::providerChanged(ATSSimTable * t, ChangeReasonCode reasonCode)
{
}

void ATSSimTable::dependUpon(int tableAddress)
{
    m_parent->dependUponTable(this,tableAddress);
}

