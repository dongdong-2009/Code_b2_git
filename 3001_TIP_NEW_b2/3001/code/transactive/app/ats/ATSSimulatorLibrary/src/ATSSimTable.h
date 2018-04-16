#ifndef ATS_SIM_TABLE_H
#define ATS_SIM_TABLE_H
/*
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 */

/**
  * Declaration file for ATSSimTable class
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTable.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  */
#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimModbusFrames.h"
#include <list>

namespace TA_IRS_App
{
namespace ATS_Sim
{
    class ATSSimTableQueryProcessor;
	class UserQuery;
	class UserResponse;

    /**
      * @class ATSSimTable
      *
      * ATSSimTable is an abstract base class for ATSSim Tables.  Whether a
      * table is only read from or written to by the ISCS is not distinguished,
      * as all tables can be written or read by the Simulation User.
      *
      */
    class ATSSimTable
    {

    public:
        /**
          * Preferred constructor
          * @param tableAddress the "address" of this table
          * @param parent this table's owner
          */
        ATSSimTable(TableIdentifier tableId, ATSSimTableQueryProcessor * parent, LocationType loc);

        /**
          * Destructor
          */
        virtual ~ATSSimTable();

        /**
          * Accessor for the start address of this table
          * @return the start address of this table
          */
        unsigned short getTableAddress() const;

        /**
          * Accessor for the "number" of this table
          * @return the number of this table as in the enumeration in Common.h
          */
        unsigned short getTableIdentifier() const;

        /**
          * Accessor for the size of this table
          * @return the size of the data in this table as determined by the
          *         value of m_tableData.size()
          */
        virtual unsigned short getTableSize() const = 0;

        /**
          * Accessor for the size of this table in Words
          * @return the number of words in this table (=getTableSize()/2)
          */
        virtual unsigned short getTableWordSize() const;

        /**
          * Accessor for the size of this table
          * @return the size of the data in this table as determined by the
          *         value of m_tableData.size()
          */
        unsigned int getNumberOfRecords() const;

        /**
          * Means of "upward" object navigation
          * @return this instance's parent
          */
        ATSSimTableQueryProcessor * getParent() const;

        /**
          * Accessor for this table's location
          * @return the value of data member m_location
          */
        LocationType getLocation() const;

        /**
          * Processes the Read Query Packet that has arrived from the ISCS
          *
          * @param query the ReadQueryPacket from the ISCS
          */
        virtual void processReadQuery(ModbusQuery * query);

        /**
          * Processes the Write Query Packet that has arrived from the ISCS
          *
          * Subclasses that override this function MUST call this base class
          * function at the end of their processing.
          *
          * @param query the WriteQueryPacket from the ISCS
          */
        virtual void processWriteQuery(ModbusQuery * query);


        /**
          * 
          */
        virtual UserResponse * processUserRead(UserQuery * query) = 0;


        /**
          * Processes the Write Query Packet that has arrived from the ISCS
          *
          * Subclasses that override this function MUST call this base class
          * function at the end of their processing.
          *
          * @param ur the UserQuery packet from the CommandProcessor
          * @return a UserResponse
          */
        virtual UserResponse * processUserWrite(UserQuery * query) = 0;


        /** Used as a virtual streaming operator.
          *
          * Outputs the table data to the stream as a string of bytes using
          * ATSSimUtility::data2string, which at the moment means hexadecimal.
          */
        virtual void toStream(std::ostream& os) const;
        

        /** Used as a virtual in-stream operator.
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
          *
          * @param n the ByteVector to populate with the network data
          */
        virtual void toNetwork(ByteVector & n) = 0;


        /** Accept the stream of bytes from the socket.  Store locally as
          * best suits.  This base class' default behaviour is to accept the
          * raw bytes and store them as such.
          *
          * @param s the ByteVector that contains the network data
          */
        virtual void fromNetwork(const ByteVector & s) = 0;

        /**
          * Adds the given table to this table's set of dependents
          *
          * Part of a small inter-table observer/subscriber/dependence
          * mechanism built in to this class. &c 
          *
          * @param t the table that has declared a dependency upon this table
          */
        virtual void addDependent(ATSSimTable * t);

        /**
          * Default action is to do nothing. Subclasses that need to setup
          * dependencies with other tables must override this and call:
          *
          *     m_parent->dependUpon(tableAddress, this);
          *
          * for each required depended upon table
          */
        virtual void setupDependents();

    protected:

        /**
          * Overloaded accessor for the underlying ByteVector, ie. the
          * std::vector of unsigned chars that represents the table data.
          * 
          * @param bv the vector to fill with this table's raw data.
          */
        virtual void getByteVector(ByteVector & bv) const = 0;


#ifdef ATSSIMTABLE_USEFUL_ACCESSORS
        //
        // Accessor Methods
        // 
        // Used to get values from the table
        //

        /**
          * Retrieves a byte from the table data.
          *
          * @param index the position within the table to retrieve
          * @return the byte at the specified position
          */
        Byte getByte(unsigned int index) const;

        /**
          * Retrieves an unsigned short value from the table data, converting
          * from network order first
          *
          * @param index the position within the table to retrieve
          * @return the unsigned short value at the specified position
          */
        unsigned short getUShort(unsigned int index) const;

