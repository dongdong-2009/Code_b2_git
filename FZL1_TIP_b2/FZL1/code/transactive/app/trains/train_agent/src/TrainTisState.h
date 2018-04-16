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
#if !defined(TrainTisState_H)
#define TrainTisState_H

#include "bus/trains/TrainTransactionModel/src/TrainState.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{

    /**
     * Represents the TTIS state of the train
     *
     * This is a local state and is only synchronised between peer agents.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:38 PM
     */
    class TrainTisState : public TA_IRS_Bus::TrainState
    {

    public:

        /**
         * Creates the object with a train ID
         *
         * @param trainId
         */
        TrainTisState( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Destructor
         */
        virtual ~TrainTisState();


        /**
         * Returns the train resource this state represents.
         *
         * @return the resource category this state represents.
         */
        virtual TA_IRS_Bus::CommonTypes::ETrainResource getResourceCategory() const;


        /**
         * This gets the lock used for sending PEC commands.
         * This lock is acquired for the duration of a PEC operation to prevent multiple
         * PEC commands being sent to the same train at the same time.
         *
         * @return the lock to acquire while sending commands to this train
         */
        TA_Base_Core::IThreadLockable& getCommandLock();


        /**
         * Gets the current download occurring on this train.
         *
         * @return the current download occurring on this train if any
         */
        TA_IRS_Bus::TtisTypes::ETtisDownloadType getCurrentDownloadType() const;


        /**
         * Gets the version of the current download occurring on this train.
         *
         * @return the version of the current download occurring on this train, 0 if none
         */
        unsigned short getCurrentDownloadVersion() const;


        /**
         * Gets the data that is being downloaded to the train
         *
         * @return the data that is being downloaded to the train if any
         */
        const TA_IRS_Bus::TtisTypes::TtisFrameList& getCurrentDownloadData() const;


        /**
         * If there is a circuit mode transfer in progress, get the TSI this transfer is to.
         *
         * @return the TSI of the current circuit mode transfer to this train if any
         */
        std::string getTransferTsi() const;


        /**
         * If there is a download in progress, get the session ID of the operator that started it
         *
         * @return the session ID of the operator that started the current download, empty if none
         */
        std::string getSessionId() const;


        /**
         * Gets a particular frame of the data that is being downloaded to the train
         *
         * @exception InvalidParameterException if the frame is not found.
         *
         * @param frameNumber    The sequence number of the frame to get
         */
        const TA_IRS_Bus::TtisTypes::TtisFrameData& getDownloadFrame( unsigned short frameNumber ) const;


        /**
         * Sets the library and the library type being downloaded to the train.
         *
         * @param sessionId    The session ID of the operator who started the transfer
         * @param downloadType    Whether a library or schedule is being downloaded
         * @param downloadVersion    The version being downloaded to the train
         * @param data    The data being sent
         * @param transferTsi    The TSI the transfer is to
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setDownload( const std::string& sessionId,
                          TA_IRS_Bus::TtisTypes::ETtisDownloadType downloadType,
                          unsigned short downloadVersion,
                          const TA_IRS_Bus::TtisTypes::TtisFrameList& data,
                          const std::string& transferTsi,
                          time_t timestamp );


        /**
         * In case a second transfer needs to be started after one is complete (eg repetition)
         * this allows to set the transfer TSI to indicate a transfer is in progress.
         *
         * @param transferTsi    The TSI the transfer is to
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setTransferTsi( const std::string& transferTsi,
                             time_t timestamp );


        /**
         * The download has finished, it can be cleared, along with the data.
         *
         * @param timestamp    The time the download completed
         */
        void clearDownload( time_t timestamp );


        /**
         * The actual transfer has finished, the TSI can be cleared,
         * however the download process may still be in progress
         *
         * @param timestamp    The time the download completed
         */
        void transferComplete( time_t timestamp );


        /**
         * Gets the state for synchronisation purposes
         *
         * @return the state needing to be synchronised
         */
        TA_IRS_Bus::StateUpdateTypes::TrainTisSyncState getSyncState() const;


        /**
         * Sets the sync state - for synchronisation
         *
         * @param state    The new internal state
         */
        void setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainTisSyncState& state );


    private:

        /**
         * Private default constructor
         */
        TrainTisState();


        /**
         * This is the lock used for sending commands.
         * This lock is acquired for the duration of an operation to prevent multiple
         * commands of the same type being sent to the same train at the same time.
         */
        TA_Base_Core::NonReEntrantThreadLockable m_commandLock;


        /**
         * This is the type of library being downloaded to the train currently
         */
        TA_IRS_Bus::TtisTypes::ETtisDownloadType m_downloadType;


        /**
         * This is the list of frames being downloaded to the train currently. Used for repetition requests.
         * The frames are stored in order, so frame 1 at position 0 and so on.
         */
        TA_IRS_Bus::TtisTypes::TtisFrameList m_currentDownload;


        /**
         * This is the version currently being downloaded to the train.
         */
        unsigned short m_currentDownloadVersion;


        /**
         * The session ID of the operator who initiated the download
         */
        std::string m_sessionId;


        /**
         * The TSI of any active transfer.
         * This needs to be a TSI rather than a flag because in the case of
         * MPU failure the transfer needs to be stopped on the correct TSI
         */
        std::string m_transferTsi;

    };


    /**
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:38 PM
     */
    typedef boost::shared_ptr< TrainTisState > TrainTisStatePtr;

} // TA_IRS_App

#endif // !defined(TrainTisState_H)
