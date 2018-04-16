/**
  *
  *
  * This is the access factory used to load/create train data.
  */

#if !defined(TrainAccessFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_)
#define TrainAccessFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

// forward declarations
namespace TA_IRS_Core
{
    class ITrain;    
}


namespace TA_IRS_Core
{
    class TrainAccessFactory
    {

    public:


	    /**
	      * ~TrainAccessFactory
	      * 
	      * Destructor
	      */
	    virtual ~TrainAccessFactory();

	    /**
	      * getInstance
	      * 
	      * Creates and returns an instance of the singleton access factory.
	      * 
	      * @return TrainAccessFactory& 
	      */
	    static TrainAccessFactory& getInstance();

	    /**
	      * removeInstance
	      * 
	      * Removes the instance of the class (if already created) and cleans up the members.  
          * Primarily used upon program termination (e.g. from main()) so that Purify does not 
          * consider this class and its members to be memory leaks.
	      * 
	      * @return void 
	      */
        static void removeInstance();

        /**
          * getAllTrains
          *
          * Retrieves all Trains and returns the associated data as a vector of  
          * objects conforming to the Train interface.
          *
          * @return  std::vector<ITrain*> 
          *          A vector of pointers to objects conforming to the ITrain interface.
          *          Note: It is the responsibility of the caller (client) to delete all
          *          the returned ITrain objects.
          * @param   const bool readWrite (optional)
          *             set to true to return write-able ConfigTrain objects or 
          *             set to false to return read-only Train objects.          
          *
          * @exception DatabaseException 
          *            - if there are any problems in communicating with the database.
          *            The possible reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          *             - if there is no Train matching the supplied key
          *             - if there is more than one Train matching the supplied key.          
          */
        std::vector<ITrain*> getAllTrains(const bool readWrite = false);
		//std::map<unsigned long, std::string>  getAllLines ();
        
        /**
          * getTrain
          *
          * Retrieves the specified Train and returns the associated data as an 
          * object conforming to the ITrain interface.
          *
          * @return  A pointer to an object conforming to the ITrain interface.
          *          Note: It is the responsibility of the caller/*client* to delete
          *          the returned ITrain object.
          * @param   key        the primary key of the Train to retrieve
          * @param   readWrite  (optional) set to true to return writable
          *                     IConfigTrain objects or set to false to return 
          *                     read only ITrain objects.
          *
          * @exception  DatabaseException 
          *              - if there are any problems in communicating with the database.
          *              The possible reasons are:
          *              1) Invalid connection string (as stored in RunParams)
          *              2) Database is not/cannot be opened
          *              3) Error while attempting to execute a database query
          * @exception   DataException 
          *              - if there is no Train matching the supplied key
          *              - if there is more than one Train matching the supplied key. 
          */
// 		ITrain* getTrain(const unsigned long key, const bool readWrite = false);

        /**
          * createTrain
          * 
          * Creates an empty ConfigTrain object, and returns a pointer to it.
          * It is the responsibility of the caller (client) to delete the returned  
          * ConfigTrain object.
          *
          * @return IConfigTrain* - A pointer to an IConfigTrain object
          */
	    ITrain* createTrain();

		 /**
         * copyMessage
         * 
         * Creates a new configuration Train object using the trainToCopy passed in
         *
         * @param  trainToCopy  the Train to copy
         *
         * @return the new object as a pointer to an ITrain object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        ITrain* copyTrain(const ITrain* trainToCopy);

		bool isTrainIDExist(unsigned long trainID);
        
    private:

	    /**
	      * TrainAccessFactory
	      * 
	      * Default constructor (privatized)
	      */
        TrainAccessFactory();

	    /**
	      * TrainAccessFactory
	      * 
	      * Copy constructor (disabled)
	      * 
	      * @param : const TrainAccessFactory& theTrainAccessFactory
	      */
		TrainAccessFactory(const TrainAccessFactory&);

	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
	      * 
	      * @return TrainAccessFactory& 
	      * @param : const TrainAccessFactory&
	      */
		TrainAccessFactory& operator=(const TrainAccessFactory&);

    private:
        
	    static TrainAccessFactory* m_instance;


		static const std::string CO_LINE_TBL;
		static const std::string COLINE_ID;
		static const std::string NAME;

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

    };
}
#endif // !defined(TrainAccessFactory_A9104F9A_471C_4e66_AD1C_B1660712FC6F__INCLUDED_)