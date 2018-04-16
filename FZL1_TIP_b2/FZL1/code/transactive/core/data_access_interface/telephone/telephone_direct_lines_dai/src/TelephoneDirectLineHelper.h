/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLineHelper.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  *
  * TelephoneDirectLineHelper is an object that is held by TelephoneDirectLine and ConfigTelephoneDirectLine objects. 
  * It contains all data used by TelephoneDirectLines, and manipulation
  * methods for the data. It helps avoid re-writing code for both TelephoneDirectLine and ConfigTelephoneDirectLine.
  */

#if !defined(TelephoneDirectLineHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define TelephoneDirectLineHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include <string>
#include <vector>
#include <list>

#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/GenericDaiHelper.h"

class TA_Base_Core::IData;

namespace TA_IRS_Core
{
    class TelephoneDirectLineHelper: public GenericDaiHelper<TelephoneDirectLineHelper>
    {
    public:
        /** 
         * TelephoneDirectLineHelper
         *
         * This constructor creates a new TelephoneDirectLineHelper for the primary key.
         * Sets m_isDialBtnNew = false
         * Sets m_isDialBtnValidData = false
         * Sets m_isDialSetNew = false
         * Sets m_isDialSetValidData = false
         * Sets m_isOperatorKeySet = false
         * Sets m_isProfIdSet = false
         *
         * @param  idKey (primary key)
         *
         */
        TelephoneDirectLineHelper(const unsigned long btnKey);

        /** 
         * TelephoneDirectLineHelper
         *
         * Sets m_isDialBtnNew = true
         * Sets m_isDialBtnValidData = false
         * Sets m_isDialSetNew = false
         * Sets m_isDialSetValidData = false
         * Sets m_isOperatorKeySet = true
         * Sets m_isProfIdSet = true
         *
         * Copy constructor.
         *
         * @param   theTelephoneDirectLineHelper the original TelephoneDirectLineHelper to copy.
         *       
         */
        TelephoneDirectLineHelper( const TelephoneDirectLineHelper& theTelephoneDirectLineHelper);

	    /**
	     * TelephoneDirectLineHelper
	     * 
	     * Constructing a helper class based on row and data information
         *
         * Sets m_isDialBtnNew = false
         * Sets m_isDialBtnValidData = true
         * Sets m_isDialSetNew = false
         * Sets m_isDialSetValidData = true
         * Sets m_isOperatorKeySet = true
         * Sets m_isProfIdSet = true
         *
	     * 
	     * @return 
	     * @param : unsigned long row
	     * @param : TA_Base_Core::IData& data
	     * 
	     * @exception <exceptions> Optional
	     */
        TelephoneDirectLineHelper(unsigned long row, TA_Base_Core::IData& data);

	    /**
	     * TelephoneDirectLineHelper
	     * 
	     * Constructing a helper class based on TE_SPEED_DIAL_SET information
	     * 
         * Sets m_isDialBtnNew = true
         * Sets m_isDialBtnValidData = false
         * Sets m_isDialSetNew = false
         * Sets m_isDialSetValidData = false
         * Sets m_isOperatorKeySet = true
         * Sets m_isProfIdSet = true
         *
	     * @return 
	     * @param : unsigned long setPrimaryKey
	     * @param : long operaterKey
	     * @param : long profId
	     * @param : const std::string& setName
	     * 
	     * @exception <exceptions> Optional
	     */
        TelephoneDirectLineHelper(unsigned long setPrimaryKey, long operaterKey, long profId, const std::string& setName);


        /** 
         * TelephoneDirectLineHelper
         *
         * This constructor creates a new TelephoneDirectLineHelper for configuration, initiailising
         * the primary key to 0.  This primary key will be automatically set to a valid value 
         * when writeTelephoneDirectLineData() is called for the first time.
         *
         * Sets m_isDialBtnNew = true
         * Sets m_isDialBtnValidData = false
         * Sets m_isDialSetNew = true
         * Sets m_isDialSetValidData = false
         * Sets m_isOperatorKeySet = false
         * Sets m_isProfIdSet = false
         *
         */
        TelephoneDirectLineHelper();

