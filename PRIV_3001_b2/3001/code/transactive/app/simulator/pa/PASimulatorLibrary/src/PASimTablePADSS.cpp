/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTablePADSS.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  * Definition file for class PASimTablePADSS.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "core/utilities/src/RunParams.h"

#include "app/PA/PASimulatorLibrary/src/PASimTablePADSS.h"
#include "app/PA/PASimulatorLibrary/src/PASimUtility.h"
#include "app/PA/PASimulatorLibrary/src/PASimUserProtocol.h"

#include <iostream>

// using namespace TA_IRS_App;
using namespace TA_IRS_App::PA_Sim;

PASimTablePADSS::PASimTablePADSS(PASimTableQueryProcessor * parent, LocationType loc)
: PASimTable(PADSS,parent, loc)
{
	if ( TA_Base_Core::RunParams::getInstance().get("Sim_type") == "OCC" )
	{
		m_numberOfRecords = 32;
	}
	else
	{
		m_numberOfRecords = 16;
	}    

    if (m_numberOfRecords > 0)
    {
        m_tableData = new PASimTablePADSSRecord[m_numberOfRecords];

        for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
        {
            m_tableData[i].year = 0;
            m_tableData[i].month = 0;
            m_tableData[i].day = 0;
            m_tableData[i].hour = 0;
            m_tableData[i].minutes = 0;
            m_tableData[i].seconds = 0;
            m_tableData[i].dssEvent = 0;
        }
    }
}


PASimTablePADSS::~PASimTablePADSS()
{
    delete [] m_tableData;
}


unsigned short PASimTablePADSS::getTableSize() const
{
    return (m_numberOfRecords * 18);
}


UserResponse * PASimTablePADSS::processUserRead(UserQuery * ur)
{
    return 0;
}


UserResponse * PASimTablePADSS::processUserWrite(UserQuery * qry)
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
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 2:
                    m_tableData[record].month = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 4:
                    m_tableData[record].day = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 6:
                    m_tableData[record].hour = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 8:
                    m_tableData[record].minutes = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 10:
                    m_tableData[record].seconds = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 12:
                    m_tableData[record].dssEvent = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

				case 14:
                    m_tableData[record].dssInput = value << 16;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;
				case 16:
                    m_tableData[record].dssInput |= value & 0xffff;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                default:
                    response->m_responseData+= "ERROR: invalid field byte number: ";
                    response->m_responseData+= PASimUtility::uitoa(field);
                    break;
            }
        }

        return response;
    }
    return 0;
}


void PASimTablePADSS::toStream(std::ostream& os) const
{
}


void PASimTablePADSS::fromStream(const std::ostream& os)
{
}


void PASimTablePADSS::toNetwork(ByteVector & n)
{
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        PA_TABLE_PUT(Word,year);
        PA_TABLE_PUT(Word,month);
        PA_TABLE_PUT(Word,day);
        PA_TABLE_PUT(Word,hour);
        PA_TABLE_PUT(Word,minutes);
        PA_TABLE_PUT(Word,seconds);
        PA_TABLE_PUT(Word,dssEvent);
        PA_TABLE_PUT(DWord,dssInput);
    }
}


void PASimTablePADSS::fromNetwork(const ByteVector & s)
{
}


void PASimTablePADSS::getByteVector(ByteVector & bv) const
{
    // TODO make toNetwork const everywhere!!
    // toNetwork(bv);
}

