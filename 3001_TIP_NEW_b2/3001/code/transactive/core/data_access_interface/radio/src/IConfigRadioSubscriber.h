/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $Workfile:$
  * @author Alexis Laredo
  * @version $Revision: 1.1.2.2 $
  * Last modification: $Modtime:$
  * Last modified by: $Author: stuartw $
  * 
  * IConfigRadioSubscriber - This is the interface supported by objects that allow configuration of
  * Radio subscribers. It provides methods for retrieving, settings, and writing data for a RadioSubscriber object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigRadioSubscriber_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigRadioSubscriber_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/radio/src/IRadioSubscriber.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include "bus/radio/radio_bus/src/radiotypes.h"

namespace TA_IRS_Core
{

    class IConfigRadioSubscriber : public virtual TA_Base_Core::IConfigItem,
                                   public virtual IRadioSubscriber
    {

    public:

        
        virtual ~IConfigRadioSubscriber() {};

	    virtual void setResourceType(RadioResourceType type) =0;

	    virtual void setResourceId(unsigned long rid) =0;

	    virtual void setSSI(unsigned long ssi) =0;

	    virtual void setAlias(const std::string& alias) =0;

        /**
         * getDateCreated
         *
         * Returns the date created for this RadioSubscriber
         *
         * @return The date created for this RadioSubscriber as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception TA_Base_Core::DataException A TA_Base_Core::DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this RadioSubscriber.
         *
         * @return The date modified for this RadioSubscriber as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception TA_Base_Core::DataException A TA_Base_Core::DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;
        
        
        /**
         * deleteThisObject
         *
         * Remove this RadioSubscriber from the database. Once this method has been executed, the
         * RadioSubscriberHelper object should be destroyed.
         *
         * @exception DatabaseException 
         *            - if there is a problem writing the data to the database.
         *            - if references to this record exists in AT_FIXED_BLOCK_MAP table.  
		 *				These can be removed by specifying the cascade option.
         *
         * @param cascade
         *        - force the removal of this record by removing all references to this record
         *          in other tables (AT_FIXED_BLOCK_MAP).  This should be set to true to prevent the 
         *          DatabaseException from being thrown.
         *
         * pre: - this RadioSubscriber was initially loaded from the database
         *      - writeRadioSubscriberData() has already been called
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
    
    typedef std::vector<IConfigRadioSubscriber*> IConfigRadioSubscribers;
    typedef IConfigRadioSubscribers::iterator    IConfigRadioSubscribersIt;
} //close namespace TA_IRS_Core
#endif // !defined(IConfigRadioSubscriber_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
