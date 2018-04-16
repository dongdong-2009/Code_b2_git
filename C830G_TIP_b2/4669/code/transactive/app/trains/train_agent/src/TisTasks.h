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
#if !defined(TisTasks_H)
#define TisTasks_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"

#include "core/threads/src/IWorkItem.h"

// forward declarations
namespace TA_IRS_App
{
    class ITimsTisManagerTaskCallback;
}

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:35 PM
     */
    class FreeTextMessageTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    The callback to do the actual work
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param displayParameters    The parameters required to display the message
         * @param timestamp    The time of the request submission. This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        FreeTextMessageTask( ITimsTisManagerTaskCallback& callback,
                             TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                             unsigned char origin,
                             const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                             time_t timestamp,
                             const std::string& sessionId );


        /**
         * Destructor
         */
        virtual ~FreeTextMessageTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        FreeTextMessageTask();


        /**
         * The callback to do the actual work
         */
        ITimsTisManagerTaskCallback& m_callback;


        /**
         * The train the operation is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * the operator's origin value
         */
        unsigned char m_origin;


        /**
         * The parameters required to display the message
         */
        TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters m_displayParameters;


        /**
         * The time of the request submission.
         * This will be set along with the session ID in the result message
         */
        time_t m_timestamp;


        /**
         * The operator submitting the message
         */
        std::string m_sessionId;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:36 PM
     */
    class StoredMessageTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    The callback to do the actual work
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param displayParameters    The predefined display parameters
         * @param timestamp    The time of the request submission. This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        StoredMessageTask( ITimsTisManagerTaskCallback& callback,
                           TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                           unsigned char origin,
                           const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                           time_t timestamp,
                           const std::string& sessionId );


        /**
         * Destructor
         */
        virtual ~StoredMessageTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        StoredMessageTask();


        /**
         * The callback to do the actual work
         */
        ITimsTisManagerTaskCallback& m_callback;


        /**
         * The train the operation is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * the operator's origin value
         */
        unsigned char m_origin;


        /**
         * The predefined display parameters
         */
        TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters m_displayParameters;


        /**
         * The time of the request submission.
         * This will be set along with the session ID in the result message
         */
        time_t m_timestamp;


        /**
         * The operator submitting the message
         */
        std::string m_sessionId;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:36 PM
     */
    class ResetMessageTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    The callback to do the actual work
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param displayParameters    The parameters for the reset message
         * @param timestamp    The time of the request submission.
         *                     This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        ResetMessageTask( ITimsTisManagerTaskCallback& callback,
                          TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                          unsigned char origin,
                          const TA_IRS_Bus::TtisTypes::TtisResetParameters& displayParameters,
                          time_t timestamp,
                          const std::string& sessionId );


        /**
         * Destructor
         */
        virtual ~ResetMessageTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();

    private:


        /**
         * Private default constructor
         */
        ResetMessageTask();


        /**
         * The callback to do the actual work
         */
        ITimsTisManagerTaskCallback& m_callback;


        /**
         * The train the operation is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * the operator's origin value
         */
        unsigned char m_origin;


        /**
         * The parameters for the reset message
         */
        TA_IRS_Bus::TtisTypes::TtisResetParameters m_displayParameters;


        /**
         * The time of the request submission.
         * This will be set along with the session ID in the result message
         */
        time_t m_timestamp;


        /**
         * The operator submitting the message
         */
        std::string m_sessionId;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:35 PM
     */
    class LibraryDownloadTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    The callback to do the actual work
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param libraryData    The message library data to download to the train
         * @param libraryVersion    The library version of the library Data
         * @param sessionId    The operator initiating the download
         */
        LibraryDownloadTask( ITimsTisManagerTaskCallback& callback,
                             TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                             unsigned char origin,
                             const TA_IRS_Bus::TtisTypes::LibraryDataType& libraryData,
                             unsigned short libraryVersion,
                             const std::string& sessionId );


        /**
         * Destructor
         */
        virtual ~LibraryDownloadTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        LibraryDownloadTask();


        /**
         * The callback to do the actual work
         */
        ITimsTisManagerTaskCallback& m_callback;


        /**
         * The train the operation is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * the operator's origin value
         */
        unsigned char m_origin;


        /**
         * The message library data to download to the train
         */
        TA_IRS_Bus::TtisTypes::LibraryDataType m_libraryData;


        /**
         * The library version of the library Data
         */
        unsigned short m_libraryVersion;


        /**
         * The operator initiating the download
         */
        std::string m_sessionId;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:35 PM
     */
    class LibraryUpgradeTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    The callback to do the actual work
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param sessionId    The operator initiating the upgrade
         */
        LibraryUpgradeTask( ITimsTisManagerTaskCallback& callback,
                            TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                            unsigned char origin,
                            const std::string& sessionId );


        /**
         * Destructor
         */
        virtual ~LibraryUpgradeTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        LibraryUpgradeTask();


        /**
         * The callback to do the actual work
         */
        ITimsTisManagerTaskCallback& m_callback;


        /**
         * The train the operation is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * the operator's origin value
         */
        unsigned char m_origin;


        /**
         * The operator initiating the upgrade
         */
        std::string m_sessionId;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:35 PM
     */
    class ScheduleDownloadTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    The callback to do the actual work
         * @param trainId    The train the operation is for
         * @param origin    the operator's origin value
         * @param scheduleData    The time schedule data to download to the train
         * @param scheduleVersion    The version of the time schedule data
         * @param sessionId    The operator initiating the download
         */
        ScheduleDownloadTask( ITimsTisManagerTaskCallback& callback,
                              TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                              unsigned char origin,
                              const TA_IRS_Bus::TtisTypes::LibraryDataType& scheduleData,
                              unsigned short scheduleVersion,
                              const std::string& sessionId );


        /**
         * Destructor
         */
        virtual ~ScheduleDownloadTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        ScheduleDownloadTask();


        /**
         * The callback to do the actual work
         */
        ITimsTisManagerTaskCallback& m_callback;


        /**
         * The train the operation is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * the operator's origin value
         */
        unsigned char m_origin;


        /**
         * The time schedule data to download to the train
         */
        TA_IRS_Bus::TtisTypes::LibraryDataType m_scheduleData;


        /**
         * The version of the time schedule data
         */
        unsigned short m_scheduleVersion;


        /**
         * The operator initiating the download
         */
        std::string m_sessionId;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:35 PM
     */
    class ScheduleUpgradeTask : public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Cosntructor
         *
         * @param callback    The callback to do the actual work
         * @param trainId    the operator's origin value
         * @param origin
         * @param sessionId    the operator initiating the upgrade
         */
        ScheduleUpgradeTask( ITimsTisManagerTaskCallback& callback,
                             TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                             unsigned char origin,
                             const std::string& sessionId );


        /**
         * Destructor
         */
        virtual ~ScheduleUpgradeTask();


        /**
         * Performs the actual task
         */
        virtual void executeWorkItem();


    private:


        /**
         * Private default constructor
         */
        ScheduleUpgradeTask();


        /**
         * The callback to do the actual work
         */
        ITimsTisManagerTaskCallback& m_callback;


        /**
         * The train the operation is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * the operator's origin value
         */
        unsigned char m_origin;


        /**
         * the operator initiating the upgrade
         */
        std::string m_sessionId;

    };

}

#endif // !defined(TisTasks_H)
