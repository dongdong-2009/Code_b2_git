/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsPlatform.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *
  * Definition file for class ATSSimTableIscsPlatform.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsPlatform.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsPlatform.h"
#include <stdexcept>

// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

ATSSimTableIscsPlatform::ATSSimTableIscsPlatform
(   ATSSimTableQueryProcessor * parent,
    LocationType loc,
    ATSSimTableAtsPlatform * atsPlatformTable)
: ATSSimTable(IscsPlatform,parent, loc)
, m_tableData(0)
, m_atsPlatformTable(atsPlatformTable)
{
    switch ( loc )
    {
        case OCC:
            throw std::logic_error("No Iscs Platform table at OCC");
            break;
        case MSS:
        case SS1:
        case SS2:
        case STATION:
            m_numberOfRecords=10;
            break;
        case DPT:
            throw std::logic_error("No Iscs Platform table at DEPOT");
            break;
    }

    m_tableData=new ATSSimTableIscsPlatformRecord[m_numberOfRecords];

    //
    // Initialise all fields to zero
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        m_tableData[i].platformAreaId = 0;
        m_tableData[i].closeRequest = 0;
        m_tableData[i].openRequest = 0;
        m_tableData[i].psdDoorStatus = 0;
    }
}


ATSSimTableIscsPlatform::~ATSSimTableIscsPlatform()
{
    delete [] m_tableData;
}


unsigned short ATSSimTableIscsPlatform::getTableSize() const
{
    return (m_numberOfRecords*6);
}



UserResponse * ATSSimTableIscsPlatform::processUserRead(UserQuery * ur)
{
    if (ur->getType() == Show)
    {
        UserResponse * response = new UserResponse(ur);

        RecordRange record_range = ur->getRecords();
        if ( record_range.empty() )
        {
            ATSSimUtility::expandRange("1-", record_range, getNumberOfRecords());
        }


        if (ur->getOutputFormat() == f_tabular)
        {
            response->m_responseData = "+---+----------+-------+-------+------------------+\n";
            response->m_responseData+= "|   | Platform | Close | Open  | PSD Door         |\n";
            response->m_responseData+= "| # | Area  Id | State | State | Status           |\n";
            response->m_responseData+= "+---+----------+-------+-------+------------------+\n";
        }

        if (record_range.size() > 0)
        {
            std::string tmpstr="";
            char buffStr[16];
            unsigned int i=0;

            if (ur->getOutputFormat() == f_hex)
            {
                // pre-flight std::string memory-handling performance-enhancement
                // the '2' is for the characters that each byte takes in hex
                response->m_responseData.reserve(2 * sizeof(ATSSimTableIscsPlatformRecord) * record_range.size() );
            }

            for ( RecordRange::const_iterator record_iter = record_range.begin() ;
                  record_iter != record_range.end() ;
                  record_iter++
                )
            {
                i = (*record_iter)-1;

                if (ur->getOutputFormat() == f_tabular)
                {
                    sprintf(buffStr, "|%.3u", i+1);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "| %.8u ", m_tableData[i].platformAreaId);
                    response->m_responseData.append(buffStr);

                    ATSSimUtility::bool2string( (m_tableData[i].closeRequest!=0), tmpstr, ATSSimUtility::booleanOutputYesNo );
                    sprintf(buffStr, "|  %s  ", tmpstr.c_str());
                    response->m_responseData.append(buffStr);

                    ATSSimUtility::bool2string( (m_tableData[i].openRequest!=0), tmpstr, ATSSimUtility::booleanOutputYesNo );
                    sprintf(buffStr, "|  %s  | ", tmpstr.c_str());
                    response->m_responseData.append(buffStr);

                    // ATSSimUtility::word2BinaryString( m_tableData[i].psdDoorStatus, tmpstr );
                    // in lieu of that binary string formatter,...
                    Word psd=m_tableData[i].psdDoorStatus;
                    {
                        tmpstr="";
                        Word mask=0x8000;
                        for (int foo=0; foo<16 ; foo++,mask>>=1)
                        {
                            if ( (foo==0) || (foo==1) || (foo==8) || (foo==9) )
                            {
                                tmpstr.append("-");
                            }
                            else if ( mask & psd )
                            {
                                tmpstr.append("1");
                            }
                            else
                            {
                                tmpstr.append("0");
                            }
                        }
                    }

                    sprintf(buffStr, "%s |\n", tmpstr.c_str());
                    response->m_responseData.append(buffStr);

                }
                else if (ur->getOutputFormat() == f_hex)
                {
                    sprintf(buffStr, "%.4X", m_tableData[i].platformAreaId);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "%.2X", m_tableData[i].closeRequest);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "%.2X", m_tableData[i].openRequest);
                    response->m_responseData.append(buffStr);

                    sprintf(buffStr, "%.4X", m_tableData[i].psdDoorStatus);
                    response->m_responseData.append(buffStr);
                }
            }
        }

        if (ur->getOutputFormat() == f_tabular)
        {
            response->m_responseData+= "+---+----------+-------+-------+------------------+";
        }
        response->m_responseData+= "\n";

        return response;
    }

    return 0;
}


UserResponse * ATSSimTableIscsPlatform::processUserWrite(UserQuery * ur)
{
    return 0;
}


void ATSSimTableIscsPlatform::toStream(std::ostream& os) const
{
}


void ATSSimTableIscsPlatform::fromStream(const std::ostream& os)
{
}


void ATSSimTableIscsPlatform::toNetwork(ByteVector & n)
{
}


void ATSSimTableIscsPlatform::fromNetwork(const ByteVector & s)
{
    int x=0;
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        ATS_TABLE_GET(Word,platformAreaId);

        ATS_TABLE_GET(Byte,closeRequest);
        if ( m_tableData[i].closeRequest==1 )
        {
            m_atsPlatformTable->setPsdClosedAndLocked(m_tableData[i].platformAreaId, 3);
        }
        
        
        ATS_TABLE_GET(Byte,openRequest);
        if ( m_tableData[i].openRequest==1 )
        {
            m_atsPlatformTable->setPsdClosedAndLocked(m_tableData[i].platformAreaId, 0);
        }

        ATS_TABLE_GET(Word,psdDoorStatus);
    }
}


void ATSSimTableIscsPlatform::getByteVector(ByteVector & bv) const
{
}
