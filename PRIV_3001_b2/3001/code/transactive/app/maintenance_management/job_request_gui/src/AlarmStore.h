/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/job_request_gui/src/AlarmStore.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/03/20 16:13:46 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  * 
  * This class provides the interface to the alarm store. It connects to it and sends information
  * to it.
  */

#ifndef ALARM_STORE_MON_09_02_2004_H
#define ALARM_STORE_MON_09_02_2004_H

#include <string>

#include "core/alarm/IDL/src/AlarmStoreCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_IRS_App
{
    class AlarmStore
    {
    public:
       
        AlarmStore();

        /**
         * Destructor
         */
        ~AlarmStore();

        /**
         * getAlarm
         *
         * This will attempt to retrieve the alarm details for the alarm with the specified
         * UUID. It will first look in the cache and if it cannot be found then retrieve it
         * from the alarm store.
         * NOTE: The caller must delete the pointer returned.
         *
         * @param string - The alarm UUID
         *
         * @return AlarmDetailCorbaDef* - The alarm details
         *
         * @exception AlarmListCtrlException - Thrown if the alarm cannot be found
         */
        TA_Base_Core::AlarmDetailCorbaDef* getAlarm(std::string alarmUUID);


         /**
         * isAlarmStoreValid
         *
         * This indicates if the alarm store is currently valid. If it is not (ie false)
         * then no alarms or alarm updates are being retrieved or sent.
         *
         * @return bool - True if the alarm store is valid and functioning.
         */
        bool isAlarmStoreValid() const
        {
            return m_bAlarmStoreValid;
        };

    private:

        volatile bool m_bAlarmStoreValid;
        
        // This is the alarm store itself.
        TA_Base_Core::IAlarmStoreCorbaDef_var m_alarmStore;
        
        // The thread lock for the m_alarmStore
        mutable TA_Base_Core::NonReEntrantThreadLockable m_alarmStoreLock;		
    };
}

#endif // ALARM_STORE_MON_09_02_2004_H
