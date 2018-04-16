/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTableIscsElectricalSubsection.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *
  * Definition file for class PASimTableIscsElectricalSubsection.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/PA/PASimulatorLibrary/src/PASimTableIscsElectricalSubsection.h"
#include "app/PA/PASimulatorLibrary/src/PASimUtility.h"
#include "app/PA/PASimulatorLibrary/src/PASimUserProtocol.h"

#include <stdexcept>

// using namespace TA_IRS_App;
using namespace TA_IRS_App::PA_Sim;

PASimTableIscsElectricalSubsection::PASimTableIscsElectricalSubsection(PASimTableQueryProcessor * parent, LocationType loc)
: PASimTable(IscsElectricalSubsection,parent, loc)
, m_tableData(0)
{
    switch ( loc )
    {
        case OCC:
            m_numberOfRecords=250;
            break;
        case MSS:
        case SS1:
        case SS2:
        case STATION:
            m_numberOfRecords=15;
            break;
        case DPT:
            throw std::logic_error("No ISCS Electrical Subsection table in DPT");
            break;
    }

    m_tableData=new PASimTableIscsElectricalSubsectionRecord[m_numberOfRecords];

    //
    // Initialise all fields to zero
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        m_tableData[i].subsectionId = 0;
        m_tableData[i].validityStatus = 0;
        m_tableData[i].powerStatus = 0;
    }
}


PASimTableIscsElectricalSubsection::~PASimTableIscsElectricalSubsection()
{
    delete [] m_tableData;
}



unsigned short PASimTableIscsElectricalSubsection::getTableSize() const
{
    return (m_numberOfRecords*4);
}


void PASimTableIscsElectricalSubsection::toStream(std::ostream& os) const
{
}


void PASimTableIscsElectricalSubsection::fromStream(const std::ostream& os)
{
}


void PASimTableIscsElectricalSubsection::toNetwork(ByteVector & n)
{
}


void PASimTableIscsElectricalSubsection::fromNetwork(const ByteVector & s)
{
    int x=0;
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        PA_TABLE_GET(Word,subsectionId);
        m_tableData[i].validityStatus = PASimUtility::getByte(s,x);
        PA_TABLE_GET(Byte,powerStatus);
    }
}


void PASimTableIscsElectricalSubsection::getByteVector(ByteVector & bv) const
{
}


UserResponse * PASimTableIscsElectricalSubsection::processUserRead(UserQuery * ur)
{
    if (ur->getType() == Show)
    {
        UserResponse * response = new UserResponse(ur);

        RecordRange record_range = ur->getRecords();
        if ( record_range.empty() )
        {
            PASimUtility::expandRange("1-", record_range, getNumberOfRecords());
        }


        if (ur->getOutputFormat() == f_tabular)
        {
            response->m_responseData = "+-----+------------+-------+---------------+\n";
            response->m_responseData+= "|     | Subsection | Valid | Power         |\n";
            response->m_responseData+= "|  #  |     Id     |       | Status        |\n";
            response->m_responseData+= "+-----+------------+-------+---------------+\n";
//                                      | 001 |   000011   |  Yes  | Not Energised |
//                                      | 002 |   000020   |  No   | Energised     |
//                                      | 003 |   000324   |       | Invalid (057) |
        }

        if (record_range.size() > 0)
        {
            if (ur->getOutputFormat() == f_hex)
            {
                // pre-flight std::string memory-handling performance-enhancement
                // the '2' is for the characters that each byte takes in hex
                response->m_responseData.reserve(2 * sizeof(PASimTableIscsElectricalSubsectionRecord) * record_range.size() );
            }

            std::string tmpstr="";
            char buffStr[51];
            unsigned int i=0;

            for ( RecordRange::const_iterator record_iter = record_range.begin() ;
                  record_iter != record_range.end() ;
                  record_iter++
                )
            {
                i = (*record_iter)-1;

                if (ur->getOutputFormat() == f_tabular)
                {
                    sprintf(buffStr, "| %.3u ", i+1);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "|   %.6u   ", m_tableData[i].subsectionId);
                    response->m_responseData.append(buffStr);

					if (m_tableData[i].validityStatus == 255)
                    {
                        response->m_responseData.append("|  Yes  ");
                    }
                    else
                    {
                        response->m_responseData.append("|  No   ");
                    }

                    if (m_tableData[i].powerStatus == 255)
                    {
                        response->m_responseData.append("| Energised     |\n");
                    }
                    else
                    {
                        sprintf(buffStr, "| Not En  (%.3u) |\n", m_tableData[i].powerStatus);
                        response->m_responseData.append(buffStr);
                    }
                }
                else if (ur->getOutputFormat() == f_hex)
                {
                    sprintf(buffStr, "%.4X", m_tableData[i].subsectionId);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "%.2X", m_tableData[i].validityStatus);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "%.2X", m_tableData[i].powerStatus);
                    response->m_responseData.append(buffStr);
                }
            }
        }

        if (ur->getOutputFormat() == f_tabular)
        {
            response->m_responseData+= "+-----+------------+-----------------------+";
        }
        response->m_responseData+= "\n";

        return response;
    }

    return 0;
}


UserResponse * PASimTableIscsElectricalSubsection::processUserWrite(UserQuery * ur)
{
    return 0;
}


