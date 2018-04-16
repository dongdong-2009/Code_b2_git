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
#if !defined(CCTVQUADCOMMAND_H)
#define CCTVQUADCOMMAND_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractCctvCommand.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Command to select a quad
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:00 PM
     */

    class CctvQuadCommand : public AbstractCctvCommand
    {

    public:

        
        /**
         * Create a carrier on command.
         *
         * @param trainId
         * @param origin    Command origin
         * @param cameraW
         * @param cameraX
         * @param cameraY
         * @param cameraZ
         */
        CctvQuadCommand( CommonTypes::TrainIdType trainId,
                         unsigned char origin,
                         ProtocolCctvTypes::ECctvCamera cameraW,
                         ProtocolCctvTypes::ECctvCamera cameraX,
                         ProtocolCctvTypes::ECctvCamera cameraY,
                         ProtocolCctvTypes::ECctvCamera cameraZ );


        /**
         * Destructor
         */
        virtual ~CctvQuadCommand();


        /**
         * Gets the camera set in this command
         *
         * @return the camera W number
         */
        ProtocolCctvTypes::ECctvCamera getCameraW();


        /**
         * Gets the camera set in this command
         *
         * @return the camera X number
         */
        ProtocolCctvTypes::ECctvCamera getCameraX();


        /**
         * Gets the camera set in this command
         *
         * @return the camera Y number
         */
        ProtocolCctvTypes::ECctvCamera getCameraY();


        /**
         * Gets the camera set in this command
         *
         * @return the camera Z number
         */
        ProtocolCctvTypes::ECctvCamera getCameraZ();


        /**
         * Returns a loggable string with information about this command.
         *
         * @return a string to log
         */
        virtual std::string getLogString();


    private:

        /**
         * Private default constructor
         */
        CctvQuadCommand();


        /**
         * The type of this message
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * The length of this message
         */
        static const unsigned short MESSAGE_LENGTH;


        /**
         * position of camera W field
         */
        static const unsigned short CAMERA_W_POSITION;


        /**
         * position of camera X field
         */
        static const unsigned short CAMERA_X_POSITION;


        /**
         * position of camera Y field
         */
        static const unsigned short CAMERA_Y_POSITION;


        /**
         * position of camera Z field
         */
        static const unsigned short CAMERA_Z_POSITION;


        /**
         * Camera W field
         */
        ProtocolCctvTypes::ECctvCamera m_cameraW;


        /**
         * Camera X field
         */
        ProtocolCctvTypes::ECctvCamera m_cameraX;


        /**
         * Camera Y field
         */
        ProtocolCctvTypes::ECctvCamera m_cameraY;


        /**
         * Camera Z field
         */
        ProtocolCctvTypes::ECctvCamera m_cameraZ;

    };


    typedef  boost::shared_ptr< CctvQuadCommand > CctvQuadCommandPtr;

}

#endif // !defined(CctvQuadCommand_H)
