/**
  *
  *
  * TrainHelper is an object that is held by Train objects.
  * It contains all data used by Trains, and manipulation methods for the data. 
  * It helps avoid re-writing code for Train.
  */

#if !defined(TRAINHELPER_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
#define TRAINHELPER_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include <string>
#include <vector>

// forward declarations
namespace TA_Base_Core
{
    class IData;
}


namespace TA_IRS_Core
{
    class TrainHelper  
    {
    public:

        //
        // Constructors and destructor
        //

        /** 
          * TrainHelper
          *
          * The default constructor creates a new TrainHelper for configuration, 
          * initializing the key to 0 - it will be specified once this train is 
          * written to the database for the first time.
          */
        TrainHelper();

	    /**
	      * TrainHelper
	      * 
	      * Copy constructor
	      * 
	      * @param : const TrainHelper& theTrainHelper
	      */
        TrainHelper(const TrainHelper& theTrainHelper);

	    /**
	      * TrainHelper
	      * 
	      * This constructor creates a new Train object for the specified key.
	      * 
	      * @param : const unsigned long key
	      */
        TrainHelper(const unsigned long key);

	    /**
	      * TrainHelper
	      * 
	      * This constructor creates a new object using the input data
          * which is representative of a row returned from SQL statement
	      * 
	      * @param : const unsigned long row
          *          - the row of data in the data object that we should collect data from
          *          in order to construct ourselves
	      * @param : TA_Base_Core::IData& data
          *          - the IData interface that should have been obtained using the 
          *          getBasicQueryData function
	      * 
	      * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
	      */
        TrainHelper(const unsigned long row, TA_Base_Core::IData& data);
       
        /** 
          * ~TrainHelper
          *
          * Destructor for basic class cleanup.
          */
        virtual ~TrainHelper();

        //
        // TrainHelper class methods
        //

        /**
          * writeTrainData
          * 
          * Write this train to the database. If this is a new train, this will populate
          * the m_key field (which can be retrieved using getKey()).
          *
          * @exception DatabaseException 
          *             A DatabaseException will be thrown if there is a problem writing the 
          *             data to the database.
          * @exception DataConfigurationException 
          *             If the data contained in the Train object is not sufficient to create 
          *             an entry in the database, a DataConfigurationException will be thrown.
          *             This is thrown as an exception and not tested as an assert as it can 
          *             be directly linked to GUI components and may be the result of a user 
          *             action that occurs prior to filling in all sections.
          */
        void writeTrainData();

        /**
          * deleteTrain
          *
          * Remove this train from the database. Once this method has been executed, the
          * TrainHelper object should be destroyed.
          *
          * @return void 
          *
          * @exception DatabaseException: If there was an error connecting to the database.
          * @exception DataException: If there was an error deleting.
          *
          * pre: Either - this train was initially loaded from the database
          *      OR     - writeTrainData() has already been called
          */
        void deleteTrain();

        //
        // ITrain interface methods
        //

	    /**
	      * getTrainId
	      * 
	      * Gets the ID of the train.
	      * 
	      * @return unsigned long
	      */
	    unsigned long getTrainId();

	    /**
	      * setTrainId
	      * 
	      * Sets the ID of the train. 
	      * 
	      * @return void 
	      * @param : unsigned unsigned long
	      */
	    void setTrainId( unsigned long);
		/**
	      * getPrimaryTsi
	      * 
	      * Gets the ID of the primary tsi.
	      * 
	      * @return unsigned long
	      */

		std::string getPrimaryTsi() ;
		/**
	      * getSecondaryTsi
	      * 
	      * Gets the ID of the second tsi.
	      * 
	      * @return unsigned long
	      */
		
		std::string getSecondaryTsi();
	
		/**
	      * setPrimaryTsi
	      * 
	      * Sets the primary tsi of the train.
		  * @parameter unsigned long	
	      * 
	      * @return 
	      */		
		void setPrimaryTsi(const std::string&);
		/**
	      * setSecondaryTsi
	      * 
	      * Sets the second tsi of the train.
	      * 
	      * @return unsigned long
	      */
		
		void setSecondaryTsi(const std::string&);
	
		/**
	      * getName
	      * 
	      * return the train id as a string.
	      * 
	      * @return std::string
	      */
		
		std::string getName() ;
		
		/**
	      * setName
	      * 
	      * sets nothing
	      * 
	      * @return unsigned long
	      */
		
		void setName(std::string&);
	
        //
        // IItem interface methods
        //

	    /**
	      * getKey
	      * 
	      * Returns the key for this item.
	      * 
	      * @return unsigned long
	      */
        //unsigned long getKey();

        /**
          * invalidate
          *
          * Make the data contained by this train as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database. This includes associated profiles and regions if required.
          * It is a precondition that writeTrainData() has been called prior to calling
          * this method on a new train, as it does not make any sense to invalidate a
          * train that has not yet been written to the database.
          *
          * pre: Either - this train was initially loaded from the database
          *      OR     - writeTrainData() has already been called
          */
        void invalidate();

