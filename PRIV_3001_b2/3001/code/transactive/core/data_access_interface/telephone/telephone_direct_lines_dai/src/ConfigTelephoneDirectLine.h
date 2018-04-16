/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/ConfigTelephoneDirectLine.h $
  * @author Raymond Pau
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * ConfigTelephoneDirectLine is a concrete implementation of IConfigTelephoneDirectLine, which is in turn an implementation
  * of ITelephoneDirectLine. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for TelephoneDirectLines.
  */

#if !defined(ConfigTelephoneDirectLine_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigTelephoneDirectLine_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>

#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/IConfigTelephoneDirectLine.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/src/IData.h"

namespace TA_IRS_Core
{
    class TelephoneDirectLineHelper;

    class ConfigTelephoneDirectLine : public IConfigTelephoneDirectLine
    {

    public:
        /**
         * ConfigTelephoneDirectLine (constructor)
         *
         * Constructs a new instance of the ConfigTelephoneDirectLine with no id. This is used when
         * creating a *new* TelephoneDirectLine - that is, one that does not yet exist in the database.
         */
        ConfigTelephoneDirectLine();

        /**
         * ConfigTelephoneDirectLine (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see TelephoneDirectLineHelper::TelephoneDirectLineHelper(IData*)
         *
         */
        ConfigTelephoneDirectLine(unsigned long row, TA_Base_Core::IData& data);
            
        /**
         * ConfigTelephoneDirectLine (constructor)
         *
         * Constructs a new instance of ConfigTelephoneDirectLine with the specified id. This is used 
         * when creating a ConfigTelephoneDirectLine based around an existing TelephoneDirectLine in the database.
         *
         * @param key The key to this TelephoneDirectLine in the database.
         */
        ConfigTelephoneDirectLine(const unsigned long idKey);

        /**
         * ConfigTelephoneDirectLine (copy constructor)
         *
         * @param ConfigTelephoneDirectLine& - The TelephoneDirectLine to make a copy of.
         */
        ConfigTelephoneDirectLine(const ConfigTelephoneDirectLine& theConfigTelephoneDirectLine);

        /**
         * ConfigTelephoneDirectLine (constructor)
         *
         * Constructs a new instance using the TE_SPEED_DIAL_SET information
         */
        ConfigTelephoneDirectLine(unsigned long setPrimaryKey, long operaterKey, long profId, const std::string& setName);
    
        virtual ~ConfigTelephoneDirectLine();

    public:
        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier();

        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * invalidate
         *
         * Make the data contained by this ConfigTelephoneDirectLine as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();
    
        /**
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setName(const std::string& name);

        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         * This will call writeSetData() and subsequently writeBtnData()
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges();


	    /**
	      * deleteTeSpeedDialSetRecord
	      * 
	      * Delete the TE_SPEED_DIAL_SET record
	      * 
	      * @return virtual void 
	      * @param : bool cascade = false
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void deleteTeSpeedDialSetRecord(bool cascade = false);

	    /**
	      * deleteTeSpeedDialButtonRecord
	      * 
	      * Delete the TE_SPEED_DIAL_BUTTON record
	      * 
	      * @return virtual void 
	      * @param : bool cascade = false
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void deleteTeSpeedDialButtonRecord(bool cascade = false);

	    /**
	      * writeBtnData
	      * 
	      * Write the TE_SPEED_DIAL_BUTTON data to the database
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
	      * Write the TE_SPEED_DIAL_SET data to the database
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
	      * getBtnSetKey 
	      * 
	      * Return the TE_SPEED_DIAL_SET primary key
	      * 
	      * @return virtual unsigned long 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getBtnSetKey();

	    /**
	      * setBtnSetKey
	      * 
	      * Sets the TE_SPEED_DIAL_SET primary key
	      * 
	      * @param : const unsigned long key
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setBtnSetKey(const unsigned long key);

	    /**
	      * getOperatorKey
	      * 
	      * Return the operator key associated with the button
	      * 
	      * @return virtual long 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual long getOperatorKey();

	    /**
	      * setOperatorKey
	      * 
	      * Set the operator key associated with the button
	      * 
	      * @param : const long key
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setOperatorKey(const long key);

	    /**
	      * getSeProfId
	      * 
	      * Return the profile Id associated with the button
	      * 
	      * @return virtual long 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual long getSeProfId();

	    /**
	      * setSeProfId
	      * 
	      * Set the profile Id associated with the button
	      * 
	      * @param : const long profId
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setSeProfId(const long profId);

	    /**
	      * getSetName
	      * 
	      * Return the setname associated with the button
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getSetName();

	    /**
	      * setSetName
	      * 
	      * Set the setname associated with the button
	      * 
	      * @param : const std::string& name
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setSetName(const std::string& name);

	    /**
	      * getButtonPosition
	      * 
	      * Return the button position
	      * 
	      * @return virtual int 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual int getButtonPosition();

	    /**
	      * setButtonPosition
	      * 
	      * Set the button position
	      * 
	      * @param : const int btnPos
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setButtonPosition(const int btnPos);

	    /**
	      * getTelephoneNumber
	      * 
	      * Return the telephone number associated button
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getTelephoneNumber();

	    /**
	      * setTelephoneNumber
	      * 
	      * Set the telephone number associated button
	      * 
	      * @param : const std::string& number
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setTelephoneNumber(const std::string& number);

	    /**
	      * getLabel
	      * 
	      * Return the label associated button 
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getLabel();

	    /**
	      * setLabel
	      * 
	      * Set the label associated button 
	      * 
	      * @return virtual void 
	      * @param : const std::string& label
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setLabel(const std::string& label);

	    /**
	      * getKey
	      * 
	      * Return the dial button primary key
	      * 
	      * @return unsigned long 
	      * 
	      * @exception <exceptions> Optional
	      */
        unsigned long getKey();

	    /**
	      * getAllItemChanges
	      * 
	      * Return all the fields that have been ammended
	      * 
	      * @return virtual TA_Base_Core::ItemChanges 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual TA_Base_Core::ItemChanges getAllItemChanges()
        {
            TA_Base_Core::ItemChanges retVal;
            return retVal;
        }

	    /**
	      * hasChanged
	      * 
	      * Indicate if any of the data has been changed
	      * 
	      * @return virtual bool 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual bool hasChanged() { return false; }

    private:
        /**
	     * Identifier
         */
        long m_uniqueIdentifier;


        /**
	      * Number used to generate the next identifier
          */
        static long s_nextAvailableIdentifier;

        /** 
         *  Pointer to TelephoneDirectLineHelper object
         */
        TelephoneDirectLineHelper * m_teSpeedDialSetHelper;
    };
} //close namespace TA_IRS_Core

#endif // !defined(ConfigTelephoneDirectLine_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
