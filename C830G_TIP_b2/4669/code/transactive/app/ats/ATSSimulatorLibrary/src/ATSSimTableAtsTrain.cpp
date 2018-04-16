/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsTrain.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  * Definition file for class ATSSimTableAtsTrain.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsTrain.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimFlexiMask.h"
#include <stdexcept>
#include <iostream>

// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

ATSSimTableAtsTrain::ATSSimTableAtsTrain(ATSSimTableQueryProcessor * parent, LocationType loc)
: ATSSimTable(AtsTrain,parent, loc)
, m_tableData(0)
{
    switch (loc)
    {
        case OCC:
            m_numberOfRecords=99;
            break;
        case MSS:
        case SS1:
        case SS2:
        case STATION:
            m_numberOfRecords=20;
            break;
        case DPT:
            m_numberOfRecords=99;
            break;
    }

    if (m_numberOfRecords>0)
    {
        m_tableData = new ATSSimTableAtsTrainRecord[m_numberOfRecords];
    }
    else
    {
        throw std::logic_error("No Ats Train table at given location");
    }

    //
    // Initialise all fields to zero
    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
        m_tableData[i].physicalTrainNumber = 0;
        m_tableData[i].atcTrainNumber = 0;

        // all these fields are irrelevant until otherwise specified
        m_tableData[i].trainDataValidity = Irrelevant;
        m_tableData[i].serviceNumber = 0;
        m_tableData[i].scheduleNumber = 0;
        m_tableData[i].branchNumber = 0;
        m_tableData[i].fbAbscissa = 0;
        m_tableData[i].headCarAbscissa = 0;
        m_tableData[i].headCabDirection = (HeadCabDirection)Unknown;
        m_tableData[i].talkativeStatus = Mute;
        m_tableData[i].holdStatus = NotHeld;
        m_tableData[i].platformAreaId = 0;
        m_tableData[i].trainReadinessMode = Shutdown;
        m_tableData[i].stalledStatus = NotStalled;
        m_tableData[i].evacuationStatus = NoEvacuation;
        m_tableData[i].ebStatus = EBNotActivated;

        // OA1 Data
        m_tableData[i].oa1DataValidity = Irrelevant;
        m_tableData[i].tetraMobileStatus = (TetraMobileStatus)Unknown;
        for ( int car=0 ; car<=2 ; car++ )
        {
            m_tableData[i].pecuStatusCar[car] = 0;
        }
        m_tableData[i].cctvCameraStatus = (CCTVCameraStatus)Unknown;
        m_tableData[i].cctvSystemEquipmentStatus = (CCTVEquipmentStatus)Unknown;
        m_tableData[i].trainPAStatus = (TrainPAStatus)Unknown;

        m_tableData[i].underSeatFireDetectionCar1 = (FireDetectionStatus)Unknown;
        m_tableData[i].underFrameFireDetectionCar1 = (FireDetectionStatus)Unknown;
        m_tableData[i].internalSmokeDetectionCar1 = (InteriorSmokeDetectionStatus)Unknown;
        m_tableData[i].externalSmokeDetectionCar1 = (ExteriorSmokeDetectionStatus)Unknown;
        m_tableData[i].underSeatFireDetectionCar2 = (FireDetectionStatus)Unknown;
        m_tableData[i].underFrameFireDetectionCar2 = (FireDetectionStatus)Unknown;
        m_tableData[i].internalSmokeDetectionCar2 = (InteriorSmokeDetectionStatus)Unknown;
        m_tableData[i].externalSmokeDetectionCar2 = (ExteriorSmokeDetectionStatus)Unknown;
        m_tableData[i].underSeatFireDetectionCar3 = (FireDetectionStatus)Unknown;
        m_tableData[i].underFrameFireDetectionCar3 = (FireDetectionStatus)Unknown;
        m_tableData[i].internalSmokeDetectionCar3 = (InteriorSmokeDetectionStatus)Unknown;
        m_tableData[i].externalSmokeDetectionCar3 = (ExteriorSmokeDetectionStatus)Unknown;

        m_tableData[i].externalTemperature = 0;

        for ( int car2=0 ; car2<=2 ; car2++ )
        {
            m_tableData[i].extinguisherStatusCar[car2] = (ExtinguisherStatus)Unknown;
            m_tableData[i].ehsStatusCar[car2] = 0;
        }

        m_tableData[i].detrainmentDoorStatusCar1 = (DetrainmentDoorStatus)Unknown;
        m_tableData[i].detrainmentDoorStatusCar3 = (DetrainmentDoorStatus)Unknown;


        // OA2 Data
        m_tableData[i].oa2DataValidity = Irrelevant;
        m_tableData[i].driverConsoleCoverStatusCar1 = (DriverConsoleCoverStatus)Unknown;
        m_tableData[i].driverConsoleCoverStatusCar3 = (DriverConsoleCoverStatus)Unknown;

        for ( int car3=0 ; car3<=2 ; car3++ ) //         Two doors per byte
        { //                                              VV
            for ( int door=0 ; door<=((NumberOfDoorsPerCar/2)-1) ; door++ )
            {
                m_tableData[i].trainDoorStatus[car3][door] = (TrainDoorStatus)Unknown;
            }

            m_tableData[i].emergencyHandleCoverStatusCar[car3] = (EmergencyHandleCoverStatus)Unknown;
            m_tableData[i].ttisStatusCar[car3] = (TTISStatus)Unknown;
        }

        m_tableData[i].year = 0;
        m_tableData[i].month = 0; //ATSSimUtility::randomWord(1,12);
        m_tableData[i].day = 0; //ATSSimUtility::randomWord(1,28);
        m_tableData[i].hour = 0; //ATSSimUtility::randomWord(0,23);
        m_tableData[i].minutes = 0; //ATSSimUtility::randomWord(0,59);
        m_tableData[i].seconds = 0; //ATSSimUtility::randomWord(0,59);
    }
}


