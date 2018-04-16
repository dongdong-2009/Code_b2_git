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
#if !defined(CCTVSINGLECOMMAND_H)
#define CCTVSINGLECOMMAND_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractCctvCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Command to select a single camera
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:01 PM
     */

    class CctvSingleCommand : public AbstractCctvCommand
    {

    public:

        /**
         * Create a carrier on command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param camera
         */
        CctvSingleCommand( CommonTypes::TrainIdType trainId,
                           unsigned char origin,
                           ProtocolCctvTypes::ECctvCamera camera );


        /**
         * Destructor
         */
        virtual ~CctvSingleCommand();


        /**
         * Gets the camera set in this command
         *
         * @return the camera number
         */
        ProtocolCctvTypes::ECctvCamera getCamera();


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
        CctvSingleCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * position of camera field
         */
        static const unsigned short CAMERA_POSITION;


        /**
         * Camera field
         */
        ProtocolCctvTypes::ECctvCamera m_camera;

    };


    typedef  boost::shared_ptr< CctvSingleCommand > CctvSingleCommandPtr;

}

#endif // !defined(CctvSingleCommand_H)