        /** 
         * ~TelephoneDirectLineHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TelephoneDirectLineHelper();    
    
        /**
         * getBasicQueryData
         *
         * Provides default inputs for a query statement for this object
         * Function only reload the TE_SPEED_DIAL_BUTTON information
         *
         * Query will query the following information
         *
         * @return a query structure that can be used to retrieve all required
         *          elements from the table
         *         
         */
        static const DatabaseQueryHelper::QueryData getBasicQueryData();

        /**
         * reloadUsing
         *
         * Fills out the local members with data contained in the input data object
         *
         * @param row the row to query in the data object
         *
         * @param data the IData interface that should have been obtained using the 
         *              getBasicQueryData function
         *         
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the record cannot be found for the helper 
         *           constructed with a key.
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);

        /**
         * getKeyDirect
         *
         * @return the m_key member directly, this is for internal
         *      use only
         *
         * @exception ValueNotSetException if key unavailable
         *
         */
        virtual unsigned long getKeyDirect() const;


        /**
         * invalidate
         *
         * Make the data contained by this TelephoneDirectLineHelper as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeBtnData() and writeSetData() have been called prior to calling
         * this method on a new subdirectory, as it does not make any sense to invalidate a
         * record that has not yet been written to the database.
         *
         * pre:    This subdirectory was initially loaded from the database
         *         or writeBtnData() and writeSetData() has already been called
         */
        virtual void invalidate();

        /**
	     * writeBtnData
         * 
	     * Will call the corresponding add addNewBtnRecord() if set information is new,
         * otherwise, it will call modifyExistingBtnRecord() instead 
         *
         * @throws  TelephoneException (Database)
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TESDBU_ID should be an unsigned long)
         *          - NO_VALUE if the Button record cannot be found for the helper 
         *            constructed with an button record
         * @throws  DataConfigurationException
         *          - if the data contained in the TelephoneDirectLineHelper object is not sufficent 
         *            to create an entry in the database. 
	     */
        virtual void writeBtnData();

	    /**
	     * writeSetData
	     * 
	     * Will call the corresponding add addNewSetRecord() if set information is new,
         * otherwise, it will call modifyExistingSetRecord() instead 
	     * 
	     * @return virtual void 
	     * 
         * @throws  TelephoneException (Database)
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TESDBU_ID should be an unsigned long)
         * @throws  DataConfigurationException
         *          - if the data contained in the TelephoneDirectLineHelper object is not sufficent 
         *            to create an entry in the database. 
	     */
        virtual void writeSetData();


	    /**
	     * deleteTeSpeedDialSetRecord
	     * 
	     * Delete the record as specified in the object.  If button set is a new record, deletion
         * will not be carried out. Once this method has been executed, the
         * TelephoneDirectLineHelper object should be destroyed.
	     * 
	     * @return void 
	     * 
         * @param  bool cascade 
         *         set to true to force the removal of this record by removing all 
         *         references to this record in other tables (TE_SPEED_DIAL_BUTTON).  This should 
         *         be set to true to prevent the DatabaseException from being thrown. 
         *
         * @throws TelephoneException(DatabaseException)
         *         - if there is a problem writing the data to the database.
         * @throws DataException 
         *         - CANNOT_DELETE if references to this record exists in 
         *           TE_DIRECTORY table.
         *         - Error executing the SQL query
	     */
        void deleteTeSpeedDialSetRecord(bool cascade = false);

	    /**
	     * deleteTeSpeedDialButtonRecord
	     * 
	     * Delete the record as specified in the object.  If button is a new record, deletion
         * will not be carried out. Once this method has been executed, the
         * TelephoneDirectLineHelper object should be destroyed.
	     * 
	     * @return void 
	     * 
         * @throws TelephoneException(DatabaseException)
         *         - if there is a problem writing the data to the database.
         * @throws DataException 
         *         - CANNOT_DELETE if references to this record exists in 
         *           TE_DIRECTORY table.
         *         - Error executing the SQL query
         */
        void deleteTeSpeedDialButtonRecord();

	    /**
	      * getDialSetPrimaryKey
	      * 
	      * Return TE_SPEED_DIAL_SET primary key
	      * 
	      * @return unsigned long 
	      * 
	      * @exception <exceptions> Optional
	      */
        unsigned long getDialSetPrimaryKey();

	    /**
	      * setDialSetPrimaryKey
	      * 
	      * Set TE_SPEED_DIAL_SET primary key
	      * 
	      * @param : unsigned long key
	      * 
	      * @exception <exceptions> Optional
	      */
        void setDialSetPrimaryKey(unsigned long key);
   

