#ifndef ATS_SIM_TABLE_ATSTRAIN_H
#define ATS_SIM_TABLE_ATSTRAIN_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsTrain.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Declaration file for ATSSimTableAtsTrain class
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTable.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableEnum.h"

namespace TA_IRS_App
{
namespace ATS_Sim
{
    /**
      * @class ATSSimTableAtsTrain
      *
      * ATSSimTableAtsTrain is a derived table class.
      *
      * TABLE AtsTrain 
      *
      */
    class ATSSimTableAtsTrain : public ATSSimTable
    {

    public:
        /**
          * Preferred constructor
          * @param parent the owner of this object
          */
        ATSSimTableAtsTrain(ATSSimTableQueryProcessor * parent, LocationType loc);

        /**
          * Destructor
          */
        ~ATSSimTableAtsTrain();


        /**
          * Accessor for the size of this table
          * @return the size of the data in this table as determined by the
          *         value of m_tableData.size()
          */
        unsigned short getTableSize() const;


        /** Virtual streaming operator.
          * Overridden from the base class ATSSimTable::toStream
          */
        virtual void toStream(std::ostream& os) const;
        

        /** Virtual in-stream operator.
          * Overridden from the base class ATSSimTable::toStream
          *
          * Inputs the table data to the stream as a string of bytes using
          * ATSSimUtility::data2string, which at the moment means hexadecimal.
          */
        virtual void fromStream(const std::ostream& os);
        

        /** Place the table data into a stream of bytes for sending through the
          * socket.  This base class' default behaviour is to send the raw
          * as retrieved by getByteVector(ByteVector &);
          * 
          * Derived classes should override either this or getByteVector to
          * ensure that the ByteVector has the appropriate raw data in
          * network order.
          */
        virtual void toNetwork(ByteVector & n);


        /** Accept the stream of bytes from the socket.  Store locally as
          * best suits.  This base class' default behaviour is to accept the
          * raw bytes and store them as such.
          */
        virtual void fromNetwork(const ByteVector & s);

        /**
          * Overloaded accessor for the underlying ByteVector, ie. the
          * std::vector of unsigned chars that represents the table data.
          * 
          * @param bv the vector to fill with this table's raw data.
          */
        virtual void getByteVector(ByteVector & bv) const;

    protected:
        typedef Word TrainDateWord;

        ///
        ///
        enum // TrainDoor
        {
            A1  = 0,
            A2  = 1,
            A3  = 2,
            A4  = 3,
            B1  = 4,
            B2  = 5,
            B3  = 6,
            B4  = 7

        }; // TrainDoor;


        ///
        ///
        typedef EHSStatus CarEHSDoorStatus[NumberOfDoorsPerCar];

        ///
        ///
        typedef CarEHSDoorStatus TrainEHSDoors[3];

        ///
        ///
        typedef PECUStatus CarPECUDoorStatus[NumberOfDoorsPerCar];

        ///
        ///
        typedef CarPECUDoorStatus TrainPECUDoors[3];


        /**
          * struct representing the data in an AtsTrain table
          */
        typedef struct ATSSimTableAtsTrainRecord
        {
            Word                physicalTrainNumber;
            Word                atcTrainNumber;

            DataValidity        trainDataValidity;
            Word                serviceNumber;
            Word                scheduleNumber;

            Word                branchNumber;
            Word                fbAbscissa;
            Word                headCarAbscissa;

            HeadCabDirection    headCabDirection;
            TalkativeStatus     talkativeStatus;
            HoldStatus          holdStatus;
            Word                platformAreaId;
            TrainReadinessMode  trainReadinessMode;
            StalledStatus       stalledStatus;
            EvacuationStatus    evacuationStatus;
            EBStatus            ebStatus;

            // On-board communication (from OA1)
            DataValidity        oa1DataValidity;
            TetraMobileStatus   tetraMobileStatus;
            Byte                pecuStatusCar[3];
            CCTVCameraStatus    cctvCameraStatus;
            CCTVEquipmentStatus cctvSystemEquipmentStatus;
            TrainPAStatus       trainPAStatus;

            // Fire detection data (from OA1)
            FireDetectionStatus             underSeatFireDetectionCar1;
            FireDetectionStatus             underFrameFireDetectionCar1;
            InteriorSmokeDetectionStatus    internalSmokeDetectionCar1;
            ExteriorSmokeDetectionStatus    externalSmokeDetectionCar1;
            FireDetectionStatus             underSeatFireDetectionCar2;
            FireDetectionStatus             underFrameFireDetectionCar2;
            InteriorSmokeDetectionStatus    internalSmokeDetectionCar2;
            ExteriorSmokeDetectionStatus    externalSmokeDetectionCar2;
            FireDetectionStatus             underSeatFireDetectionCar3;
            FireDetectionStatus             underFrameFireDetectionCar3;
            InteriorSmokeDetectionStatus    internalSmokeDetectionCar3;
            ExteriorSmokeDetectionStatus    externalSmokeDetectionCar3;

            Byte                externalTemperature;
            ExtinguisherStatus  extinguisherStatusCar[3];

            Word                ehsStatusCar[3];

            DetrainmentDoorStatus detrainmentDoorStatusCar1;
            DetrainmentDoorStatus detrainmentDoorStatusCar3;

            // Data from OA2
            DataValidity        oa2DataValidity;
            DriverConsoleCoverStatus driverConsoleCoverStatusCar1;
            DriverConsoleCoverStatus driverConsoleCoverStatusCar3;

            Word                trainDoorStatus[3][2];

            EmergencyHandleCoverStatus emergencyHandleCoverStatusCar[3];

            TTISStatus          ttisStatusCar[3];

            TrainDateWord       year;
            TrainDateWord       month;
            TrainDateWord       day;
            TrainDateWord       hour;
            TrainDateWord       minutes;
            TrainDateWord       seconds;
        };


        ///
        /// Type definition of an array of the AtsTrainRecords
        /// typedef ATSSimTableAtsTrainRecord * AtsTrainDataArray;
        ///
        ATSSimTableAtsTrainRecord * m_tableData;

        /**
          * overridden to make this class concrete. To be implemented later.
          */
        virtual UserResponse * processUserRead(UserQuery * ur);

        /**
          * overridden to make this class concrete. To be implemented later.
          */
        virtual UserResponse * processUserWrite(UserQuery * ur);

    }; // class ATSSimTableAtsTrain

} // namespace ATS_Sim
} // namespace TA_IRS_App
#endif // #ifndef ATS_SIM_TABLE_ATSTRAIN_H
