/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTableIscsTrain.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *
  * Definition file for class PASimTableIscsTrain.
  */
#include "app/PA/PASimulatorLibrary/src/PASimTableIscsTrain.h"
#include "app/PA/PASimulatorLibrary/src/PASimUtility.h"
#include "app/PA/PASimulatorLibrary/src/PASimUserProtocol.h"

using namespace TA_IRS_App::PA_Sim;

PASimTableIscsTrain::PASimTableIscsTrain(PASimTableQueryProcessor * parent, LocationType loc)
: PASimTable(IscsTrain,parent, loc)
, m_tableData(0)
{
    switch ( loc )
    {
        case OCC:
            m_numberOfRecords=99;
            break;
        case MSS:
        case SS1:
        case SS2:
        case STATION:
            m_numberOfRecords=10;
            break;
        case DPT:
            m_numberOfRecords=99;
            break;
    }

    m_tableData=new PASimTableIscsTrainRecord[m_numberOfRecords];

    //
    // Initialise all fields to zero
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        m_tableData[i].physicalTrainNumber = 0;
        m_tableData[i].validityStatus = Irrelevant;
        for ( int bytenum=0 ; bytenum<NumberOfOA1Bytes ; bytenum++ )
        {
            m_tableData[i].oa1[bytenum] = 0;
        }
    }
}


PASimTableIscsTrain::~PASimTableIscsTrain()
{
    delete [] m_tableData;
}


unsigned short PASimTableIscsTrain::getTableSize() const
{
    return (m_numberOfRecords*28);
}



UserResponse * PASimTableIscsTrain::processUserRead(UserQuery * ur)
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
            response->m_responseData = "+---+----------+----------+---------\n";
            response->m_responseData+= "|   | Physical |          |\n";
            response->m_responseData+= "| # |  Train   | Validity | oa1 Data\n";
            response->m_responseData+= "|   |  Number  |  status  |\n";
            response->m_responseData+= "+---+----------+----------+---------\n";
        }


        if (record_range.size() > 0)
        {
            if (ur->getOutputFormat() == f_hex)
            {
                // pre-flight std::string memory-handling performance-enhancement
                // the '2' is for the characters that each byte takes in hex
                response->m_responseData.reserve(2 * sizeof(PASimTableIscsTrainRecord) * record_range.size() );
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
                    char buffStr[16];

                    sprintf(buffStr, "|%.3u", i+1);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "| %8u ", m_tableData[i].physicalTrainNumber);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "|   %.4X   | ", m_tableData[i].validityStatus);
                    response->m_responseData.append(buffStr);

                    for ( int x=0 ; x<NumberOfOA1Bytes ; x++ )
                    {
                        sprintf(buffStr,"%.2X",m_tableData[i].oa1[x]);
                        response->m_responseData.append(buffStr);
                    }
                    response->m_responseData.append("\n");
                }
                else if (ur->getOutputFormat() == f_hex)
                {
                    char buffStr[16];

                    sprintf(buffStr, "%.4X", m_tableData[i].physicalTrainNumber);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "%.4X", (int)m_tableData[i].validityStatus);
                    response->m_responseData.append(buffStr);

                    for ( int x=0 ; x<NumberOfOA1Bytes ; x++ )
                    {
                        sprintf(buffStr,"%.2X",m_tableData[i].oa1[x]);
                        response->m_responseData.append(buffStr);
                    }
                }
            }
        }

        if (ur->getOutputFormat() == f_tabular)
        {
            response->m_responseData+= "+---+----------+----------+---------";
        }
        response->m_responseData+= "\n";

        return response;
    }

    return 0;
}


UserResponse * PASimTableIscsTrain::processUserWrite(UserQuery * ur)
{
    return 0;
}


void PASimTableIscsTrain::toStream(std::ostream& os) const
{
}


void PASimTableIscsTrain::fromStream(const std::ostream& os)
{
}


void PASimTableIscsTrain::toNetwork(ByteVector & n)
{
}


void PASimTableIscsTrain::fromNetwork(const ByteVector & s)
{
    int x=0;
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        PA_TABLE_GET(Word,physicalTrainNumber);
        PA_TABLE_GET_SPARE(Byte);
        m_tableData[i].validityStatus = (DataValidity)PASimUtility::getByte(s,x);
        for ( int j=0 ; j<NumberOfOA1Bytes ; j++ )
        {
            m_tableData[i].oa1[j]=PASimUtility::getByte(s,x);
        }
    }
}


void PASimTableIscsTrain::getByteVector(ByteVector & bv) const
{
}

