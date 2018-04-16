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
#if !defined(ITimsDriverCallManagerTaskCallback_H)
#define ITimsDriverCallManagerTaskCallback_H

#include "app/trains/train_agent/src/TrainVoiceCallRequest.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"


namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:46 PM
     */
    class ITimsDriverCallManagerTaskCallback
    {

    public:

        /**
         * This is called to send the required command to TIMS once a call to a train has ended.
         * The call has already ended, so there is no command sent to the radio to end the call
         * It clears the state of any calls or incoming calls.
         * It will then send the end of voice call command to the train
         *
         * @param trainId    The train to end the call for
         */
        virtual void performTimsCallReset( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) = 0;


        /**
         * This is called to send the required command to TIMS to set up a call to a train.
         * The command is sent to the train, and the response processed.
         * The given request object is updated with the status.
         *
         * @param voiceCallRequest    The object containing all information for the request, and which will also be used to store the result.
         */
        virtual void performTimsCallSetup( TrainVoiceCallRequestPtr voiceCallRequest ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsDriverCallManagerTaskCallback_H)
