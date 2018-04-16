#ifndef ATS_SIM_TABLE_ISCSWASH_H
#define ATS_SIM_TABLE_ISCSWASH_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsWash.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Declaration file for ATSSimTableIscsWash class
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTable.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableEnum.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimModbusFrames.h"

namespace TA_IRS_App
{
namespace ATS_Sim
{
    /**
      * @class ATSSimTableIscsWash
      *
      * ATSSimTableIscsWash is a derived table class.
      *
      * TABLE IscsWash 
      *
      */
    class ATSSimTableIscsWash : public ATSSimTable
    {

    public:
        /**
          * Preferred constructor
          * @param parent the owner of this object
          */
        ATSSimTableIscsWash(ATSSimTableQueryProcessor * parent, LocationType loc);

        /**
          * Destructor
          */
        ~ATSSimTableIscsWash();


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

    private:
        /**
          * struct representing the data in an IscsWash table
          */
        typedef struct ATSSimTableIscsWashRecord
        {
            // 0
            DataValidity     validityField;
            Byte             wpStatus;
            // 2
            Byte             noWash;
            Byte             dailyRinse;
            // 4
            Byte             dailyDetergent;
            Byte             weeklyWash;
            // 6
            Byte             cabEndWashCompleted;
            Byte             washCycleFullyCompleted;
            // 8
            Byte             spare1;
            Byte             trainSpeedOutOfRange;
            // 10
            Byte             trainStalledInTWP;
            Byte             trainIncorrectlyPositioned;
            // 12
            Byte             spare2;
            Byte             washCycleStartFailure;
            // 14
            Byte             pHCheck;
            Byte             mainsWaterLow;
            // 16
            Byte             secondBrushRinseAndFinalRinseSpray;
            Byte             cabEndWashGantryMovementFailureHor;
            // 18
            Byte             cabEndWashGantryMovementFailureVer;
            Byte             cabEndWashBrushMovementFailure;
            // 20
            Byte             emergencyStopActivated;
            Byte             pnuematicFailure;
            // 22
            Byte             brushActuationFailure;
            Byte             sideRoofBrushingFailure;
            // 24
            Byte             endBrushingFailure;
            Byte             airBlowFailure;
            // 26
            Byte             preWetFailure;
            Byte             recycledRinseFailure;
            // 28
            Byte             waterRecyclingFailure;
            Byte             detergentFailure;
            // 30
            Byte             detergentLowAlarms;
			Byte			 airPressureLow;
			// 31
			Byte			 nonCriticalSummaryAlarm;				
            };

        /**
          * 
          */
        ATSSimTableIscsWashRecord m_tableData;


        /**
          * overridden to make this class concrete. To be implemented later.
          */
        virtual UserResponse * processUserRead(UserQuery * ur);

        /**
          * overridden to make this class concrete. To be implemented later.
          */
        virtual UserResponse * processUserWrite(UserQuery * ur);

    }; // class ATSSimTableIscsWash
} // namespace ATS_Sim
} // namespace TA_IRS_App
#endif // #ifndef ATS_SIM_TABLE_ISCSWASH_H
