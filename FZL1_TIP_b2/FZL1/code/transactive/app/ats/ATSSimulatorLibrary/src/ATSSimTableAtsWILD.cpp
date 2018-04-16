/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsWILD.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  *
  * Definition file for class ATSSimTableAtsWILD.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsWILD.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include <stdexcept>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;

ATSSimTableAtsWILD::ATSSimTableAtsWILD(ATSSimTableQueryProcessor * parent, LocationType loc)
: ATSSimTable(AtsWILD,parent, loc)
{
    if (loc != OCC)
    {
        throw std::logic_error("Ats WILD table only valid at OCC");
    }

    m_numberOfRecords = 5;

    if (m_numberOfRecords>0)
    {
        m_tableData = new ATSSimTableAtsWILDRecord[m_numberOfRecords];

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


ATSSimTableAtsWILD::~ATSSimTableAtsWILD()
{
    delete[] m_tableData;
}


unsigned short ATSSimTableAtsWILD::getTableSize() const
{
    return (m_numberOfRecords * 18);
}



UserResponse * ATSSimTableAtsWILD::processUserRead(UserQuery * qry)
{
    return 0;
}


UserResponse * ATSSimTableAtsWILD::processUserWrite(UserQuery * qry)
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
                    response->m_responseData+= ATSSimUtility::uitoa(value);
                    break;
            }
        }

        return response;
    }
    return 0;
}


void ATSSimTableAtsWILD::toStream(std::ostream& os) const
{
}


void ATSSimTableAtsWILD::fromStream(const std::ostream& os)
{
}


void ATSSimTableAtsWILD::toNetwork(ByteVector & n)
{
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        ATS_TABLE_PUT(Word,year);
        ATS_TABLE_PUT(Word,month);
        ATS_TABLE_PUT(Word,day);
        ATS_TABLE_PUT(Word,hour);
        ATS_TABLE_PUT(Word,minutes);
        ATS_TABLE_PUT(Word,seconds);
        ATS_TABLE_PUT(Word,physicalTrainNumber);
        ATS_TABLE_PUT(Word,activeCabNumber);
        ATS_TABLE_PUT_SPARE(Byte);
        ATS_TABLE_PUT(Byte,headCabDirection);
    }
}


void ATSSimTableAtsWILD::fromNetwork(const ByteVector & s)
{
    throw std::logic_error("Ats WILD Table should not be put to vector fromNetwork");
}


void ATSSimTableAtsWILD::getByteVector(ByteVector & bv) const
{
}

