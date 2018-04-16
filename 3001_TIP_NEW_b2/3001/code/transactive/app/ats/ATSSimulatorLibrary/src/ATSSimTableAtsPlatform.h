#ifndef ATS_SIM_TABLE_ATSPLATFORM_H
#define ATS_SIM_TABLE_ATSPLATFORM_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsPlatform.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Declaration file for ATSSimTableAtsPlatform class
  */
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTable.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableEnum.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimModbusFrames.h"


namespace TA_IRS_App
{
namespace ATS_Sim
{
    class ATSSimTableIscsPlatform;

    /**
      * @class ATSSimTableAtsPlatform
      *
      * ATSSimTableAtsPlatform is a derived table class.
      *
      * TABLE AtsPlatform 
      *
      */
    class ATSSimTableAtsPlatform : public ATSSimTable
    {

    public:
        /**
          * Preferred constructor
          * @param parent the owner of this object
          */
        ATSSimTableAtsPlatform(ATSSimTableQueryProcessor * parent, LocationType loc);

        /**
          * Destructor
          */
        ~ATSSimTableAtsPlatform();


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

        /** Allows the ATSSimTableIscsPlatform table to update this table's
          * closed and locked field when it gets such requests from the Agent
          * 
          * @param platformAreaId
          * @param value
          */
        void setPsdClosedAndLocked(Word platformAreaId, Byte value);

    private:
        typedef enum PlatformType { Arrival=0, Departure=1 };

        /**
          * struct representing the data in an AtsPlatform table
          * common to the first and second last train information
          */
        typedef struct PlatformNextTrainData
        {
            DataValidity        nextTrainDataValidity;
            Byte                platformType;
            Word                physicalNumber;
            Word                serviceNumber;
            Word                destinationAreaId;
            //Byte              spare bits
            bool                lastTrainOfTheOperatingDay;
            Word                year;
            Word                month;
            Word                day;
            Word                hour;
            Word                minutes;
            Word                seconds;
        };

        typedef bool PSDUninhibitArray[12];

        /**
          * struct representing the data in an AtsPlatform table
          */
        typedef struct ATSSimTableAtsPlatformRecord
        {
            Word                platformAreaId;

            Byte                prearrivalEventStatus;
            Byte                arrivalEventStatus;
            Byte                predepartureEventStatus;
            Byte                departureEventStatus;
            Byte                skipEventStatus;
            Byte                holdEventStatus;

            Byte                trainEvacuationStatus;
            Byte                overrunEventStatus;

            PlatformNextTrainData firstNextTrain;
            PlatformNextTrainData secondNextTrain;

            Byte                psdClosePreWarning;
            Byte                psdClosedAndLocked;

            Word                psdDoorUninhibitionRequest;
        };

        ///
        ///
        ATSSimTableAtsPlatformRecord * m_tableData;


        /**
          * overridden to make this class concrete. To be implemented later.
          */
        virtual UserResponse * processUserRead(UserQuery * ur);

        /**
          * overridden to make this class concrete. To be implemented later.
          */
        virtual UserResponse * processUserWrite(UserQuery * ur);

        ///
        ///
        unsigned short m_recordSize;

        ///
        ///
        ATSSimTableIscsPlatform * m_iscsPlatformTable;
    }; // class ATSSimTableAtsPlatform
} // namespace ATS_Sim
} // namespace TA_IRS_App
#endif // #ifndef ATS_SIM_TABLE_ATSPLATFORM_H
