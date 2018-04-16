#if !defined(TRAIN_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
#define TRAIN_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/trains/src/Attic/Train.h,v $
  * @author:  Adam Radics
  * @version: $Revision: 1.1.2.2 $
  *
  * Last modification: $Date: 2006/08/16 03:26:19 $
  * Last modified by:  $Author: adamr $
  *
  * Class Train is an implementation of the interface ITrain.
  * It holds the data specific to a Train entry in the database, 
  * and allows read/write access to that data.
  */

#include "core/data_access_interface/TrainStateDai/src/ITrain.h"
#include "core/data_access_interface/TrainStateDai/src/TrainHelper.h"


namespace TA_IRS_Core
{
    class Train : public ITrain  
    {
    public:
		 /**
	      * Train
	      * 
	      * Default constructor.
	      * Constructs a new instance of the ConfigTrain with no key. This is used when
          * creating a *new* Train - that is, one that does not yet exist in the database.
	      */
		Train();
	    /**
	      * Train
	      * 
	      * Constructs a train object based on the primary key
	      * 
	      * @param : const unsigned long key
	      */
	    Train(const unsigned long key);
        
	    /**
	      * Train
	      * 
	      * Constructs a new Train object using the information provided in IData interface
	      * 
	      * @param : const unsigned long row
	      * @param : TA_Base_Core::IData& data
	      * 
	      * @see TrainHelper::TrainHelper(const unsigned long, TA_Base_Core::IData&);
	      */
        Train(const unsigned long row, TA_Base_Core::IData& data);

		 /**
	      * Train
	      * 
	      * Copy constructor (disabled).
          * If a copy constructor is required care should be taken. The copy constructor to 
          * the TrainHelper has only been written for ConfigTrain objects and will not copy 
          * the Train helper for a read-only object (ie this one).
	      * 
	      * @param : const Train&
	      */
        Train(const Train&);

	    /**
	      * ~Train
	      * 
	      * Destructor 
	      */
	    virtual ~Train();

        //
        // ITrain methods
        //

	    /**
	      * getTrainId
	      * 
	      * Gets the ID of the train.
	      * 
	      * @return unsigned char
	      */
	    virtual unsigned long getTrainId();

	    /**
		* gets the 2 emu ids that make up this train 
		* 
		* 
		* 
		* @return unsigned long
		*/
		virtual std::string getPrimaryTsi();
		/**
		* 
		* 
		* gets the 2 emu ids that make up
		* 
		* @return unsigned 
		*/
		virtual std::string getSecondaryTsi();
		/**
		* sets the PrimaryEmuId
		* 
		* @return unsigned 
		*/
		virtual void setTrainId(unsigned long) ;
		/**
		* sets the train ID 
		* 
		* @return  
		*/
		virtual void setPrimaryTsi(const std::string&) ;
		/**
		* sets the SecondEmuId
		* 
		* 
		* 
		* @return 
		*/
		virtual void setSecondaryTsi(const std::string&) ;

        //
        // IItem methods
        //

	    /**
	      * getKey
	      * 
	      * Returns the key for this item.
	      * 
	      * @return unsigned long
	      */
		virtual unsigned long getKey();

        /**
          * getName
          * 
          * Returns the name of this item. If this is the first time data for this item
          * has been requested, all the data is loaded from the database.
          * 
          * @return std::string
          * 
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format (e.g. ACCESSLEVEL should be an 
          *            unsigned long), or if the wrong amount of data is retrieved.
          */
        virtual std::string getName();
		 /**
          * setName
          *
          * Sets the name of this item locally (not implemented). 
          *
          * @return void 
          * @param std::string& name The name to give this item.
          *
          * @exception DatabaseException A DatabaseException will be thrown if there is a
          *            problem establishing a connection with the database, or if an
          *            error is encountered while retrieving data.
          * @exception DataException A DataException will be thrown if the data cannot be 
          *            converted to the required format (e.g. TYPEKEY should be an 
          *            unsigned long), or if the wrong amount of data is retrieved, or if the
          *            the item key is invalid (and this is not a new item).
          */
        virtual void setName(const std::string&);
		

