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
#if !defined(PersistedTrainState_H)
#define PersistedTrainState_H


#include "core/data_access_interface/TrainStateDai/src/IPersistedTrainState.h"
#include "core/data_access_interface/src/IData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <vector>

namespace TA_IRS_Core
{
    /**
     * @author Adam Radics
     * @version 1.0
     * @created 31-Aug-2007 2:18:38 PM
     */
    class PersistedTrainState : public IPersistedTrainState
    {

    public:
       
        typedef std::vector<std::string> ColumnNames;
        /**
         * Loads all persisted train state objects from the database
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */        
        static const IPersistedTrainStateMap loadAllTrainStates();
        

        /**
         * This save the persisted states of a single train
         * (whether it previously existed or not)
         *
         * @param trainId               The Train ID
         * @param primaryTsi            The primary TSI
         * @param secondaryTsi          The secondary TSI
         * @param paLibraryVersion      The PA library version
         * @param ttisLibraryVersion    The TTIS library version
         * @param ttisTimeSchedule      The TTIS time schedule version
         * @param timestamp             The modification time        
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */        
        static void saveTrainState( unsigned char trainId,
                                    const std::string& primaryTsi,
                                    const std::string& secondaryTsi,
                                    unsigned short paLibraryVersion,
                                    unsigned short ttisLibraryVersion,                                    
                                    unsigned short ttisTimeSchedule,
                                    unsigned short nextttisLibraryVersion,
                                    unsigned short nextttisTimeSchedule,
                                    time_t timestamp );
        

        /**
         * Destructor
         */
        virtual ~PersistedTrainState();


        /**
         * Constructor with all information
         *
         * @param data    The raw data
         * @param row     The row within the raw data to load
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        PersistedTrainState(TA_Base_Core::IData& data, unsigned int row);


        /**
         * Gets the train Id
         * 
         * @return the train ID
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual unsigned char getTrainId();

        
        /**
         * Gets theprimary train TSI
         * 
         * @return the train TSI
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual const std::string& getPrimaryTsi();

        
        /**
         * Gets the secondary train TSI
         * 
         * @return the train TSI
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual const std::string& getSecondaryTsi();


        /**
         * Gets the pa library that is on the train
         * 
         * @return the version of the PA library
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual unsigned short getPaLibraryVersion();


        /**
         * Gets the TTIS library that is on the train
         * 
         * @return the version of the TTIS library
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual unsigned short getTtisLibraryVersion();

        virtual unsigned short getNextTtisLibraryVersion();


        /**
         * Gets the TTIS schedule that is on the train
         * 
         * @return the version of the TTIS schedule
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual unsigned short getTtisScheduleVersion();

        virtual unsigned short getNextTtisScheduleVersion();


        /**
         * Gets the timestamp of this train data
         * 
         * @return the time this data was last updated
         * 
         * @exception DataException if there was a problem with the 
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual time_t getTimestamp();


    protected:

        /**
         * This gets the column names to be used when decoding the data
         */
        static const ColumnNames& getColumnNames();

    private:
        
        PersistedTrainState();

        
        /**
         * Loads and populates the fields from the given data
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param data
         * @param row
         */
        void loadData(TA_Base_Core::IData& data, unsigned int row);

		//hongzhi
		static std::string getLocalDatabaseName();
        
        /**
         * These are constants to define the database structure
         */
        static const std::string TABLE_NAME;
        static const std::string TRAIN_ID_COLUMN;
        static const std::string PRIMARY_TSI_COLUMN;
        static const std::string SECONDARY_TSI_COLUMN;
        static const std::string PA_LIBRARY_VERSION_COLUMN;
        static const std::string TTIS_LIBRARY_VERSION_COLUMN;
        static const std::string TTIS_SCHEDULE_VERSION_COLUMN;
        static const std::string NEXT_TTIS_LIBRARY_VERSION_COLUMN;
        static const std::string NEXT_TTIS_SCHEDULE_VERSION_COLUMN;
        static const std::string TIMESTAMP_COLUMN;
        

        /**
         * lock for initialisation of static column names vector
         */
        static TA_Base_Core::NonReEntrantThreadLockable s_columnNamesLock;

        /**
         * The vector of column names used to load data from the database
         */
        static ColumnNames s_columnNames;

        /**
         * The Train ID
         */
        unsigned char m_trainId;

        /**
         * The Primary TSI
         */
        std::string m_primaryTsi;

        /**
         * The Secondary TSI
         */
        std::string m_secondaryTsi;

        /**
         * The PA library version
         */
        unsigned short m_paLibraryVersion;

        /**
         * The TTIS library version
         */
        unsigned short m_ttisLibraryVersion;

        unsigned short m_nextttisLibraryVersion;

        /**
         * The TTIS time schedule version
         */
        unsigned short m_ttisScheduleVersion;
        unsigned short m_nextttisScheduleVersion;

        /**
         * The last update time
         */
        time_t m_timestamp;

    };

}
#endif // !defined(PersistedTrainState_H)
