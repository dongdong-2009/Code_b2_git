/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:     $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/OverScaleHiAlarmDetails.h $
  * @author:    HoaVu
  * @version:   $Revision: #2 $
  *
  * Last modification:  $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:   $Author: haijun.li $
  *
  * Description:
  *
  * Represents an alarm condition for an over high engineering limit alarm value
  *
  */


#ifndef OVER_SCALE_HI_ALARMDETAILS_H
#define OVER_SCALE_HI_ALARMDETAILS_H

#include "bus/scada/datapoint_library/src/AnalogueAlarmDetails.h"

namespace TA_Base_Bus
{
    class OverScaleHiAlarmDetails : public AnalogueAlarmDetails
    {
    public:

        /**
         * OverScaleHiAlarmDetails
         *
         * @param   dpEntityKey     the pkey for the DataPoint entity that this
         *                          alarm pertains to
         *
         * @param   almEnabled      whether or not to process this alarm
         *
         * @param   almDelay        the alarm state must persist for this period
         *                          of time (in seconds) before an alarm is
         *                          generated
         *
         * @param   almSeverity     the severity of alarms generated as a result
         *                          of this alarm condition
         *
         * @param   almMessage      the message of alarms generated as a result
         *                          of this alarm condition
         *
         * @param   almValue        the value that defines this alarm condition
         *
         */
        OverScaleHiAlarmDetails( TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess,
                                 unsigned long          dpParentEntityKey,
                                 unsigned long          dpEntityKey,
                                 unsigned long          dpEntityTypeKey,
                                 unsigned long          regionKey,
                                 unsigned long          subsystemKey,
                                 const std::string &    entityName,
                                 bool                   almEnabled,
                                 int                    almDelay,
                                 int                    almSeverity,
                                 const std::string &    almMessage,
                                 const std::string &    MMSConfiguration,
                                 const DpValue          almValue,
                                 MmsScadaUtility &      mmsScadaUtility );
        
        virtual ~OverScaleHiAlarmDetails();
        
        virtual std::string getAlarmTypeStr();
    
    protected:

       /*
        * inAlarm()
        *
        * Determines whether a DataPointState constitutes an alarm
        * state or not.
        *
        * @param state     the state to be tested
        *
        */
        virtual bool inAlarm( const DataPointState & state );
    };
}

#endif