	    /**
	      * getDialSetOperatorKey
	      * 
	      * Return TE_SPEED_DIAL_SET operator key
	      * 
	      * @return long 
	      * 
	      * @exception <exceptions> Optional
	      */
        long getDialSetOperatorKey();

	    /**
	      * setDialSetOperatorKey
	      * 
	      * Set TE_SPEED_DIAL_SET operator key
	      * 
	      * @param : long key
	      * 
	      * @exception <exceptions> Optional
	      */
        void setDialSetOperatorKey(long key);

	    /**
	      * getDialSetSeProfId
	      * 
	      * Return TE_SPEED_DIAL_SET profile ID
	      * 
	      * @return long 
	      * 
	      * @exception <exceptions> Optional
	      */
        long getDialSetSeProfId();

	    /**
	      * setDialSetSeProfId
	      * 
	      * Set TE_SPEED_DIAL_SET profile ID
	      * 
	      * @param : long id
	      * 
	      * @exception <exceptions> Optional
	      */
        void setDialSetSeProfId(long id);


	    /**
	      * getDialSetSetName
	      * 
	      * Return TE_SPEED_DIAL_SET set name
	      * 
	      * @return std::string  
	      * 
	      * @exception <exceptions> Optional
	      */
  	    std::string getDialSetSetName();

	    /**
	      * setDialSetSetName
	      * 
	      * Set TE_SPEED_DIAL_SET set name
	      * 
	      * @param : const std::string& name
	      * 
	      * @exception <exceptions> Optional
	      */
        void setDialSetSetName(const std::string& name);


	    /**
	      * getIsDialSetValid
	      * 
	      * Indicates if the TE_SPEED_DIAL_SET record is valid
	      * 
	      * @return bool 
	      * 
	      * @exception <exceptions> Optional
	      */
        bool getIsDialSetValid();

	    /**
	      * setIsDialSetValid
	      * 
	      * Sets the validity of the TE_SPEED_DIAL_SET record
	      * 
	      * @param : bool state
	      * 
	      * @exception <exceptions> Optional
	      */
        void setIsDialSetValid(bool state);


	    /**
	      * getIsDialSetNew
	      * 
	      * Indicates if the TE_SPEED_DIAL_SET record is a new/existing record
	      * 
	      * @return bool 
	      * 
	      * @exception <exceptions> Optional
	      */
        bool getIsDialSetNew();

	    /**
	      * setIsBialSetNew
	      * 
	      * Set the state of TE_SPEED_DIAL_SET record
	      * 
	      * @param : bool state
	      * 
	      * @exception <exceptions> Optional
	      */
        void setIsBialSetNew(bool state);

	    /**
	      * getDialBtnPrimaryKey
	      * 
	      * Return TE_SPEED_DIAL_BUTTON primary key
	      * 
	      * @return unsigned long 
	      * 
	      * @exception <exceptions> Optional
	      */
        unsigned long getDialBtnPrimaryKey();

	    /**
	      * setDialBtnPrimaryKey
	      * 
	      * Set TE_SPEED_DIAL_BUTTON primary key
	      * 
	      * @param : unsigned long key
	      * 
	      * @exception <exceptions> Optional
	      */
        void setDialBtnPrimaryKey(unsigned long key);


	    /**
	      * getDialBtnDialSetId
	      * 
	      * Return TE_SPEED_DIAL_SET primary key associated with the button
	      * 
	      * @return unsigned long 
	      * 
	      * @exception <exceptions> Optional
	      */
        unsigned long getDialBtnDialSetId();

	    /**
	      * setDialBtnDialSetId
	      * 
	      * Set TE_SPEED_DIAL_SET primary key associated with the button
	      * 
	      * @param : unsigned long Id
	      * 
	      * @exception <exceptions> Optional
	      */
        void setDialBtnDialSetId(unsigned long Id);


	    /**
	      * getDialBtnPos
	      * 
	      * Return the TE_SPEED_DIAL_BUTTON button position
	      * 
	      * @return int 
	      * 
	      * @exception <exceptions> Optional
	      */
        int getDialBtnPos();

