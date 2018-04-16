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
#if !defined(TtisFreeTextMessageCommand_H)
#define TtisFreeTextMessageCommand_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractTtisCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"


namespace TA_IRS_Bus
{
    /**
     * Command to display a TTIS free text message
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:35 PM
     */

    class TtisFreeTextMessageCommand : public AbstractTtisCommand
    {

    public:

        /**
         * Create a command.
         *
         * @exception ProtocolException for a message too long
         *
         * @param trainId
         * @param origin    Command origin
         * @param pidSelection
         * @param fontSize
         * @param justification
         * @param intensity
         * @param displayMode
         * @param priority
         * @param startTime
         * @param stopTime
         * @param repeatInterval
         * @param message
         */
        TtisFreeTextMessageCommand( CommonTypes::TrainIdType trainId,
                                    unsigned char origin,
                                    ProtocolTtisTypes::ETtisPid pidSelection,
                                    ProtocolTtisTypes::ETtisFontSize fontSize,
                                    ProtocolTtisTypes::ETtisJustification justification,
                                    ProtocolTtisTypes::ETtisIntensity intensity,
                                    ProtocolTtisTypes::ETtisDisplayMode displayMode,
                                    ProtocolTtisTypes::ETtisPriority priority,
                                    const ProtocolTtisTypes::TtisTime& startTime,
                                    const ProtocolTtisTypes::TtisTime& stopTime,
                                    unsigned char repeatInterval,
                                    const std::string& message );

                
        /**
         * Destructor
         */
        virtual ~TtisFreeTextMessageCommand();


        /**
         * Returns a log string with information about this command.
         *
         * @return a string to log
         */
        virtual std::string getLogString();


    private:

        /**
         * Private default constructor
         */
        TtisFreeTextMessageCommand();


        /**
         * combines the display attributes help by this object into a single byte for the message
         *
         * @return the byte representing the combined display effect
         */
        unsigned char combineDisplayAttributes();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * Offset of the PID parameter
         */
        static const unsigned short PID_POSITION;


        /**
         * Offset of the display attributes parameter
         */
        static const unsigned short DISPLAY_ATTRIBUTES_POSITION;


        /**
         * Offset of the priority parameter
         */
        static const unsigned short PRIORITY_POSITION;


        /**
         * Offset of the start hour parameter
         */
        static const unsigned short START_HOUR_POSITION;


        /**
         * Offset of the start minute parameter
         */
        static const unsigned short START_MINUTE_POSITION;


        /**
         * Offset of the stop hour parameter
         */
        static const unsigned short STOP_HOUR_POSITION;


        /**
         * Offset of the stop minute parameter
         */
        static const unsigned short STOP_MINUTE_POSITION;


        /**
         * Offset of the repeat interval parameter
         */
        static const unsigned short REPEAT_INTERVAL_POSITION;


        /**
         * Offset of the message parameter
         */
        static const unsigned short MESSAGE_POSITION;


        /**
         * The longest message that can fit
         */
        static const unsigned short MAX_MESSAGE_LENGTH;


        /**
         * The PID selection
         */
        ProtocolTtisTypes::ETtisPid m_pidSelection;


        /**
         * Message font size
         */
        ProtocolTtisTypes::ETtisFontSize m_fontSize;


        /**
         * Message justification
         */
        ProtocolTtisTypes::ETtisJustification m_justification;


        /**
         * Message intensity
         */
        ProtocolTtisTypes::ETtisIntensity m_intensity;


        /**
         * Message display effect
         */
        ProtocolTtisTypes::ETtisDisplayMode m_displayMode;


        /**
         * message priority
         */
        ProtocolTtisTypes::ETtisPriority m_priority;


        /**
         * start time
         */
        ProtocolTtisTypes::TtisTime m_startTime;


        /**
         * stop hour
         */
        ProtocolTtisTypes::TtisTime m_stopTime;


        /**
         * repeat interval
         */
        unsigned char m_repeatInterval;


        /**
         * message text
         */
        std::string m_message;


        /**
         * The combined display attributes byte
         */
        unsigned char m_displayAttributesByte;

    };


    typedef boost::shared_ptr< TtisFreeTextMessageCommand > TtisFreeTextMessageCommandPtr;

}

#endif // !defined(TtisFreeTextMessageCommand_H)
