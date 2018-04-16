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
#if !defined(AbstractPecCommand_H)
#define AbstractPecCommand_H


#include "bus/trains/TrainProtocolLibrary/src/TimsCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"


namespace TA_IRS_Bus
{

    /**
     * All PEC commands inherit from this. They all specify a PEC number.
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:51 PM
     */

    class AbstractPecCommand : public TimsCommand
    {

    public:

        /**
         * Create a PEC command.
         *
         * @param trainId
         * @param messageType
         * @param origin    Command origin
         * @param carNumber
         * @param pecNumber
         * @param responseExpected
         */
        AbstractPecCommand( CommonTypes::TrainIdType trainId,
                            TrainMessageType messageType,
                            unsigned char origin,
                            ProtocolPecTypes::EPecCarNumber carNumber,
                            ProtocolPecTypes::EPecNumber pecNumber,
                            bool responseExpected );


        /**
         * Destructor
         */
        virtual ~AbstractPecCommand();


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
        AbstractPecCommand();


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * The offset of PEC number
         */
        static const unsigned short PEC_NUMBER_POSITION;

    protected:

        /**
         * The car number
         */
        ProtocolPecTypes::EPecCarNumber m_carNumber;


        /**
         * The PEC number within the car
         */
        ProtocolPecTypes::EPecNumber m_pecNumber;


        /**
         * The combined PEC byte for outputting to the message
         */
        unsigned char m_combinedPecByte;

    };


    typedef boost::shared_ptr< AbstractPecCommand > AbstractPecCommandPtr;

}

#endif // !defined(AbstractPecCommand_H)