	    /**
	      * setDialBtnPos
	      * 
	      * Set the TE_SPEED_DIAL_BUTTON button position
	      * 
	      * @param : int btnPos
	      * 
	      * @exception <exceptions> Optional
	      */
        void setDialBtnPos(int btnPos);

	    /**
	      * getDialBtnLabel
	      * 
	      * Return the TE_SPEED_DIAL_BUTTON label
	      * 
	      * @return std::string  
	      * 
	      * @exception <exceptions> Optional
	      */
        std::string   getDialBtnLabel();

	    /**
	      * setDialBtnLabel
	      * 
	      * Set the TE_SPEED_DIAL_BUTTON label
	      * 
	      * @param : const std::string& label
	      * 
	      * @exception <exceptions> Optional
	      */
        void setDialBtnLabel(const std::string& label);

	    /**
	      * getDialBtnTsi
	      * 
	      * Return the TE_SPEED_DIAL_BUTTON telephone number
	      * 
	      * @return std::string  
	      * 
	      * @exception <exceptions> Optional
	      */
        std::string   getDialBtnTsi();

	    /**
	      * setDialBtnTsi
	      * 
	      * Set the TE_SPEED_DIAL_BUTTON telephone number
	      * 
	      * @param : const std::string& tsi
	      * 
	      * @exception <exceptions> Optional
	      */
        void setDialBtnTsi(const std::string& tsi);


	    /**
	      * getIsDialBtnValidData
	      * 
	      * Return the TE_SPEED_DIAL_BUTTON telephone number
	      * 
	      * @return bool 
	      * 
	      * @exception <exceptions> Optional
	      */
        bool getIsDialBtnValidData();

	    /**
	      * setIsDialBtnValidData
	      * 
	      * Set the TE_SPEED_DIAL_BUTTON telephone number
	      * 
	      * @param : bool state
	      * 
	      * @exception <exceptions> Optional
	      */
        void setIsDialBtnValidData(bool state);

	    /**
	      * getIsDialBtnNew
	      * 
	      * Indicates if the button record is new/existing record
	      * 
	      * @return bool 
	      * 
	      * @exception <exceptions> Optional
	      */
        bool getIsDialBtnNew();

	    /**
	      * setIsDialBtnNew
	      * 
	      * Set the state of the button record 
          * True = new record
          * False = existing record
	      * 
	      * @param : bool state
	      * 
	      * @exception <exceptions> Optional
	      */
        void setIsDialBtnNew(bool state);

        // ---------------------------------
        // Constant variables that is used
        // ---------------------------------

        static const std::string TE_SPEED_DIAL_SET_SEQ_NUM;
        static const std::string TE_SPEED_DIAL_SET_TABLE_NAME;
        static const std::string TE_SPEED_DIAL_SET_PRI_KEY;
        static const std::string TE_SPEED_DIAL_SET_OPERATOR_KEY;
        static const std::string TE_SPEED_DIAL_SET_SEPROF_ID;
        static const std::string TE_SPEED_DIAL_SET_SET_NAME;
        static const std::string TE_SPEED_DIAL_SET_CREATED_BY;
        static const std::string TE_SPEED_DIAL_SET_DATE_CREATED;
        static const std::string TE_SPEED_DIAL_SET_MODIFIED_BY;
        static const std::string TE_SPEED_DIAL_SET_DATE_MODIFIED;

        static const std::string TE_SPEED_DIAL_BTN_SEQ_NUM;
        static const std::string TE_SPEED_DIAL_BTN_TABLE_NAME;
        static const std::string TE_SPEED_DIAL_BTN_PRI_KEY;
        static const std::string TE_SPEED_DIAL_BTN_TESDSE_ID;
        static const std::string TE_SPEED_DIAL_BTN_BUTTON_POSITION;
        static const std::string TE_SPEED_DIAL_BTN_BUTTON_LABEL;
        static const std::string TE_SPEED_DIAL_BTN_BUTTON_TSI;
        static const std::string TE_SPEED_DIAL_BTN_CREATED_BY;
        static const std::string TE_SPEED_DIAL_BTN_DATE_CREATED;
        static const std::string TE_SPEED_DIAL_BTN_MODIFIED_BY;
        static const std::string TE_SPEED_DIAL_BTN_DATE_MODIFIED;