ATSSimTableAtsTrain::~ATSSimTableAtsTrain()
{
    delete [] m_tableData;
}

unsigned short ATSSimTableAtsTrain::getTableSize() const
{
    return (m_numberOfRecords*96);
}



UserResponse * ATSSimTableAtsTrain::processUserRead(UserQuery * qry)
{
#if defined ATSSIMTABLEATSTRAIN_PROCESSUSERREAD_FIXED
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

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].physicalTrainNumber );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].atcTrainNumber );
            response->m_responseData.append(buffStr);


            // all these fields are irrelevant until otherwise specified
            ATSSimUtility::bool2string( (m_tableData[record].trainDataValidity==Relevant), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                            | %s     |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].serviceNumber );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].scheduleNumber );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].branchNumber );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].fbAbscissa );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].headCarAbscissa );
            response->m_responseData.append(buffStr);

            switch ( m_tableData[record].headCabDirection )
            {
                case DownDirection:
                    response->m_responseData.append( "| Head Cab Direction         | Down   |\n" );
                    break;
                case UpDirection:
                    response->m_responseData.append( "| Head Cab Direction         | Up     |\n" );
                    break;
                default:
                    sprintf(buffStr, "| Talktative Status          | #%.5d |\n", (int)(m_tableData[record].headCabDirection) );
                    response->m_responseData.append(buffStr);
                    break;
            }

            switch ( m_tableData[record].talkativeStatus )
            {
                case Mute:
                    response->m_responseData.append( "| Talktative Status          | Mute   |\n" );
                    break;
                case Talkative:
                    response->m_responseData.append( "| Talktative Status          | Talk   |\n" );
                    break;
                default:
                    sprintf(buffStr, "| Talktative Status          | #%.5d |\n", (int)m_tableData[record].talkativeStatus );
                    response->m_responseData.append(buffStr);
                    break;
            }

            ATSSimUtility::bool2string( (m_tableData[record].holdStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                            | %s     |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].platformAreaId );
            response->m_responseData.append(buffStr);

            ATSSimUtility::bool2string( (m_tableData[record].trainReadinessMode!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                            | %s     |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);

            ATSSimUtility::bool2string( (m_tableData[record].stalledStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                            | %s     |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);

            ATSSimUtility::bool2string( (m_tableData[record].evacuationStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                            | %s     |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);

            ATSSimUtility::bool2string( (m_tableData[record].ebStatus!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                            | %s     |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);

            // OA1 Data
            ATSSimUtility::bool2string( (m_tableData[record].oa1DataValidity==Relevant), boolstr, ATSSimUtility::booleanOutputYesNo );
            sprintf(buffStr, "|                            | %s     |\n", boolstr.c_str() );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].tetraMobileStatus );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", 
            for ( int car=0 ; car<=2 ; car++ )
            {
                m_tableData[record].pecuStatusCar[car]
            }
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].cctvCameraStatus );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].cctvSystemEquipmentStatus );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].trainPAStatus );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].underSeatFireDetectionCar1 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].underFrameFireDetectionCar1 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].internalSmokeDetectionCar1 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].externalSmokeDetectionCar1 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].underSeatFireDetectionCar2 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].underFrameFireDetectionCar2 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].internalSmokeDetectionCar2 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].externalSmokeDetectionCar2 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].underSeatFireDetectionCar3 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].underFrameFireDetectionCar3 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].internalSmokeDetectionCar3 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].externalSmokeDetectionCar3 );
            response->m_responseData.append(buffStr);


            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].externalTemperature );
            response->m_responseData.append(buffStr);


            sprintf(buffStr, "|                            | %.5d  |\n", 
            for ( car=0 ; car<=2 ; car++ )
            {
                m_tableData[record].extinguisherStatusCar[car]
            }
            response->m_responseData.append(buffStr);


            sprintf(buffStr, "|                            | %.5d  |\n", 
            for ( car=0 ; car<=2 ; car++ )
            {
                m_tableData[record].ehsStatusCar[car]
            }
            response->m_responseData.append(buffStr);


            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].detrainmentDoorStatusCar1 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].detrainmentDoorStatusCar3 );
            response->m_responseData.append(buffStr);



            // OA2 Data
            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].oa2DataValidity );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].driverConsoleCoverStatusCar1 );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].driverConsoleCoverStatusCar3 );
            response->m_responseData.append(buffStr);


            sprintf(buffStr, "|                            | %.5d  |\n", 
            for ( car=0 ; car<=2 ; car++ ) //         Two doors per byte
            { //                                              VV
                for ( int door=0 ; door<=((NumberOfDoorsPerCar/2)-1) ; door++ )
                {
                    m_tableData[record].trainDoorStatus[car][door]
                }
            }
            response->m_responseData.append(buffStr);


            sprintf(buffStr, "|                            | %.5d  |\n", 
            for ( car=0 ; car<=2 ; car++ ) //         Two doors per byte
            { //                                              VV
                m_tableData[record].emergencyHandleCoverStatusCar[car]
            }
            response->m_responseData.append(buffStr);


            sprintf(buffStr, "|                            | %.5d  |\n", 
            for ( car=0 ; car<=2 ; car++ ) //         Two doors per byte
            { //                                              VV
                m_tableData[record].ttisStatusCar[car]
            }
            response->m_responseData.append(buffStr);


            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].year );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].month );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].day );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].hour );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].minutes );
            response->m_responseData.append(buffStr);

            sprintf(buffStr, "|                            | %.5d  |\n", m_tableData[record].seconds );
            response->m_responseData.append(buffStr);
        }
        else if (qry->getOutputFormat() == f_hex)
        {
            response->m_responseData.append("ERROR: Hex format of AtsTrain table not supported\n");
        }

        return response;
    }
