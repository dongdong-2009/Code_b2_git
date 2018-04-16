/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  adamr
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */
#if !defined(EChangeModeCommandResult_H)
#define EChangeModeCommandResult_H

namespace TA_IRS_App
{

    /**
     * Result of a TTIS command
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:47:34 PM
     */
    enum EChangeModeCommandResult
    {
        // The command was successful
        Success = 0,

        // A bad command response was received, with a bad checksum error.
        BadCommandBadChecksum = 1,

        // A bad command response was received, with an unknown command error.
        BadCommandUnknownCommand = 2,

        // A bad command response was received, with a bad parameter error.
        BadCommandBadParameter = 3,

        // A bad command response was received, with an unexpected command error.
        BadCommandUnexpectedCommand = 4,

        // The train was invalid or there are no permissions for the train
        InvalidTrainError = 5,

        // The command could not be sent to the train
        TransmissionFailureError = 6,

        // The train did not respond to the command
        TransactionTimeoutError = 7,

        // The command was cancelled externally
        TransactionCancelledError = 8,

        // The train experienced an MPU changeover during the command execution
        MpuChangeoverError = 9
    };

} // TA_IRS_App

#endif // !defined(EChangeModeCommandResult_H)