    private:
        /**
	     * addNewBtnRecord
	     * 
	     * Function will get an appropriate primary key and assign to m_dialBtnPrimaryKey
         * Using this key, this new record will be added to the database
         * It will call validateSpeedDialSetExistance() to validate that the set info
         * specified in the button record exist
	     * 
	     * @return void 
	     * 
         * @throws  TelephoneException (Database)
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TESDBU_ID should be an unsigned long)
         *          - NO_VALUE if the Button record cannot be found for the helper 
         *            constructed with an button record
	     */
        void addNewBtnRecord();


	    /**
	     * modifyExistingBtnRecord
	     * 
         * This will update an existing button record in the database with the internals  
         * currently set in this instance.  It will call validateSpeedDialSetExistance() 
         * to validate that the set info specified in the button record exist
	     * 
	     * @return void 
         *
         * @throws  TelephoneException (Database)
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TESDBU_ID should be an unsigned long)
         *          - NO_VALUE if the Button record cannot be found for the helper 
         *            constructed with an button record
	     */
        void modifyExistingBtnRecord();


	    /**
	     * addNewSetRecord
	     * 
	     * Function will get an appropriate primary key and assign to m_dialSetPrimaryKey
         * Using this key, this new record will be added to the database
	     * 
	     * @return void 
	     * 
         * @throws  TelephoneException (Database)
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TESDBU_ID should be an unsigned long)
         *          - NO_VALUE if the Button record cannot be found for the helper 
         *            constructed with an button record
	     */
        void addNewSetRecord();


	    /**
	     * modifyExistingSetRecord
	     * 
         * This will update an existing button set record in the database with the internals  
         * currently set in this instance.  It will call validateSpeedDialSetExistance() 
         * to validate that the set info specified in the button record exist
	     * 
	     * @return void 
         *
         * @throws  TelephoneException (Database)
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TESDBU_ID should be an unsigned long)
         *          - NO_VALUE if the Button record cannot be found for the helper 
         *            constructed with an button record
	      */
        void modifyExistingSetRecord();


	    /**
	     * validateSpeedDialSetExistance
	     * 
         * Validate that the set info specified in this helper object exist in the database
         *
	     * @return void 
	     * 
         * @throws  DataException
         *          - NO_VALUE if the button set record cannot be found
	     */
        void validateSpeedDialSetExistance();


	    /**
	      * initialize
	      * 
	      * Initialize all member variables to a default values
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        void initialize();


        /**
	      * Stores the TE_SPEED_DIAL_SET primary key
          */
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_dialSetPrimaryKey;

        /**
	      * Stores the TE_SPEED_DIAL_SET operator key
          */
        long m_dialSetOperatorKey;

        /**
	      * Stores the TE_SPEED_DIAL_SET profile ID
          */
        long m_dialSetSeProfId;

        /**
	      * Stores the TE_SPEED_DIAL_SET Set Name
          */
  	    std::string m_dialSetSetName;

        /**
	      * Indicates whether the TE_SPEED_DIAL_SET information is valid
          */
        bool m_isDialSetValidData;

        /**
	      * Indicates whether the TE_SPEED_DIAL_SET record is a new/existing record
          */
        bool m_isDialSetNew;

        /**
	      * Indicates whether the TE_SPEED_DIAL_SET operator key has been set before
          */
        bool m_isOperatorKeySet;

        /**
	      * Indicates whether the TE_SPEED_DIAL_SET profile ID has been set before
          */
        bool m_isProfIdSet;


        /**
	      * Stores TE_SPEED_DIAL_BUTTON primary key
          */
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_dialBtnPrimaryKey;

        /**
	      * Stores TE_SPEED_DIAL_BUTTON button position
          */
        int m_dialBtnPos;

        /**
	      * Stores TE_SPEED_DIAL_BUTTON label
          */
        std::string m_dialBtnLabel;

        /**
	      * Stores TE_SPEED_DIAL_BUTTON telephone number
          */
        std::string m_dialBtnTsi;

        /**
	      * Indicates whether the TE_SPEED_DIAL_BUTTON is valid
          */
        bool m_isDialBtnValidData;

        /**
	      * Indicates whether the TE_SPEED_DIAL_BUTTON record is a new/existing record
          */
        bool m_isDialBtnNew;
    };
} // close TA_IRS_Core

#endif // !defined(TelephoneDirectLineHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
