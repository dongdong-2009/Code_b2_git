#ifndef PA_SIM_TABLE_PAPLATFORM_H
#define PA_SIM_TABLE_PAPLATFORM_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTablePAPlatform.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Declaration file for PASimTablePAPlatform class
  */
#include "app/PA/PASimulatorLibrary/src/PASimTable.h"
#include "app/PA/PASimulatorLibrary/src/PASimCommon.h"
#include "app/PA/PASimulatorLibrary/src/PASimTableEnum.h"
#include "app/PA/PASimulatorLibrary/src/PASimModbusFrames.h"


namespace TA_IRS_App
{
namespace PA_Sim
{
    class PASimTableIscsPlatform;

    /**
      * @class PASimTablePAPlatform
      *
      * PASimTablePAPlatform is a derived table class.
      *
      * TABLE PAPlatform 
      *
      */
    class PASimTablePAPlatform : public PASimTable
    {

    public:
        /**
          * Preferred constructor
          * @param parent the owner of this object
          */
        PASimTablePAPlatform(PASimTableQueryProcessor * parent, LocationType loc);

        /**
          * Destructor
          */
        ~PASimTablePAPlatform();


        /**
          * Accessor for the size of this table
          * @return the size of the data in this table as determined by the
          *         value of m_tableData.size()
          */
        unsigned short getTableSize() const;


        /** Virtual streaming operator.
          * Overridden from the base class PASimTable::toStream
          */
        virtual void toStream(std::ostream& os) const;
        

        /** Virtual in-stream operator.
          * Overridden from the base class PASimTable::toStream
          *
          * Inputs the table data to the stream as a string of bytes using
          * PASimUtility::data2string, which at the moment means hexadecimal.
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

        /** Allows the PASimTableIscsPlatform table to update this table's
          * closed and locked field when it gets such requests from the Agent
          * 
          * @param platformAreaId
          * @param value
          */
        void setPsdClosedAndLocked(Word platformAreaId, Byte value);

    private:
        typedef enum PlatformType { Arrival=0, Departure=1 };

        /**
          * struct representing the data in an PAPlatform table
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
          * struct representing the data in an PAPlatform table
          */
        typedef struct PASimTablePAPlatformRecord
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
        PASimTablePAPlatformRecord * m_tableData;


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
        PASimTableIscsPlatform * m_iscsPlatformTable;
    }; // class PASimTablePAPlatform
} // namespace PA_Sim
} // namespace TA_IRS_App
#endif // #ifndef PA_SIM_TABLE_PAPLATFORM_H