        //
        // IConfigItem interface methods
        //

        /**
          * isNew
          *
          * This returns whether this is a new Train.  A new Train is created by the default
          * constructor.  Once written to the database, it is no longer considered new.
          *
          * @return true if this is a new Train
          */
        bool isNew() const
        {
            return m_isNew;
        };

		/**
         * getDateCreated
         *
         * Returns the date created for this Train.  If this Train record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing Train has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PaStation as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PaStation record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();

		 /**
         * getDateModified
         *
         * Returns the date modified for this Train.  If this PaStation record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaStation has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this Train as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PaStation record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();


    private:

	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
          *
          * @return TrainHelper& 
	      * @param : const TrainHelper&
	      */
		TrainHelper& operator=(const TrainHelper&);

        /**
          * reload
          *
          * This method reloads the data from the database. It is called when a get... method
          * is called and the data state is not valid.
          *
          * pre: Either - this train was initially loaded from the database
          *      OR     - writeTrainData() has already been called
          *
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the Train Id record cannot be found for the helper 
          *             constructed with an Train Id.
          *             - NOT_UNIQUE if the Train Id returns multiple records 
          */
        void reload();

        /**
          * reloadUsing
          *
          * Fills out the local members with data contained in the input data object
          *
          * @param : const unsigned long row - the row to query in the data object
          * @param : TA_Base_Core::IData& data - the IData interface that should 
          *          have been obtained using the getBasicQueryData function
          *         
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
          */
        void reloadUsing(const unsigned long row, TA_Base_Core::IData& data);

        /**
          * modifyExistingTrain
          *
          * This will update an existing Train in the database with the internals currently 
          * set in this instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if there is a problem writing the data to the database.  
          * @exception  DataConfigurationException 
          *             if the data contained in the Train object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the train id references a non-existent SI_TRAIN record. 
          */
        void modifyExistingTrain();
        
        /**
          * addNewTrain
          *
          * This will add a new Train in the database with the internals currently set in this 
          * instance.
          *
          * @return void 
          *
          * @exception  DatabaseException 
          *             if we are unable to get a primary key for the new record.
          *             if there is a problem writing the data to the database. 
          * @exception  DataConfigurationException 
          *             if the data contained in the Train object is not sufficient to create an
          *             entry in the database.  This is thrown as an exception and not tested as 
          *             an assert as it can be directly linked to GUI components and may be the 
          *             result of a user action that occurs prior to filling in all sections.
          * @exception  DataException 
          *             if there is a problem writing the data to the database.
          *             if the train id references a non-existent SI_TRAIN record. 
          *             if we are unable to use the primary key obtained from database.
          */
        void addNewTrain();

	    /**
	      * validateCarRadioUniqueness
	      * 
	      * This checks if both emuid1 and emuid2 numbers are already in use in the database.
	      *
		  * @isnew  if this is a new record, insert opearation.
		  *
	      * @return void 
	      * 
	      * @exception TA_Base_Core::TransactiveException 
          *            If train car 1 and 2 radio number or train car 2 already exist.
	      */
        //void validateEmuidUniqueness(bool isNew);


	    /**
	      * validateTrainIdUniqueness
	      * 
	      * This checks the train id against the db.
		  * @isnew  if this is a new record, insert opearation.
	      * 
	      * @return void 
	      * 
	      * @exception TA_Base_Core::TransactiveException 
          *            If train id already exists.
	      */
        void validateTrainIdUniqueness(bool isNew);

	    /**
	      * getNextSequenceNumber
	      * 
	      * Returns the next number in the primary key sequence (from database).
	      * 
	      * @return unsigned long 
	      * 
	      * @exception  DatabaseException
          *             - The database has is closed or unable to obtain the next sequence
          *             number from the database.
          *             DataException
          *             - If the next sequence number cannot be converted to an unsigned 
		  *             long after obtaining it from the database.
	      */
       // unsigned long getNextSequenceNumber();

    private:	    

        bool            m_isNew;
        bool            m_isValidData;	
        
		static std::string getLocalDatabaseName();

		/**
         * lock for initialisation of static column names vector
         */
        static TA_Base_Core::NonReEntrantThreadLockable s_columnNamesLock;

		/**
         * The vector of column names used to load data from the database
         */
        static std::vector<std::string> s_columnNames;

		/**
         * This gets the column names to be used when decoding the data
         */
        static const std::vector<std::string>& getColumnNames();

        /**
	      * SI_TRAIN fields
          */
 
        //unsigned long   m_key;      
        unsigned long  m_trainId;

		std::string     m_primaryTsi;
		std::string     m_secondaryTsi;

		time_t m_dateCreated;
		time_t m_dateModified;

    };
}
#endif // !defined(TRAINHELPER_H__0348DE8C_BF93_4F98_A510_A5CDA6FCFBDD__INCLUDED_)
