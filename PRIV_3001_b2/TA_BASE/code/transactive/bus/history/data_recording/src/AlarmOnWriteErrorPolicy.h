#ifndef ALARM_ON_WRITE_ERROR_POLICY_H
#define ALARM_ON_WRITE_ERROR_POLICY_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/history/data_recording/src/AlarmOnWriteErrorPolicy.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * Implementation of IDataWritePolicy that opens
 * an alarm when an error occurs, and closes it when
 * a successful write is done.
 *
 */

#include <memory>

#include "bus/history/data_recording/src/IDataWritePolicy.h"

namespace TA_Base_Core
{
    class IEntityData;
    class NonUniqueAlarmHelper;
}

namespace TA_Base_Bus
{
    class AlarmOnWriteErrorPolicy : public IDataWritePolicy
    {

    public:

        /**
         * Constructor
         *
         * @param   entityConfig    entity configuration to use for alarm purposes
         *
         */
        AlarmOnWriteErrorPolicy( TA_Base_Core::IEntityData& entityConfig );


        /**
         * Destructor
         *
         */
        ~AlarmOnWriteErrorPolicy();


        /**
         * notifyDataWriteSucess
         *
         * Callback from the DataRecorder when data is written
         * successfully. Closes an alarm, if one exists.
         *
         */
        virtual void notifyDataWriteSucess();


        /**
         * notifyDataWriteError
         *
         * Callback from the DataRecorder when an error is
         * detected attempting to write data. Throws a
         * DataRecordingException.
         *
         * @param   unwrittenCache     the data that the DataRecorder failed to write
         *
         */
        virtual void notifyDataWriteError( DataCache& unwrittenCache );

    private:

        TA_Base_Core::NonUniqueAlarmHelper&   m_alarmHelper;
        std::string                          m_currentAlarm;
        TA_Base_Core::IEntityData&                          m_entityConfig;
    };
}

#endif // ALARM_ON_WRITE_ERROR_POLICY_H