        /**
          * Retrieves an unsigned long value from the table data, converting
          * from network order first
          *
          * @param index the position within the table to retrieve
          * @return the unsigned long value at the specified position
          */
        unsigned long getULong(unsigned int index) const;
#endif // #ifdef ATSSIMTABLE_USEFUL_ACCESSORS
#ifdef ATSSIMTABLE_USEFUL_MUTATORS
        //
        // Mutator Methods
        // 
        // Used to set values in the table
        //

        /**
          * Sets a byte in the data
          *
          * @param value the byte value
          * @param index the [optional] position within the table to
          *              set the value
          */
        void setTableData(Byte value, unsigned int index=0);

        /**
          * Sets an unsigned short value. Converts to network order first.
          *
          * @param value the unsigned short value
          * @param index the [optional] position within the table to
          *              set the value
          */
        void setTableData(unsigned short value, unsigned int index=0);

        /**
          * Sets an unsigned long value. Converts to network order first.
          *
          * @param value the unsigned long value
          * @param index the [optional] position within the table to
          *              set the value
          */
        void setTableData(unsigned long value, unsigned int index=0);

        /**
          * setTableData
          *
          * This overloaded version copies byte by byte to the internal
          * representation from the given pointer.
          *
          * @param address pointer to the new table data\
          * @param bytes the number of data bytes to set. The default of -1
          *             causes m_tableData.size() bytes to be transferred.
          *             (due to the way default parameters work, it is not
          *             possible to specify the index without specifying
          *             the bytes parameter)
          * @param index the index at which to start transferring to.  If
          *             bytes+index > m_tableData.size(), an out_of_range
          *             exception is thrown.
          */
        void setTableData(void * address, int bytes=-1, unsigned int index=0);

#endif // #ifdef ATSSIMTABLE_USEFUL_MUTATORS


    protected:
        /**
          * Allows subclasses that have overwritten processWriteQuery()
          * but that use ATSSimTable::processWriteQuery to sent the Write
          * Response, to specify what return value to stick in that field.
          *
          * The Modbus frame function code is automatically adjusted to reflect
          * the exception.
          */
        void setExceptionCode(Byte value);

        /// Enumeration of reasons for a table having changed
        typedef enum ChangeReasonCode
        {
            no_change           = 0L,
            user_modification   = 1L,
            iscs_write_query    = 2L
        };

        /// Notifies all dependents when this table is changed
        void notifyDependents(ChangeReasonCode reasonCode);

        /// Called by subclasses to register for notification
        void dependUpon(int tableAddress);

        /**
          * This is the notification that a provider has changed, ie. a table
          * that we have setup a dependency on has called notifyDependents.
          *
          * Subclasses can override this to take special actions depending
          * on why the provider has changed.
          */
        virtual void providerChanged(ATSSimTable * t, ChangeReasonCode reasonCode);

    protected:
        ///
        /// How many records are in this table at this location
        unsigned int m_numberOfRecords;

    private:
        ///
        /// The number of this beast
        TableIdentifier m_tableIdentifier;

        ///
        /// The modbus address of this table. As ascertained from call to
        /// TableAddressRegister::findAddress()
        unsigned short m_tableAddress;

        ///
        /// This instance's owner
        ATSSimTableQueryProcessor * m_parent;

        ///
        /// Whether this is an OCC, Depot or Station simulator
        LocationType m_location;

        /// The data contained in the table.
        // ByteVector m_tableData;

        ///
        /// Flag to track if this table has been changed since the last time
        /// it was read
        bool m_dirty;

        ///
        /// value inserted as the exception code in a response
        Byte m_exceptionCode;

        ///
        /// Typedef for a list of tables dependent on this one
        typedef std::list< ATSSimTable * > DependentTableContainer;

        ///
        /// List of tables dependent on this one
        DependentTableContainer m_dependents;
    }; // class ATSSimTable

    inline unsigned int ATSSimTable::getNumberOfRecords() const
    {
        return m_numberOfRecords;
    }

    /**
      * Streaming operator.  Since operators are not virtual, it calls
      * the virtual toStream().  Also, since the first parameter must be the
      * OStream ref, this is a nonmember function, .
      *
      * @param os the OStream to stream to
      * @param table the ATSSimTable to be streamed
      */
    std::ostream &operator<< (std::ostream &os, const ATSSimTable &table);

#define ATS_TABLE_PUT(x,y) ATSSimUtility::put##x(m_tableData[i].y,n)
#define ATS_TABLE_PUT_SPARE(x) ATSSimUtility::put##x(0,n)
// #define ATS_TABLE_PUT2D(x,y) ATSSimUtility::put##x(m_tableData[i][j].y,n)
#define ATS_TABLE_GET(y,z) m_tableData[i].z = ATSSimUtility::get##y(s,x)
#define ATS_TABLE_GET_SPARE(y) ATSSimUtility::get##y(s,x)
// #define ATS_TABLE_GET2D(y,z) m_tableData[i][j].z = ATSSimUtility::get##y(s,x)
} // namespace ATS_Sim
} // namespace TA_IRS_App
#endif // #ifndef ATS_SIM_TABLE_H
