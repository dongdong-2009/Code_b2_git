/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsPlatform.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *
  * Definition file for class ATSSimTableAtsPlatform.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#pragma warning (disable : 4244)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsPlatform.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimFlexiMask.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsPlatform.h"

#include <stdexcept>
#include <iostream>

#include "core/utilities/src/DebugUtil.h"

// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

ATSSimTableAtsPlatform::ATSSimTableAtsPlatform( ATSSimTableQueryProcessor * parent, LocationType loc )
: ATSSimTable(AtsPlatform,parent, loc)
, m_recordSize(0)
{
    switch (loc)
    {
        case OCC:
            m_numberOfRecords = 100;
            m_recordSize = 54;
            break;
        case MSS:
        case SS1:
        case SS2:
        case STATION:
            m_numberOfRecords=20;
            m_recordSize = 58;
            break;
        default:
            m_numberOfRecords=0;
            break;
    }

    if (m_numberOfRecords>0)
    {
        m_tableData = new ATSSimTableAtsPlatformRecord[m_numberOfRecords];
    }
    else
    {
        throw std::logic_error("No Ats Train table at given location");
    }

    for ( unsigned int i=0; i<m_numberOfRecords ; i++ )
    {
        m_tableData[i].platformAreaId = 0;
        m_tableData[i].prearrivalEventStatus = 0;
        m_tableData[i].arrivalEventStatus = 0;

        m_tableData[i].predepartureEventStatus = 0;
        m_tableData[i].departureEventStatus = 0;
        m_tableData[i].skipEventStatus = 0;
        m_tableData[i].holdEventStatus = 0;

        m_tableData[i].trainEvacuationStatus = 0;
        m_tableData[i].overrunEventStatus = 0;

        m_tableData[i].firstNextTrain.nextTrainDataValidity = Irrelevant;
        m_tableData[i].firstNextTrain.platformType = 0;
        m_tableData[i].firstNextTrain.physicalNumber = 0;
        m_tableData[i].firstNextTrain.serviceNumber = 0;
        m_tableData[i].firstNextTrain.destinationAreaId = 0; // Odd is IB -> CVC Station, Even is OB -> DBG Station
        m_tableData[i].firstNextTrain.lastTrainOfTheOperatingDay = false;
        m_tableData[i].firstNextTrain.year = 0;
        m_tableData[i].firstNextTrain.month = 0;
        m_tableData[i].firstNextTrain.day = 0;
        m_tableData[i].firstNextTrain.hour = 0;
        m_tableData[i].firstNextTrain.minutes = 0;
        m_tableData[i].firstNextTrain.seconds = 0;

        m_tableData[i].secondNextTrain.nextTrainDataValidity = Irrelevant;
        m_tableData[i].secondNextTrain.platformType = 0;
        m_tableData[i].secondNextTrain.physicalNumber = 0;
        m_tableData[i].secondNextTrain.serviceNumber = 0;
        m_tableData[i].secondNextTrain.destinationAreaId = 0;
        m_tableData[i].secondNextTrain.lastTrainOfTheOperatingDay = 0;
        m_tableData[i].secondNextTrain.year = 0;
        m_tableData[i].secondNextTrain.month = 0;
        m_tableData[i].secondNextTrain.day = 0;
        m_tableData[i].secondNextTrain.hour = 0;
        m_tableData[i].secondNextTrain.minutes = 0;
        m_tableData[i].secondNextTrain.seconds = 0;

        m_tableData[i].psdClosePreWarning = 0;
        m_tableData[i].psdClosedAndLocked = 0;
        m_tableData[i].psdDoorUninhibitionRequest = 0;
    }
}


ATSSimTableAtsPlatform::~ATSSimTableAtsPlatform()
{
    delete [] m_tableData;
}


unsigned short ATSSimTableAtsPlatform::getTableSize() const
{
    return (m_numberOfRecords * m_recordSize);
}



