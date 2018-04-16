/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTablePAWash.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *
  * Definition file for class PASimTablePAWash.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#pragma warning (disable : 4244)
#endif // #ifdef WIN32

#include "app/PA/PASimulatorLibrary/src/PASimTablePAWash.h"
#include "app/PA/PASimulatorLibrary/src/PASimUtility.h"
#include "app/PA/PASimulatorLibrary/src/PASimUserProtocol.h"

#include <stdexcept>
#include <iostream>

using namespace TA_IRS_App::PA_Sim;

PASimTablePAWash::PASimTablePAWash(PASimTableQueryProcessor * parent, LocationType loc)
: PASimTable(PAWash,parent, loc)
{
    if (loc != DPT)
    {
        throw std::logic_error("PA Train Wash Plant table only valid at DPT");
    }

    m_numberOfRecords = 1;

    m_tableData[0].validityField = Irrelevant;
    m_tableData[0].noWashControl = 0;
    m_tableData[0].dailyRinseWashControl = 0;
    m_tableData[0].dailyDetergentWashControl = 0;
    m_tableData[0].weeklyWashControl = 0;
    m_tableData[0].trainStoppedForFacesWash = 0;
    m_tableData[0].stopWPControl = 0;
    m_tableData[0].restartWPControl = 0;
}


PASimTablePAWash::~PASimTablePAWash()
{
}


unsigned short PASimTablePAWash::getTableSize() const
{
    return (m_numberOfRecords * 8);
}



UserResponse * PASimTablePAWash::processUserRead(UserQuery * qry)
{
    if (qry->getType() == Show)
    {
        UserResponse * response = new UserResponse(qry);

        // Note: there is only one Wash table record, so we ignore the UserQuery

        if (qry->getOutputFormat() == f_tabular)
        {
            char buffStr[64];

            std::string interline = "+----------------------------+-----+\n";
            std::string boolstr = "";
            response->m_responseData.append(interline);

            PASimUtility::bool2string( (m_tableData[0].validityField==Relevant), boolstr, PASimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|             Validity field | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            PASimUtility::bool2string( (m_tableData[0].noWashControl!=0), boolstr, PASimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                    No Wash | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            PASimUtility::bool2string( (m_tableData[0].dailyRinseWashControl!=0), boolstr, PASimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                Daily Rinse | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            PASimUtility::bool2string( (m_tableData[0].dailyDetergentWashControl!=0), boolstr, PASimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|            Daily Detergent | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            PASimUtility::bool2string( (m_tableData[0].weeklyWashControl!=0), boolstr, PASimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                Weekly Wash | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            PASimUtility::bool2string( (m_tableData[0].trainStoppedForFacesWash!=0), boolstr, PASimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Train Stopped 4 Faces Wash | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            PASimUtility::bool2string( (m_tableData[0].stopWPControl!=0), boolstr, PASimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|    Stop Wash Plant Request | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            PASimUtility::bool2string( (m_tableData[0].restartWPControl!=0), boolstr, PASimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Restart Wash Plant Request | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);
        }
        else if (qry->getOutputFormat() == f_hex)
        {
            response->m_responseData.append("ERROR: Hex format of PAWash table not supported\n");
        }

        return response;
    }
    return 0;
}


UserResponse * PASimTablePAWash::processUserWrite(UserQuery * qry)
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
                    m_tableData[record].validityField = (DataValidity)(value!=0);
                    std::cout << "Modified field=" << field << " value=" << (value!=0) << std::endl;
                    break;

                case 1:
                    m_tableData[record].noWashControl = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    //m_tableData[record].validityField = Relevant;
                    break;

                case 2:
                    m_tableData[record].dailyRinseWashControl = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    //m_tableData[record].validityField = Relevant;
                    break;

                case 3:
                    m_tableData[record].dailyDetergentWashControl = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    //m_tableData[record].validityField = Relevant;
                    break;

                case 4:
                    m_tableData[record].weeklyWashControl = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    //m_tableData[record].validityField = Relevant;
                    break;

                case 5:
                    m_tableData[record].trainStoppedForFacesWash = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    //m_tableData[record].validityField = Relevant;
                    break;

                case 6:
                    m_tableData[record].stopWPControl = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    //m_tableData[record].validityField = Relevant;
                    break;

                case 7:
                    m_tableData[record].restartWPControl = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    //m_tableData[record].validityField = Relevant;
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


void PASimTablePAWash::toStream(std::ostream& os) const
{
}


void PASimTablePAWash::fromStream(const std::ostream& os)
{
}


void PASimTablePAWash::toNetwork(ByteVector & n)
{
    for ( int i=0 ; i<1 ; i++ )
    {
        PA_TABLE_PUT(Byte,validityField);
        PA_TABLE_PUT(Byte,noWashControl);
        PA_TABLE_PUT(Byte,dailyRinseWashControl);
        PA_TABLE_PUT(Byte,dailyDetergentWashControl);
        PA_TABLE_PUT(Byte,weeklyWashControl);
        PA_TABLE_PUT(Byte,trainStoppedForFacesWash);
        PA_TABLE_PUT(Byte,stopWPControl);
        PA_TABLE_PUT(Byte,restartWPControl);
    }
}


void PASimTablePAWash::fromNetwork(const ByteVector & s)
{
}


void PASimTablePAWash::getByteVector(ByteVector & bv) const
{
}