#endif // #if defined ATSSIMTABLEATSTRAIN_PROCESSUSERREAD_FIXED

    return 0;
}


UserResponse * ATSSimTableAtsTrain::processUserWrite(UserQuery * qry)
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

        typedef FlexiMask<unsigned short> WordMask;
		WordMask mask;

        int car = 0;
        int door = 0;

        //
        // loop 'round processing parameters.
        // This is the real guts of the method
        while (qry->moreArgs())
        {
            UserQuery::FieldValuePair fvp = qry->popArg();

            unsigned long field = fvp.first;
			UserQuery::FieldValueType value = fvp.second;

            switch (field)
            {
                case MaskFieldIndicator:
					mask.setValueMask(value);
                    break;
                case ToggleMaskFieldIndicator:
					mask.setToggleMask(value);
                    break;
                case LeaveMaskFieldIndicator:
					mask.setLeaveMask(value);
                    break;

                case 0:
                    m_tableData[record].physicalTrainNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 2:
                    m_tableData[record].atcTrainNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 5:
                    m_tableData[record].trainDataValidity = (DataValidity)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 6:
                    m_tableData[record].serviceNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 8:
                    m_tableData[record].scheduleNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;


                case 10:
                    m_tableData[record].branchNumber = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    m_tableData[record].trainDataValidity = Relevant;
                    break;

                case 12:
                    m_tableData[record].fbAbscissa = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    m_tableData[record].trainDataValidity = Relevant;
                    break;

                case 14:
                    m_tableData[record].headCarAbscissa = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    m_tableData[record].trainDataValidity = Relevant;
                    break;

                case 17:
                    m_tableData[record].headCabDirection = (HeadCabDirection)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 18:
                    m_tableData[record].talkativeStatus = (TalkativeStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 19:
                    m_tableData[record].holdStatus = (HoldStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 20:
                    m_tableData[record].platformAreaId = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 22:
                    m_tableData[record].trainReadinessMode = (TrainReadinessMode)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 23:
                    m_tableData[record].stalledStatus = (StalledStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 24:
                    m_tableData[record].evacuationStatus = (EvacuationStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 25:
                    m_tableData[record].ebStatus = (EBStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 27:
                    m_tableData[record].oa1DataValidity = (DataValidity)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 28:
                    m_tableData[record].tetraMobileStatus = (TetraMobileStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 29:
                case 30:
                case 31:
                    m_tableData[record].pecuStatusCar[field-29] = (Byte)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 32:
                    m_tableData[record].cctvCameraStatus = (CCTVCameraStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 33:
                    m_tableData[record].cctvSystemEquipmentStatus = (CCTVEquipmentStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 35:
                    m_tableData[record].trainPAStatus = (TrainPAStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 36:
                    m_tableData[record].underSeatFireDetectionCar1 = (FireDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 37:
                    m_tableData[record].underFrameFireDetectionCar1 = (FireDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 38:
                    m_tableData[record].internalSmokeDetectionCar1 = (InteriorSmokeDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 39:
                    m_tableData[record].externalSmokeDetectionCar1 = (ExteriorSmokeDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 40:
                    m_tableData[record].underSeatFireDetectionCar2 = (FireDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 41:
                    m_tableData[record].underFrameFireDetectionCar2 = (FireDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 42:
                    m_tableData[record].internalSmokeDetectionCar2 = (InteriorSmokeDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 43:
                    m_tableData[record].externalSmokeDetectionCar2 = (ExteriorSmokeDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 44:
                    m_tableData[record].underSeatFireDetectionCar3 = (FireDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 45:
                    m_tableData[record].underFrameFireDetectionCar3 = (FireDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 46:
                    m_tableData[record].internalSmokeDetectionCar3 = (InteriorSmokeDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 47:
                    m_tableData[record].externalSmokeDetectionCar3 = (ExteriorSmokeDetectionStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 49:
                    m_tableData[record].externalTemperature = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 50:
                case 51:
                case 52:
                    m_tableData[record].extinguisherStatusCar[field-50] = (ExtinguisherStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                // bytes 54-59 EHS status, see mask logic below
                case 54:
                case 56:
                case 58:
                    car = (field-54)/2;
                    if (mask.isUsed())
                    {
                        m_tableData[record].ehsStatusCar[car] = mask.apply(m_tableData[record].ehsStatusCar[0]);
                        std::cout << "Applied mask to field=" << field << " mask=" << mask << std::endl;;
                        mask.reset();
                    }
                    else // mask is notUsed
                    {
                        m_tableData[record].ehsStatusCar[car] = value;
                        std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    }
                    break;
                    
                case 60:
                    m_tableData[record].detrainmentDoorStatusCar1 = (DetrainmentDoorStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 61:
                    m_tableData[record].detrainmentDoorStatusCar3 = (DetrainmentDoorStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 63:
                    m_tableData[record].oa2DataValidity = (DataValidity)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 64:
                    m_tableData[record].driverConsoleCoverStatusCar1 = (DriverConsoleCoverStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 65:
                    m_tableData[record].driverConsoleCoverStatusCar3 = (DriverConsoleCoverStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 66:
                case 68:
                case 70:
                case 72:
                case 74:
                case 76:
                    door = (field-66) % 4;
                    car = (field-66) / 4;
                    if (mask.isUsed())
                    {
                        m_tableData[record].trainDoorStatus[car][door] = mask.apply(m_tableData[record].trainDoorStatus[car][door]);
                        std::cout << "Applied mask to field=" << field << " mask=" << mask << std::endl;;
                        mask.reset();
                    }
                    else // mask is notUsed
                    {
                        m_tableData[record].trainDoorStatus[car][door] = value;
                        std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    }
                    break;

                case 78:
                case 79:
                case 80:
                    m_tableData[record].emergencyHandleCoverStatusCar[field-78] = (EmergencyHandleCoverStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 81:
                case 82:
                case 83:
                    m_tableData[record].ttisStatusCar[field-81] = (TTISStatus)value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 84:
                    m_tableData[record].year = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 86:
                    m_tableData[record].month = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 88:
                    m_tableData[record].day = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 90:
                    m_tableData[record].hour = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 92:
                    m_tableData[record].minutes = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

                case 94:
                    m_tableData[record].seconds = value;
                    std::cout << "Modified field=" << field << " value=" << value << std::endl;;
                    break;

				//TD17131
				case 95:
					if (value == 0x00)
					{
						m_tableData[record].trainDataValidity = Irrelevant;
					}
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


void ATSSimTableAtsTrain::toStream(std::ostream& os) const
{
}


void ATSSimTableAtsTrain::fromStream(const std::ostream& os)
{
}


void ATSSimTableAtsTrain::toNetwork(ByteVector & n)
{
    int idx;

    for ( unsigned int i=0 ; i<m_numberOfRecords ; i++ )
    {
                                                            //  @Begin
        ATS_TABLE_PUT(Word,physicalTrainNumber);            //  0
        ATS_TABLE_PUT(Word,atcTrainNumber);                 //  2

        // Train Data
        ATS_TABLE_PUT_SPARE(Byte);                          //  4 spare bits    
        ATS_TABLE_PUT(Byte,trainDataValidity);              //  5
        ATS_TABLE_PUT(Word,serviceNumber);                  //  6
        ATS_TABLE_PUT(Word,scheduleNumber);                 //  8
        ATS_TABLE_PUT(Word,branchNumber);                   //  10
        ATS_TABLE_PUT(Word,fbAbscissa);                     //  12
        ATS_TABLE_PUT(Word,headCarAbscissa);                //  14
        ATS_TABLE_PUT_SPARE(Byte);                          //  16  spare bits
        ATS_TABLE_PUT(Byte,headCabDirection);               //  17
        ATS_TABLE_PUT(Byte,talkativeStatus);                //  18
        ATS_TABLE_PUT(Byte,holdStatus);                     //  19
        ATS_TABLE_PUT(Word,platformAreaId);                 //  20
        ATS_TABLE_PUT(Byte,trainReadinessMode);             //  22
        ATS_TABLE_PUT(Byte,stalledStatus);                  //  23
        ATS_TABLE_PUT(Byte,evacuationStatus);               //  24
        ATS_TABLE_PUT(Byte,ebStatus);                       //  25

        // OA1 Data
        // On-board communication
        ATS_TABLE_PUT_SPARE(Byte); // spare bits            //  26
        ATS_TABLE_PUT(Byte,oa1DataValidity);                //  27
        ATS_TABLE_PUT(Byte,tetraMobileStatus);              //  28
        for ( idx=0 ; idx<3 ; idx++ )                       //  29 30 31
        { ATS_TABLE_PUT(Byte,pecuStatusCar[idx]); }
//         ATS_TABLE_PUT(Byte,pecuStatusCar1);
//         ATS_TABLE_PUT(Byte,pecuStatusCar2);
//         ATS_TABLE_PUT(Byte,pecuStatusCar3);
        ATS_TABLE_PUT(Byte,cctvCameraStatus);               //  32
        ATS_TABLE_PUT(Byte,cctvSystemEquipmentStatus);      //  33
        ATS_TABLE_PUT_SPARE(Byte); // spare bits            //  34
        ATS_TABLE_PUT(Byte,trainPAStatus);                  //  35
        // Fire detection data
        // Car1
        ATS_TABLE_PUT(Byte,underSeatFireDetectionCar1);     //  36
        ATS_TABLE_PUT(Byte,underFrameFireDetectionCar1);    //  37
        ATS_TABLE_PUT(Byte,internalSmokeDetectionCar1);     //  38
        ATS_TABLE_PUT(Byte,externalSmokeDetectionCar1);     //  39
        // Car2
        ATS_TABLE_PUT(Byte,underSeatFireDetectionCar2);     //  40
        ATS_TABLE_PUT(Byte,underFrameFireDetectionCar2);    //  41
        ATS_TABLE_PUT(Byte,internalSmokeDetectionCar2);     //  42
        ATS_TABLE_PUT(Byte,externalSmokeDetectionCar2);     //  43
        // Car3
        ATS_TABLE_PUT(Byte,underSeatFireDetectionCar3);     //  44
        ATS_TABLE_PUT(Byte,underFrameFireDetectionCar3);    //  45
        ATS_TABLE_PUT(Byte,internalSmokeDetectionCar3);     //  46
        ATS_TABLE_PUT(Byte,externalSmokeDetectionCar3);     //  47
        //
        ATS_TABLE_PUT_SPARE(Byte); // spare bits            //  48
        ATS_TABLE_PUT(Byte,externalTemperature);            //  49
        for ( idx=0 ; idx<3 ; idx++ )
        { ATS_TABLE_PUT(Byte,extinguisherStatusCar[idx]); } //  50 51 52
        ATS_TABLE_PUT_SPARE(Byte); // spare bits            //  53
        //
        for ( idx=0 ; idx<3 ; idx++ )
        { ATS_TABLE_PUT(Word,ehsStatusCar[idx]); }          //  54 56 58
        ATS_TABLE_PUT(Byte,detrainmentDoorStatusCar1);      //  60
        ATS_TABLE_PUT(Byte,detrainmentDoorStatusCar3);      //  61

        // OA2 Data
        ATS_TABLE_PUT_SPARE(Byte); // spare bits            //  62
        ATS_TABLE_PUT(Byte,oa2DataValidity);                //  63
        ATS_TABLE_PUT(Byte,driverConsoleCoverStatusCar1);   //  64
        ATS_TABLE_PUT(Byte,driverConsoleCoverStatusCar3);   //  65

        for ( idx=0 ; idx<3 ; idx++ )                       //  66 68  Car1
        {                                                   //  70 72  Car2
            ATS_TABLE_PUT(Word,trainDoorStatus[idx][0]);    //  74 76  Car3
            ATS_TABLE_PUT(Word,trainDoorStatus[idx][1]);
        }

        for ( idx=0 ; idx<3 ; idx++ )
        { ATS_TABLE_PUT(Byte,emergencyHandleCoverStatusCar[idx]); } //  78 79 80

        for ( idx=0 ; idx<3 ; idx++ )
        { ATS_TABLE_PUT(Byte,ttisStatusCar[idx]); }         //  81 82 83

        ATS_TABLE_PUT(Word,year);                           //  84
        ATS_TABLE_PUT(Word,month);                          //  86
        ATS_TABLE_PUT(Word,day);                            //  88
        ATS_TABLE_PUT(Word,hour);                           //  90
        ATS_TABLE_PUT(Word,minutes);                        //  92
        ATS_TABLE_PUT(Word,seconds);                        //  94
    }
}


void ATSSimTableAtsTrain::fromNetwork(const ByteVector & s)
{
    throw std::logic_error("Ats Train Table should not be put to vector fromNetwork");
}


void ATSSimTableAtsTrain::getByteVector(ByteVector & bv) const
{
}


