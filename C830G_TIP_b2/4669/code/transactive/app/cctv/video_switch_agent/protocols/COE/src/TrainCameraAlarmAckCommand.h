#ifndef TRAIN_ALARM_ACK_COMMAND_H
#define TRAIN_ALARM_ACK_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/TrainCameraAlarmAckCommand.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

// Forward declarations.

namespace TA_IRS_App
{
    class COEVideoOutput;
}

namespace TA_IRS_App
{
    class TrainCameraAlarmAckCommand : public AbstractCOERequestCommand
    {
    public:

        /** 
          * TrainCameraAlarmAckCommand
          *
          * description
          *
          * @param communicationsChannelIndex
          * @param 
          *
          */
        TrainCameraAlarmAckCommand(unsigned long stageId, unsigned long monitorId);
       
        
        /**
          * ~TrainCameraAlarmAckCommand
          *
          * Standard destructor.
          */
        virtual ~TrainCameraAlarmAckCommand() {};


        /**
          * processResponse
          *
          * Processes the response to this command that has been received from
          * the switching hardware.
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

        //
        // Disable copy constructor and assignment operator.
        //

        TrainCameraAlarmAckCommand( const TrainCameraAlarmAckCommand& theTrainCameraAlarmAckCommand );
        TrainCameraAlarmAckCommand& operator=( const TrainCameraAlarmAckCommand& );


    };
}

#endif // TRAIN_ALARM_ACK_COMMAND_H

