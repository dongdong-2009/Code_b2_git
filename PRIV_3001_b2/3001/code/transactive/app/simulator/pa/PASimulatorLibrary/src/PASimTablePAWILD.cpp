/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTablePAWILD.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  * Definition file for class PASimTablePAWILD.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/PA/PASimulatorLibrary/src/PASimTablePAWILD.h"
#include "app/PA/PASimulatorLibrary/src/PASimUtility.h"
#include "app/PA/PASimulatorLibrary/src/PASimUserProtocol.h"
#include <stdexcept>
#include <iostream>

using namespace TA_IRS_App::PA_Sim;

PASimTablePAWILD::PASimTablePAWILD(PASimTableQueryProcessor * parent, LocationType loc)
: PASimTable(PAWILD,parent, loc)
{
    if (loc != OCC)
    {
        throw std::logic_error("PA WILD table only valid at OCC");
    }

    m_numberOfRecords = 5;

    if (m_numberOfRecords>0)
    {
        m_tableData = new PASimTablePAWILDRecord[m_numberOfRecords];

        for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
        {
            m_tableData[i].year = 0;
            m_tableData[i].month = 0;
            m_tableData[i].day = 0;
            m_tableData[i].hour = 0;
            m_tableData[i].minutes = 0;
            m_tableData[i].seconds = 0;
            m_tableData[i].physicalTrainNumber = 0;
            m_tableData[i].activeCabNumber = 0;
            m_tableData[i].headCabDirection = (HeadCabDirection)Unknown;
        }
    }
}


PASimTablePAWILD::~PASimTablePAWILD()
{
    delete[] m_tableData;
}


unsigned short PASimTablePAWILD::getTableSize() const
{
    return (m_numberOfRecords * 18);
}



UserResponse * PASimTablePAWILD::processUserRead(UserQuery * qry)
{
    return 0;
}


UserResponse * PASimTablePAWILD::processUserWrite(UserQuery * qry)
{
    if (qry->getType() == Set)
    {
        UserResponse * response = new UserResponse(qry);

        const RecordRange & record_range = qry->getRecords();
        if ( record_range.size() != 1 )
        {
            response->m_responseData = "WARNING: only one record should be specified. Using first record.";
        }

        unsigned short record = (*(record_range.begin()))-1;
        if ( (record < 0) || (record >= getNumberOfRecords()) )
        {
            response->m_responseData+="ERROR: record number out of range";
            return response;
        }

        while (qry->moreArgs())
        {
            UserQuery::FieldValuePair fvp = qry->popArg();

            unsigned long field = fvp.first;
			UserQuery::FieldValueType value = fvp.second;


            switch (field)
            {
                case 0:
                    m_tableData[record].year = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 2:
                    m_tableData[record].month = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 4:
                    m_tableData[record].day = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 6:
                    m_tableData[record].hour = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 8:
                    m_tableData[record].minutes = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 10:
                    m_tableData[record].seconds = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 12:
                    m_tableData[record].physicalTrainNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 14:
                    m_tableData[record].activeCabNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 17:
                    m_tableData[record].headCabDirection = (HeadCabDirection)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                default:
                    response->m_responseData+= "ERROR: invalid field byte number: ";
                    response->m_responseData+= PASimUtility::uitoa(value);
                    break;
            }
        }

        return response;
    }
    return 0;
}


void PASimTablePAWILD::toStream(std::ostream& os) const
{
}


void PASimTablePAWILD::fromStream(const std::ostream& os)
{
}


void PASimTablePAWILD::toNetwork(ByteVector & n)
{
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        PA_TABLE_PUT(Word,year);
        PA_TABLE_PUT(Word,month);
        PA_TABLE_PUT(Word,day);
        PA_TABLE_PUT(Word,hour);
        PA_TABLE_PUT(Word,minutes);
        PA_TABLE_PUT(Word,seconds);
        PA_TABLE_PUT(Word,physicalTrainNumber);
        PA_TABLE_PUT(Word,activeCabNumber);
        PA_TABLE_PUT_SPARE(Byte);
        PA_TABLE_PUT(Byte,headCabDirection);
    }
}


void PASimTablePAWILD::fromNetwork(const ByteVector & s)
{
    throw std::logic_error("PA WILD Table should not be put to vector fromNetwork");
}


void PASimTablePAWILD::getByteVector(ByteVector & bv) const
{
}

