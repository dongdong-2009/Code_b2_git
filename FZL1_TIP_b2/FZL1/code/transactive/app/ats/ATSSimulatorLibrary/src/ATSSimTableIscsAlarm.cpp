/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsAlarm.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  *
  * Definition file for class ATSSimTableIscsAtcData.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsAlarm.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

using namespace TA_IRS_App::ATS_Sim;

ATSSimTableIscsAtcData::ATSSimTableIscsAtcData(ATSSimTableQueryProcessor * parent, LocationType loc)
: ATSSimTable(IscsAtcData,parent, loc)
, m_tableData(0)
{
    switch ( loc )
    {
        case OCC:
            m_numberOfRecords=NumberOfOCCRecords;
            break;
        case MSS:
        case SS1:
        case SS2:
        case STATION:
            m_numberOfRecords=NumberOfMSSRecords;
            break;
        case DPT:
            m_numberOfRecords=NumberOfDPTRecords;
            break;
    }

    m_tableData=new ATSSimTableIscsAtcDataRecord[m_numberOfRecords];

    //
    // Initialise all fields to zero
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        m_tableData[i].physicalTrainNumber = 0;
        m_tableData[i].atcStatusReportCodeHeader = 0;
        for ( int bytenum=0 ; bytenum<NumberOfAtcBytes ; bytenum++ )
        {
            m_tableData[i].atcStatusReport[bytenum] = 0;
        }
    }
}


ATSSimTableIscsAtcData::~ATSSimTableIscsAtcData()
{
    delete [] m_tableData;
}


unsigned short ATSSimTableIscsAtcData::getTableSize() const
{
    return (m_numberOfRecords*26);
}



UserResponse * ATSSimTableIscsAtcData::processUserRead(UserQuery * ur)
{
    if (ur->getType() == Show)
    {
        UserResponse * response = new UserResponse(ur);

        // need to take a copy of the query range of records so that we might
        // populate it with the full complement if it is empty
        RecordRange record_range = ur->getRecords();
        if ( record_range.empty() )
        {
            ATSSimUtility::expandRange("1-", record_range, getNumberOfRecords());
        }


        if (ur->getOutputFormat() == f_tabular)
        {
            response->m_responseData = "+-----+----------+----+----------------------------------------------+\n";
            response->m_responseData+= "|     | Physical |Code|                                              |\n";
            response->m_responseData+= "|  #  |  Train   |Hea-|        ATC train-borne status report         |\n";
            response->m_responseData+= "|     |  Number  | der|                                              |\n";
            response->m_responseData+= "+-----+----------+----+----------------------------------------------+\n";
        }


        if (record_range.size() > 0)
        {
            if (ur->getOutputFormat() == f_hex)
            {
                // pre-flight std::string memory-handling performance-enhancement
                // the '2' is for the characters that each byte takes in hex
                response->m_responseData.reserve(2 * sizeof(ATSSimTableIscsAtcDataRecord) * record_range.size() );
            }

            unsigned int i=0;
            for ( RecordRange::const_iterator record_iter = record_range.begin() ;
                  record_iter != record_range.end() ;
                  record_iter++
                )
            {
                i = (*record_iter)-1;

                if (ur->getOutputFormat() == f_tabular)
                {
                    char buffStr[16] = {0};

                    sprintf(buffStr, "| %3u ", i+1);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "| %8u ", m_tableData[i].physicalTrainNumber);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "| %.2x | ", m_tableData[i].atcStatusReportCodeHeader);
                    response->m_responseData.append(buffStr);

                    for ( int x=0 ; x<NumberOfAtcBytes ; x++ )
                    {
                        sprintf(buffStr,"%.2X",m_tableData[i].atcStatusReport[x]);
                        response->m_responseData.append(buffStr);
                    }

                    response->m_responseData.append(" |\n");
                }
                else if (ur->getOutputFormat() == f_hex)
                {
                    char buffStr[16] = {0};

                    sprintf(buffStr, "%.4X", m_tableData[i].physicalTrainNumber);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "%.2x", m_tableData[i].atcStatusReportCodeHeader);
                    response->m_responseData.append(buffStr);

                    for ( int x=0 ; x<NumberOfAtcBytes ; x++ )
                    {
                        sprintf(buffStr,"%.2X",m_tableData[i].atcStatusReport[x]);
                        response->m_responseData.append(buffStr);
                    }
                    // spare byte at the end of each record
                    response->m_responseData.append("00");
                }
            }
        }

        if (ur->getOutputFormat() == f_tabular)
        {
            response->m_responseData+= "+-----+----------+----+----------------------------------------------+";
        }
        response->m_responseData+= "\n";

        return response;
    }

    return 0;
}


UserResponse * ATSSimTableIscsAtcData::processUserWrite(UserQuery * ur)
{
    return 0;
}


void ATSSimTableIscsAtcData::toStream(std::ostream& os) const
{
}


void ATSSimTableIscsAtcData::fromStream(const std::ostream& os)
{
}


void ATSSimTableIscsAtcData::toNetwork(ByteVector & n)
{
}


void ATSSimTableIscsAtcData::fromNetwork(const ByteVector & s)
{
    int x=0;
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        ATS_TABLE_GET(Word,physicalTrainNumber);
        ATS_TABLE_GET(Byte,atcStatusReportCodeHeader);
        for ( int j=0 ; j<NumberOfAtcBytes ; j++ )
        {
            m_tableData[i].atcStatusReport[j]=ATSSimUtility::getByte(s,x);
        }
        ATS_TABLE_GET_SPARE(Byte);
    }
}


void ATSSimTableIscsAtcData::getByteVector(ByteVector & bv) const
{
}

const int ATSSimTableIscsAtcData::NumberOfOCCRecords=99;
const int ATSSimTableIscsAtcData::NumberOfMSSRecords=10;
const int ATSSimTableIscsAtcData::NumberOfDPTRecords=99;
