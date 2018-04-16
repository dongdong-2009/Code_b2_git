/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(ABSTRACTTABLEEVENT_H)
#define ABSTRACTTABLEEVENT_H

#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"


namespace TA_IRS_Bus
{
    /**
     * Represents an incoming OA1/ATC table
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:53 PM
     */

    class AbstractTableEvent : public TimsEvent
    {

    public:


        /**
         * Create a train event from incoming data
         *
         * @exception ProtocolException if the event could not be created ie this is not the right kind of event or the event is invalid
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        AbstractTableEvent( const TrainMessageData& data,
                            const std::string& sendingTsi,
                            const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~AbstractTableEvent();


        /**
         * Returns the data.
         * 24 bytes long.
         */
        const TrainMessageData& getTableData() const;


    protected:

        /**
         * Gets the length of the expected message, so the message length can be validated.
         */
        virtual unsigned short getExpectedMessageLength() const;


    private:

        /**
         * Private default constructor
         */
        AbstractTableEvent();


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * Length of the data table.
         */
        static const unsigned short DATA_LENGTH;


        /**
         * Offset of the data table.
         */
        static const unsigned short DATA_POSITION;


        /**
         * This is the table (24 bytes)
         */
        TrainMessageData m_tableData;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:53 PM
     */
    typedef boost::shared_ptr< AbstractTableEvent > AbstractTableEventPtr;

} //end of namespace

#endif // !defined(AbstractTableEvent_H)
