/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/TrainTimeScheduleManager.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This is used to manage the current list of time schedules.
  * Windows will use it to retrieve the list, and to load time schedules.
  *
  * Interested windows will receive a message when the list of time schedules change.
  * WM_UPDATE_TIMESCHEDULE
  * 
  * Interested windows will also receive a message when the current time schedule version
  * changes. WM_UPDATE_CURRENT_TIME_SCHEDULE
  */

#if !defined(TRAINTIMESCHEDULEMANAGER_H)
#define TRAINTIMESCHEDULEMANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/tis_agent/src/ITrainTimeSchedule.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/idl/src/CommsMessageCorbaDef.h"

#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

// forward declarations
namespace TA_Base_Bus
{
    class ScadaProxyFactory;
}

namespace TA_IRS_App
{
    class TrainTimeScheduleManager : public TA_Base_Bus::IEntityUpdateEventProcessor,
                                     public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
        
    public:

        // a map of time schedule names. Name->pkey
        typedef std::map<std::string, unsigned long> TimeScheduleMap;


        /**
         * ~TrainTimeScheduleManager
         *
         * Standard destructor.
         */

        virtual ~TrainTimeScheduleManager();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      TrainTimeScheduleManager&
         *              A reference to an instance of a TrainTimeScheduleManager object.
         */
		static TrainTimeScheduleManager& getInstance();

        
        /**
         * removeInstance
         *
         * cleans up the singleton
         */
		static void removeInstance();


        /** 
          * registerCurrentTimeScheduleUser
          *
          * When the current train time schedule version is updated, window
          * will be given a WM_UPDATE_CURRENT_TIME_SCHEDULE message.
          *
          * @param window       The window to be notified of change.
          *
          */
        void registerCurrentTimeScheduleUser(CWnd* window);


        /** 
          * deregisterNextVersionUser
          *
          * window is no longer interested in being notified of current time schedule version changes.
          *
          * @param window       The window to be removed
          */
        void deregisterCurrentTimeScheduleUser(CWnd* window);


        /** 
          * registerForTimeScheduleChanges
          *
          * When a train time schedule is created, updated, or deleted window
          * will be given a WM_UPDATE_TIMESCHEDULE message.
          * wParam will be a pointer to a TA_Base_Core::TimeScheduleChange structure
          * which must be deleted when done or it will cause memory leaks.
          * lParam will be a boolean telling whether the time schedule name list
          * needs reloading - ie has the name changed.
          *
          * @param window       The window to be notified of change.
          *
          */
        void registerForTimeScheduleChanges(CWnd* window);


        /** 
          * deregisterForTimeScheduleChanges
          *
          * window is no longer interested in being notified of time schedule changes.
          *
          * @param window       The window to be removed
          */
        void deregisterForTimeScheduleChanges(CWnd* window);


        /** 
          * getCurrentTimeScheduleKey
          *
          * This returns the pkey of the current train time schedule
          *
          * @return current version of the pre-defined message library
          */
        inline unsigned long getCurrentTimeScheduleKey()
        {
            return m_currentTimeScheduleKey;
        }


         /** 
          * getCurrentTimeScheduleVersion
          *
          * This returns the ISCS version of the current train time schedule
          *
          * @return version of the current time schedule
          */
        inline unsigned short getCurrentTimeScheduleVersion()
        {
            return m_currentTimeScheduleVersion;
        }


        /** 
          * getCurrentTimeScheduleNames
          *
          * gets the list of time schedule names.
          *
          *
          * @return a map of schedule names to their pkey.
          */
        inline TimeScheduleMap getCurrentTimeScheduleNames()
        {
            TA_Base_Core::ThreadGuard guard(m_scheduleMapLock);

            return m_scheduleMap;
        }


        /** 
          * getTrainTimeSchedule
          *
          * Gets a time schedule by its name.
          * The pointer becomes the responsibility of the caller (to own and delete when done).
          *
          * @param name The name from the time schedule name list
          *
          * @return a pointer to the time schedule object. NULL if not found.
          *
          * @exception   DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        TA_IRS_Core::ITrainTimeSchedule* getTrainTimeSchedule(std::string name);


        /** 
          * getTrainTimeSchedule
          *
          * Gets a time schedule by its key.
          * The pointer becomes the responsibility of the caller (to own and delete when done).
          *
          * @param key The key from the time schedule name list
          *
          * @return a pointer to the time schedule object. NULL if not found.
          *
          * @exception   DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        TA_IRS_Core::ITrainTimeSchedule* getTrainTimeSchedule(unsigned long key);


        /** 
          * createNewTrainTimeSchedule
          *
          * Gets a new time schedule.
          * The pointer becomes the responsibility of the caller (to own and delete when done).
          *
          * @return a pointer to the new time schedule object.
          */
        TA_IRS_Core::ITrainTimeSchedule* createNewTrainTimeSchedule();


        /** 
          * copyTrainTimeSchedule
          *
          * Gets a new time schedule as a clone of the old one.
          * The pointer becomes the responsibility of the caller (to own and delete when done).
          *
          * @return a pointer to the new time schedule object.
          */
        TA_IRS_Core::ITrainTimeSchedule* copyTrainTimeSchedule(TA_IRS_Core::ITrainTimeSchedule* toClone);


