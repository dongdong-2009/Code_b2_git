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
#if !defined(EndOfTtisDownloadEvent_H)
#define EndOfTtisDownloadEvent_H

#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Represents a download completion message
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:06 PM
     */

    class EndOfTtisDownloadEvent : public TimsEvent
    {

    public:

        /**
         * Success/Failure of a TTIS download
         * @author Adam Radics
         * @version 1.0
         * @created 05-Feb-2008 3:20:06 PM
         */
        enum ETtisDownloadResult
        {
            TtisDownloadOk = 0,
            TtisDownloadFailed = 1,
            TtisDownloadRepetition = 2
        };


        /**
         * Create a train event from incoming data
         *
         * @exception ProtocolException if the event could not be created ie the event is invalid
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        EndOfTtisDownloadEvent( const TrainMessageData& data,
                                const std::string& sendingTsi,
                                const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~EndOfTtisDownloadEvent();


        /**
         * Gets the type of message this reader is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Gets the command result
         */
        ETtisDownloadResult getResult() const;


        /**
         * Gets the frames to repeat
         */
        std::vector<unsigned short> getFramesToRepeat() const;


        /**
         * Formats and returns a loggable string.
         */
        virtual std::string getLogString() const;


    private:

        /**
         * Private default constructor
         */
        EndOfTtisDownloadEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Offset of the result field
         */
        static const unsigned short RESULT_POSITION;


        /**
         * Offset of the frames to repeat
         */
        static const unsigned short REPETITION_POSITION;


        /**
         * The result of the download
         */
        ETtisDownloadResult m_result;


        /**
         * If the result is repetition, there will be some frame numbers given.
         */
        std::vector<unsigned short> m_framesToRepeat;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:06 PM
     */
    typedef boost::shared_ptr< EndOfTtisDownloadEvent > EndOfTtisDownloadEventPtr;

} //end of namespace


#endif // !defined(EndOfTtisDownloadEvent_H)
