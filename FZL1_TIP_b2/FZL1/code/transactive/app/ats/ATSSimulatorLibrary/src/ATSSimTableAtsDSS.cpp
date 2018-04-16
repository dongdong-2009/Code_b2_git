/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsDSS.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  *
  * Definition file for class ATSSimTableAtsDSS.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "core/utilities/src/RunParams.h"

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsDSS.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

#include <iostream>

// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

ATSSimTableAtsDSS::ATSSimTableAtsDSS(ATSSimTableQueryProcessor * parent, LocationType loc)
: ATSSimTable(AtsDSS,parent, loc)
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
        m_tableData = new ATSSimTableAtsDSSRecord[m_numberOfRecords];

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


ATSSimTableAtsDSS::~ATSSimTableAtsDSS()
{
    delete [] m_tableData;
}


unsigned short ATSSimTableAtsDSS::getTableSize() const
{
    return (m_numberOfRecords * 18);
}


UserResponse * ATSSimTableAtsDSS::processUserRead(UserQuery * ur)
{
    return 0;
}


UserResponse * ATSSimTableAtsDSS::processUserWrite(UserQuery * qry)
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
                    response->m_responseData+= ATSSimUtility::uitoa(field);
                    break;
            }
        }

        return response;
    }
    return 0;
}


void ATSSimTableAtsDSS::toStream(std::ostream& os) const
{
}


void ATSSimTableAtsDSS::fromStream(const std::ostream& os)
{
}


void ATSSimTableAtsDSS::toNetwork(ByteVector & n)
{
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        ATS_TABLE_PUT(Word,year);
        ATS_TABLE_PUT(Word,month);
        ATS_TABLE_PUT(Word,day);
        ATS_TABLE_PUT(Word,hour);
        ATS_TABLE_PUT(Word,minutes);
        ATS_TABLE_PUT(Word,seconds);
        ATS_TABLE_PUT(Word,dssEvent);
        ATS_TABLE_PUT(DWord,dssInput);
    }
}


void ATSSimTableAtsDSS::fromNetwork(const ByteVector & s)
{
}


void ATSSimTableAtsDSS::getByteVector(ByteVector & bv) const
{
    // TODO make toNetwork const everywhere!!
    // toNetwork(bv);
}