	    /**
	      * invalidate
	      * 
	      * Make the data contained by this item as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
	      * 
	      * @return void 
	      */
        virtual void invalidate();

		/**
          * hasChanged
          *
          * This should be called to determine if any part of the item has been changed by the user.
          *
          * @return bool - This will return true if the item does not match the one in the database.
          *                It will return false if the data has not changed from that in the database.
          */
        virtual bool hasChanged();
	
		/**
          * applyChanges
          *
          * This will apply all changes made to the database.
          *
          * @return void 
          *
          * @exception DatabaseException Thrown if there is a database error.
          * @exception DataException Thrown if the a parameter name cannot be found, or if
          *            there is more than one value for a parameter.
          * @exception DataConfigurationException If the data contained in the Item object
          *            is not sufficient to create an entry in the database, a 
          *            DataConfigurationException will be thrown. This is thrown as an exception
          *            and not tested as an assert as it can be directly linked to GUI components
          *            and may be the result of a user action that occurs prior to filling in 
          *            all sections.
          *
          * pre: This item has not been deleted
          */
		void  applyChanges() ;  

		/**
          * isNew
          *
          * This should be called to determine if this item is new. A new item is one that has not
          * yet been applied to the database and been given a primary key and create date.
          *
          * @return bool - This will return true if the item has not been applied to the database.
          */
        virtual bool isNew();
		/**
          * getAllItemChanges
          *
          * This is called to retrieve all changes made to the item. If the map returned is empty then
          * no changes have been made. This must be called before the changes are applied or it will be
          * cleared.
          *
          * @return ItemChanges - The map containing all item changes
          */
        virtual TA_Base_Core::ItemChanges getAllItemChanges();

		/**
          * getUniqueIdentifier
          *
          * This retrieves the unique identifier for this item. We cannot use keys for identification
          * in the configuration editor because new items do not have keys. Therefore we use this
          * method for identification
          *
          * @return unsigned long - The unique identifier for this item
          */
        virtual unsigned long getUniqueIdentifier();
		/**
          * deleteThisObject
          *
          * Removes this Train from the database. 
          * The calling application MUST then delete this Train object, as it makes no sense
          * to keep it any longer.
          *
          * @exception DatabaseException: If there was an error connecting to the database.
          * @exception DataException: If there was an error deleting.
          *
          * pre: Either - this Train was initially loaded from the database
          *      OR     - writeTrainData() has already been called
          *      This Train has not been deleted
          */
        virtual void deleteThisObject();

		/**
         * getDateCreated
         *
         * Returns the date created for this Train
         *
         * @return The date created for this Train as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this Train.
         *
         * @return The date modified for this Train as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified();


    private:

	   

	    /**
	      * operator=
	      * 
	      * Assignment operator (disabled)
	      * 
	      * @return Train& 
	      * @param : const Train &
	      */
		Train& operator=(const Train&);

		  /**
	      * updateTrainChanges
	      * 
	      * This updates the map recording the changes to the Train. Whenever a call to set
          * is made in this class then this method must be called to store the changes.
	      * 
	      * @return void 
	      * @param : const std::string& name - The name of the attribute that has been given a new value
	      * @param : const std::string& oldValue - The old value that was stored for this attribute
	      * @param : const std::string& newValue - The new value to be stored for this attribute
	      */
		void updateTrainChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);

    private:

		std::auto_ptr<TrainHelper>  m_trainHelper;
        TA_Base_Core::ItemChanges   m_itemChanges;
		
        /**
		* We cannot use keys for identification of trains in the Configuration Editor 
		* because new trains do not have keys. Therefore we use this method for identification.
		*/
        unsigned long m_uniqueIdentifier; 
		
        /**
		* The next unique identifier available.
		*/
        static unsigned long s_nextAvailableIdentifier; 
    };
}
#endif // !defined(TRAIN_H__AF9B5FE3_E43B_474F_B6A5_D6F29EC22743__INCLUDED_)
