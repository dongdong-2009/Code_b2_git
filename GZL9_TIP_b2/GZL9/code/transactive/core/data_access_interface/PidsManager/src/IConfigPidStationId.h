/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/PidsManager/src/IConfigPidStationId.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * IConfigPidStationId - This is the interface supported by objects that allow configuration of
  * PidStationIds. It provides methods for retrieving, settings, and writing data for an PidStationId object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigPidStationId_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigPidStationId_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/PidsManager/src/IPidStationId.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{

    class IConfigPidStationId : public virtual TA_Base_Core::IConfigItem,
                                  public virtual IPidStationId
    {

    public:

        
        virtual ~IConfigPidStationId() {};

        virtual void setStationId(unsigned long id) =0;
        virtual void setTaLocation(unsigned long track) =0;  
        
        /**
         * getDateCreated
         *
         * Returns the date created for this PidStationId
         *
         * @return The date created for this PidStationId as a time_t.
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
         * Returns the date modified for this PidStationId.
         *
         * @return The date modified for this PidStationId as a time_t.
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
         * Remove this PidStationId from the database. Once this method has been executed, the
         * PidStationIdHelper object should be destroyed.
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
         * pre: - this PidStationId was initially loaded from the database
         *      - writePidStationIdData() has already been called
         */
        virtual void deleteThisObject() = 0;

		/**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception TA_Base_Core::DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the PidStationId object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This PidStationId has not been deleted
         */
        virtual void applyChanges() = 0;
    
        
    };
    
    typedef std::vector<IConfigPidStationId*> IConfigPidStationIds;
    typedef IConfigPidStationIds::iterator    IConfigPidStationIdsIt;
} //close namespace TA_IRS_Core
#endif // !defined(IConfigPidStationId_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
