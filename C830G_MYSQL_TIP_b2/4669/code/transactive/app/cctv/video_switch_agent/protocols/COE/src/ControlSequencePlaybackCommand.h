#ifndef CONTROL_SEQUENCE_PLAYBACK_COMMAND_H
#define CONTROL_SEQUENCE_PLAYBACK_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/ControlSequencePlaybackCommand.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Object for requesting the playback of a Sequence on a Monitor be set to a 
  * particular mode, and for processing the response.
  *
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"

// Forward declarations.

namespace TA_IRS_App
{
    class COESequence;
}

namespace TA_IRS_App
{
    class ControlSequencePlaybackCommand : public AbstractCOERequestCommand
    {
    public:

        enum ESequencePlaybackCommandType
        {
            SEQUENCE_HOLD,
            SEQUENCE_SKIP_FORWARD,
            SEQUENCE_SKIP_BACKWARD,
            SEQUENCE_PLAY
        };

        /**
          * ControlSequencePlaybackCommand
          *
          * Constructs the command to send to the switching hardware regarding
          * the switch of an input to an output.
          *
          * @param      ESequencePlaybackCommand sequencePlaybackCommand
          *             The sequence playback command type to be sent.
          * @param      unsigned long videoOutputEntityKey
          *             The entity key of the VideoOutput the Sequence Playback 
          *             being controlled is on.
          * @param      COESequence& sequence
          *             The Sequence to control playback for.
          *
          * @exception  DatabaseException
          *             Thrown if the database cannot be contacted.
          * @exception  DataException
          *             Thrown if the data cannot be retrieved or is invalid.
          */

        ControlSequencePlaybackCommand( ESequencePlaybackCommandType sequencePlaybackCommandType, unsigned long videoOutputEntityKey );

        /**
          * ~ControlSequencePlaybackCommand
          *
          * Standard destructor.
          */

        virtual ~ControlSequencePlaybackCommand() {};

        /**
          * processResponse
          *
          * Processes the response to this command that has been received from
          * the switching hardware. The checksum for this response will already 
          * have been checked and removed.
          *
          * @return     bool
          *             True if the response was valid.
          *             False otherwise.
          *
          * @param      DataVector& data
          *             The COE to process.  This will have the checksum removed.
          */

        virtual bool processResponse( const std::vector< unsigned char >& data );

    private:

        unsigned char m_expectedResponseCmd;

        //
        // Disable copy constructor and assignment operator.
        //

        ControlSequencePlaybackCommand( const ControlSequencePlaybackCommand& theControlSequencePlaybackCommand );
        ControlSequencePlaybackCommand& operator=( const ControlSequencePlaybackCommand& );

        //
        // The playback command type, VideoOutput entitykey, and the Sequence object that will be set.
        //

        ESequencePlaybackCommandType m_sequencePlaybackCommandType;
        unsigned long m_videoOutputEntityKey;
    };
}

#endif // CONTROL_SEQUENCE_PLAYBACK_COMMAND_H