UserResponse * ATSSimTableAtsPlatform::processUserRead(UserQuery * qry)
{
    if (qry->getType() == Show)
    {
        UserResponse * response = new UserResponse(qry);

        unsigned long record = *(qry->getRecords().begin()) - 1;

        if (qry->getOutputFormat() == f_tabular)
        {
            // pre-flight std::string memory-handling for performance-enhancement
            response->m_responseData.reserve(40 /* size of each line */ * (38 /* number of lines */ + 1 /* 2B sure */) );

            char buffStr[64];

            std::string interline = "+----------------------------+--------+\n";
            std::string boolstr = "";
            response->m_responseData.append(interline);

            sprintf(buffStr, "| Platform Aread Id          | %.5d  |\n", m_tableData[record].platformAreaId );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[record].prearrivalEventStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Pre-Arrival Event Status   | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[record].arrivalEventStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Arrival Event Status       | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[record].predepartureEventStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Pre-Departure Event Status | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[record].departureEventStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Departure Event Status     | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[record].skipEventStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Skip Event Status          | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[record].holdEventStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Hold Event Status          | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[record].trainEvacuationStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Train Evacuation Status    | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);

            ATSSimUtility::bool2string( (m_tableData[record].overrunEventStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Overrun Event Status       | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);

            response->m_responseData.append(interline);
            response->m_responseData.append("| First Next Train Info:     |        |\n");

            ATSSimUtility::bool2string( (m_tableData[record].firstNextTrain.nextTrainDataValidity==Relevant), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Valid                      | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Platform Type              | %s |\n", (m_tableData[record].firstNextTrain.platformType==0) ? "Arrive" : "Depart" );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Physical Number            | %.5u  |\n", m_tableData[record].firstNextTrain.physicalNumber );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Service Number             | %.5u  |\n", m_tableData[record].firstNextTrain.serviceNumber );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Destination Area Id        | %.5u  |\n", m_tableData[record].firstNextTrain.destinationAreaId );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[record].firstNextTrain.lastTrainOfTheOperatingDay), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Last Train of Oper'ing Day | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Date                       | %.2d%.2d%.2d |\n", m_tableData[record].firstNextTrain.year, m_tableData[record].firstNextTrain.month, m_tableData[record].firstNextTrain.day );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Time                       | %.2d%.2d%.2d |\n", m_tableData[record].firstNextTrain.hour, m_tableData[record].firstNextTrain.minutes, m_tableData[record].firstNextTrain.seconds );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            response->m_responseData.append(interline);
            response->m_responseData.append("| Second Next Train Info:    |        |\n");

            ATSSimUtility::bool2string( (m_tableData[record].secondNextTrain.nextTrainDataValidity==Relevant), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Valid                      | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Platform Type              | %s |\n", (m_tableData[record].secondNextTrain.platformType==0) ? "Arrive" : "Depart" );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Physical Number            | %.5u  |\n", m_tableData[record].secondNextTrain.physicalNumber );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Service Number             | %.5u  |\n", m_tableData[record].secondNextTrain.serviceNumber );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Destination Area Id        | %.5u  |\n", m_tableData[record].secondNextTrain.destinationAreaId );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            ATSSimUtility::bool2string( (m_tableData[record].secondNextTrain.lastTrainOfTheOperatingDay), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "| Last Train of Oper'ing Day | %s    |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Date                       | %.2d%.2d%.2d |\n", m_tableData[record].secondNextTrain.year, m_tableData[record].secondNextTrain.month, m_tableData[record].secondNextTrain.day );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            sprintf(buffStr, "| Time                       | %.2d%.2d%.2d |\n", m_tableData[record].secondNextTrain.hour, m_tableData[record].secondNextTrain.minutes, m_tableData[record].secondNextTrain.seconds );
            response->m_responseData.append(buffStr);
            // response->m_responseData.append(interline);

            if ( getLocation() != OCC )
            {
                response->m_responseData.append(interline);
                response->m_responseData.append("| Platform Screen Doors :    |        |\n");

                response->m_responseData.append( "| PSD Close Pre-Warning      | " );
                switch (m_tableData[record].psdClosePreWarning)
                {
                    case 0:
                        response->m_responseData.append("Start  |\n");
                        break;
                    case 1:
                        response->m_responseData.append("End    |\n");
                        break;
                    default:
                        sprintf(buffStr, "(%.3u)  |\n", m_tableData[record].psdClosePreWarning );
                        response->m_responseData.append(buffStr);
                        break;
                }

                ATSSimUtility::bool2string( ((m_tableData[record].psdClosedAndLocked & 0x02)!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf( buffStr, "| PSD Closed and Locked A    | %s    |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                // response->m_responseData.append(interline);

                ATSSimUtility::bool2string( ((m_tableData[record].psdClosedAndLocked & 0x01)!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf( buffStr, "| PSD Closed and Locked B    | %s    |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                // response->m_responseData.append(interline);

                response->m_responseData.append("| PSD Inhibit Request P1-P6  | ");
                Word psd=m_tableData[record].psdDoorUninhibitionRequest;
                Word mask=0x2000;
                boolstr = "";
                for (int foo=0; foo<6 ; foo++,mask>>=1)
                {
                    if ( mask & psd )
                    {
                        boolstr.append("1");
                    }
                    else
                    {
                        boolstr.append("0");
                    }
                }
                response->m_responseData.append(boolstr);
                response->m_responseData.append(" |\n");

                response->m_responseData.append("| PSD Inhibit Request P7-P12 | ");
                mask=0x0020;
                boolstr = "";
                for (int fooi=0; fooi<6 ; fooi++,mask>>=1)
                {
                    if ( mask & psd )
                    {
                        boolstr.append("1");
                    }
                    else
                    {
                        boolstr.append("0");
                    }
                }
                response->m_responseData.append(boolstr);
                response->m_responseData.append(" |\n");

            }

            response->m_responseData.append(interline);
        }
        else if (qry->getOutputFormat() == f_hex)
        {
            response->m_responseData.append("ERROR: Hex format of AtsPlatform table not supported\n");
        }

        return response;
    }

    return 0;
}


UserResponse * ATSSimTableAtsPlatform::processUserWrite(UserQuery * qry)
{
    if (qry->getType() == Set)
    {
        UserResponse * response = new UserResponse(qry);

        const RecordRange & record_range = qry->getRecords();
        if ( record_range.size() != 1 )
        {
            response->m_responseData = "WARNING: only one record should be specified. Using first record.";
        }

        unsigned long record = (*(record_range.begin()))-1;
        if (record >= getNumberOfRecords())
        {
            response->m_responseData+="ERROR: record number out of range";
            return response;
        }

        typedef FlexiMask<Word> WordMask;
		WordMask mask;


        while (qry->moreArgs())
        {
            UserQuery::FieldValuePair fvp = qry->popArg();

            unsigned long field = fvp.first;
			UserQuery::FieldValueType value = fvp.second;

            switch (field)
            {
                case 0:
                    m_tableData[record].platformAreaId = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 2:
                    m_tableData[record].prearrivalEventStatus = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 3:
                    m_tableData[record].arrivalEventStatus = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 4:
                    m_tableData[record].predepartureEventStatus = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 5:
                    m_tableData[record].departureEventStatus = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 6:
                    m_tableData[record].skipEventStatus = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 7:
                    m_tableData[record].holdEventStatus = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 8:
                    m_tableData[record].trainEvacuationStatus = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 9:
                    m_tableData[record].overrunEventStatus = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 10:
                    m_tableData[record].firstNextTrain.nextTrainDataValidity = (DataValidity)(value!=0);
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 11:
                    m_tableData[record].firstNextTrain.platformType = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 12:
                    m_tableData[record].firstNextTrain.physicalNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 14:
                    m_tableData[record].firstNextTrain.serviceNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 16:
                    m_tableData[record].firstNextTrain.destinationAreaId = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 19:
                    m_tableData[record].firstNextTrain.lastTrainOfTheOperatingDay = (value!=0);
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 20:
                    m_tableData[record].firstNextTrain.year = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 22:
                    m_tableData[record].firstNextTrain.month = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 24:
                    m_tableData[record].firstNextTrain.day = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 26:
                    m_tableData[record].firstNextTrain.hour = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 28:
                    m_tableData[record].firstNextTrain.minutes = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 30:
                    m_tableData[record].firstNextTrain.seconds = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 32:
                    m_tableData[record].secondNextTrain.nextTrainDataValidity = (DataValidity)(value!=0);
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 33:
                    m_tableData[record].secondNextTrain.platformType = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 34:
                    m_tableData[record].secondNextTrain.physicalNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 36:
                    m_tableData[record].secondNextTrain.serviceNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 38:
                    m_tableData[record].secondNextTrain.destinationAreaId = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 40:
                    m_tableData[record].secondNextTrain.lastTrainOfTheOperatingDay = (value!=0);
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 42:
                    m_tableData[record].secondNextTrain.year = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 44:
                    m_tableData[record].secondNextTrain.month = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 46:
                    m_tableData[record].secondNextTrain.day = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 48:
                    m_tableData[record].secondNextTrain.hour = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 50:
                    m_tableData[record].secondNextTrain.minutes = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 52:
                    m_tableData[record].secondNextTrain.seconds = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;
                    break;

                case 54:
                    m_tableData[record].psdClosePreWarning = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 55:
                    m_tableData[record].psdClosedAndLocked = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 56:
                    if (mask.isUsed())
                    {
                        m_tableData[record].psdDoorUninhibitionRequest = mask.apply(m_tableData[record].psdDoorUninhibitionRequest);
                        std::cout << "Applied mask to field=" << field << " mask=" << mask << std::endl;;
                        mask.reset();
                    }
                    else // mask is notUsed
                    {
                        m_tableData[record].psdDoorUninhibitionRequest = value;
                        std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    }
                    break;

                case MaskFieldIndicator:
					mask.setValueMask(value);
                    std::cout << "Set value for mask=" << mask << std::endl;;
                    break;
                case ToggleMaskFieldIndicator:
					mask.setToggleMask(value);
                    std::cout << "Set toggle for mask=" << mask << std::endl;;
                    break;
                case LeaveMaskFieldIndicator:
					mask.setLeaveMask(value);
                    std::cout << "Set leave for mask=" << mask << std::endl;;
                    break;
                case MaskEndFieldIndicator:
                    std::cout << "Mask fields ended. mask=" << mask << std::endl;;
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

void ATSSimTableAtsPlatform::setPsdClosedAndLocked(Word platformAreaId, Byte value )
{
    // search for the correct platform
    for ( unsigned int record=0 ; record<getNumberOfRecords() ; record++ )
    {
        if ( m_tableData[record].platformAreaId == platformAreaId )
        {
            m_tableData[record].psdClosedAndLocked = value;
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "PSD Closed and Locked set to 0x%0.2x for platform area id=%u", value, platformAreaId);
            return;
        }
    }

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
        "Platform area id=%u not found for setting PSD closed and locked", platformAreaId);
}


void ATSSimTableAtsPlatform::toStream(std::ostream& os) const
{
}


void ATSSimTableAtsPlatform::fromStream(const std::ostream& os)
{
}


void ATSSimTableAtsPlatform::toNetwork(ByteVector & n)
{
    for ( unsigned int i=0; i<m_numberOfRecords ; i++ )
    {
        ATS_TABLE_PUT(Word,platformAreaId);
        ATS_TABLE_PUT(Byte,prearrivalEventStatus);
        ATS_TABLE_PUT(Byte,arrivalEventStatus);

        ATS_TABLE_PUT(Byte,predepartureEventStatus);
        ATS_TABLE_PUT(Byte,departureEventStatus);
        ATS_TABLE_PUT(Byte,skipEventStatus);
        ATS_TABLE_PUT(Byte,holdEventStatus);

        ATS_TABLE_PUT(Byte,trainEvacuationStatus);
        ATS_TABLE_PUT(Byte,overrunEventStatus);

        ATS_TABLE_PUT(Byte,firstNextTrain.nextTrainDataValidity);
        ATS_TABLE_PUT(Byte,firstNextTrain.platformType);
        ATS_TABLE_PUT(Word,firstNextTrain.physicalNumber);
        ATS_TABLE_PUT(Word,firstNextTrain.serviceNumber);
        ATS_TABLE_PUT(Word,firstNextTrain.destinationAreaId);
        ATS_TABLE_PUT_SPARE(Byte);
        ATS_TABLE_PUT(Byte,firstNextTrain.lastTrainOfTheOperatingDay);
        ATS_TABLE_PUT(Word,firstNextTrain.year);
        ATS_TABLE_PUT(Word,firstNextTrain.month);
        ATS_TABLE_PUT(Word,firstNextTrain.day);
        ATS_TABLE_PUT(Word,firstNextTrain.hour);
        ATS_TABLE_PUT(Word,firstNextTrain.minutes);
        ATS_TABLE_PUT(Word,firstNextTrain.seconds);

        ATS_TABLE_PUT(Byte,secondNextTrain.nextTrainDataValidity);
        ATS_TABLE_PUT(Byte,secondNextTrain.platformType);
        ATS_TABLE_PUT(Word,secondNextTrain.physicalNumber);
        ATS_TABLE_PUT(Word,secondNextTrain.serviceNumber);
        ATS_TABLE_PUT(Word,secondNextTrain.destinationAreaId);
        ATS_TABLE_PUT_SPARE(Byte);
        ATS_TABLE_PUT(Byte,secondNextTrain.lastTrainOfTheOperatingDay);
        ATS_TABLE_PUT(Word,secondNextTrain.year);
        ATS_TABLE_PUT(Word,secondNextTrain.month);
        ATS_TABLE_PUT(Word,secondNextTrain.day);
        ATS_TABLE_PUT(Word,secondNextTrain.hour);
        ATS_TABLE_PUT(Word,secondNextTrain.minutes);
        ATS_TABLE_PUT(Word,secondNextTrain.seconds);

        LocationType l = getLocation();
        if (    ( l == SS1 ) || ( l == SS2 )
             || ( l == MSS ) || ( l == STATION )
           )
        {
            ATS_TABLE_PUT(Byte,psdClosePreWarning);
            ATS_TABLE_PUT(Byte,psdClosedAndLocked);
            ATS_TABLE_PUT(Word,psdDoorUninhibitionRequest);
        }
    }
}


void ATSSimTableAtsPlatform::fromNetwork(const ByteVector & s)
{
    throw std::logic_error("Ats Platorm OCC Table should not be streamed fromNetwork");
}


void ATSSimTableAtsPlatform::getByteVector(ByteVector & bv) const
{
    // TODO make toNetwork const everywhere!!
    // toNetwork(bv);
}
