/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsWash.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  *
  * Definition file for class ATSSimTableAtsWash.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#pragma warning (disable : 4244)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsWash.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

#include <stdexcept>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;

ATSSimTableAtsWash::ATSSimTableAtsWash(ATSSimTableQueryProcessor * parent, LocationType loc)
: ATSSimTable(AtsWash,parent, loc)
{
    if (loc != DPT)
    {
        throw std::logic_error("Ats Train Wash Plant table only valid at DPT");
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


ATSSimTableAtsWash::~ATSSimTableAtsWash()
{
}


unsigned short ATSSimTableAtsWash::getTableSize() const
{
    return (m_numberOfRecords * 8);
}



UserResponse * ATSSimTableAtsWash::processUserRead(UserQuery * qry)
{
    if (qry->getType() == Show)
    {
        UserResponse * response = new UserResponse(qry);

        // Note: there is only one Wash table record, so we ignore the UserQuery

        if (qry->getOutputFormat() == f_tabular)
        {
            char buffStr[64] = {0};

            std::string interline = "+----------------------------+-----+\n";
            std::string boolstr = "";
            response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[0].validityField==Relevant), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|             Validity field | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[0].noWashControl!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                    No Wash | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[0].dailyRinseWashControl!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                Daily Rinse | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[0].dailyDetergentWashControl!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|            Daily Detergent | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[0].weeklyWashControl!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                Weekly Wash | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[0].trainStoppedForFacesWash!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Train Stopped 4 Faces Wash | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[0].stopWPControl!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|    Stop Wash Plant Request | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[0].restartWPControl!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Restart Wash Plant Request | %s |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            response->m_responseData.append(interline);
        }
        else if (qry->getOutputFormat() == f_hex)
        {
            response->m_responseData.append("ERROR: Hex format of AtsWash table not supported\n");
        }

        return response;
    }
    return 0;
}


UserResponse * ATSSimTableAtsWash::processUserWrite(UserQuery * qry)
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
                    response->m_responseData+= ATSSimUtility::uitoa(field);
                    break;
            }
        }

        return response;
    }

    return 0;
}


void ATSSimTableAtsWash::toStream(std::ostream& os) const
{
}


void ATSSimTableAtsWash::fromStream(const std::ostream& os)
{
}


void ATSSimTableAtsWash::toNetwork(ByteVector & n)
{
    for ( int i=0 ; i<1 ; i++ )
    {
        ATS_TABLE_PUT(Byte,validityField);
        ATS_TABLE_PUT(Byte,noWashControl);
        ATS_TABLE_PUT(Byte,dailyRinseWashControl);
        ATS_TABLE_PUT(Byte,dailyDetergentWashControl);
        ATS_TABLE_PUT(Byte,weeklyWashControl);
        ATS_TABLE_PUT(Byte,trainStoppedForFacesWash);
        ATS_TABLE_PUT(Byte,stopWPControl);
        ATS_TABLE_PUT(Byte,restartWPControl);
    }
}


void ATSSimTableAtsWash::fromNetwork(const ByteVector & s)
{
}


void ATSSimTableAtsWash::getByteVector(ByteVector & bv) const
{
}