        /** 
          * saveTimeSchedule
          *
          * Saves the given time schedule (new or used) and notify other GUIS
          * (and hence this one) of the change.
          *
          * @param scheduleToSave   A pointer to the time schedule to save
          *
          * @exception DataConfigurationException   if there is an error in the time schedule
          * @exception DataException                a database constraint was broken etc.
          * @exception DatabaseException            error executing sql. Or database down.
          */
        void saveTimeSchedule(TA_IRS_Core::ITrainTimeSchedule* scheduleToSave);


        /** 
          * deleteTimeSchedule
          *
          * Deletes the given time schedule used and notify other GUIS
          * (and hence this one) of the change.
          *
          * @param scheduleToSave   A pointer to the time schedule to delete
          *
          * @exception DataException                a database constraint was broken etc.
          * @exception DatabaseException            error executing sql. Or database down.
          */
        void deleteTimeSchedule(TA_IRS_Core::ITrainTimeSchedule* scheduleToDelete);


        /** 
          * processEntityUpdateEvent
          *
          * implemented from IEntityUpdateEventProcessor.
          * called when a datapoint is updated.
          *
          * @param entityKey
          * @param updateType
          *
          */
        virtual void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);


        /** 
          * receiveSpecialisedMessage
          *
          * receives comms messages.
          * It will be how this gui finds out about time schedule changes.
          *
          * @param the message
          */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message );
        
    private:
        
        /**
         * TrainTimeScheduleManager
         *
         * Private constructors.
         */
        TrainTimeScheduleManager();
        TrainTimeScheduleManager& operator=(const TrainTimeScheduleManager &);
        TrainTimeScheduleManager( const TrainTimeScheduleManager& );

        static const std::string CURRENT_TIME_SCHEDULE_VERSION_DP_NAME;


        /**
          * registerForTimeSceduleVersionChanges
          *
          * registers for datapoint state changes for the time schedule version.
          *
          */
        void registerForTimeSceduleVersionChanges();


        /** 
          * notifyWindowsOfTimeScheduleVersionChange
          *
          * Sends an update message to all interested windows
          * telling them the current time schedule version has changed.
          *
          */
        void notifyWindowsOfTimeScheduleVersionChange();


        /** 
          * notifyWindowsOfTimeScheduleChange
          *
          * Sends an update message to all interested windows
          * telling them that a time schedule has changed.
          * they also receive a pointer to their very own copy of scheduleChange
          * which must they must delete when done.
          *
          * @param scheduleChange   the structure to copy and send
          * @param bool scheduleNameListChanged Whether the list of names needs reloading
          */
        void notifyWindowsOfTimeScheduleChange(const TA_Base_Core::TimeScheduleChange& scheduleChange,
                                               bool scheduleNameListChanged);


        /** 
          * postMessageToWindows
          *
          * post the given message to the given vector of windows
          *
          * @param windows  A vector of windows to post the message to
          * @param message  The message number to post
          *
          */
        void postMessageToWindows(const std::vector<CWnd*>& windows, int message);


        /** 
          * loadTimeScheduleNames
          *
          * load the name and pkey of each time schedule in the database
          *
          */
        void loadTimeScheduleNames();


        /** 
          * registerForTimeScheduleChanges
          *
          * registers for messages indicating a time schedule has changed.
          *
          */
        void registerForTimeScheduleChanges();


        /** 
          * handleTimeScheduleChange
          *
          * When a time schedule changes, decide whether the name
          * list needs loading, and reload it if necessary.
          * Also send the message to each window notifying of hte change.
          *
          * @param pkey         The pkey of the changed time schedule
          * @param changeType   What was done to it.
          *
          */
        void handleTimeScheduleChange( unsigned long pkey,
                                       TA_Base_Core::ETimeScheduleChangeType changeType);


        // for internal use, a 'mirror' of the other map type.
        typedef std::map<unsigned long, std::string> ReverseTimeScheduleMap;

        // class instance
        static TrainTimeScheduleManager* m_theClassInstance;

        // reference count
        static unsigned int m_refCounter;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

        // the current time schedule key and version
        unsigned long m_currentTimeScheduleKey;
        unsigned short m_currentTimeScheduleVersion;

        // the time schedules name->pkey
        TimeScheduleMap m_scheduleMap;
        // then the other way around
        ReverseTimeScheduleMap m_reverseScheduleMap;

        // the last outstanding time schedule change
        // so this gui can be updated instantly, in case the message lags a bit.
        unsigned long m_lastScheduleChangedPkey;
        TA_Base_Core::ETimeScheduleChangeType m_lastOperation;

        // the datapoint proxy for the current time schedule version
        TA_Base_Bus::DataPointProxySmartPtr m_currentTimeScheduleVersionDp;
        
        // the windows that must be notified of updates to the time schedule version
        std::vector<CWnd*> m_tsvWindowsToNotify;
        // the windows that must be notified of updates to time schedules
        std::vector<CWnd*> m_scheduleChangeWindowsToNotify;

        // threadlock for registration/deregistration
        TA_Base_Core::NonReEntrantThreadLockable m_windowListLock;

        // threadlock for schedule name list
        TA_Base_Core::NonReEntrantThreadLockable m_scheduleMapLock;

        // threadlock for last operation
        TA_Base_Core::NonReEntrantThreadLockable m_lastOperationLock;

        TA_Base_Bus::ScadaProxyFactory& m_scadaProxyFactory;

    };

} // end namespace TA_IRS_App

#endif 
