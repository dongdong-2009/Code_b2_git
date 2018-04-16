/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/IConfigVideoStationId.h $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by: $Author: weikun.lin $
  * 
  * IConfigVideoStationId - This is the interface supported by objects that allow configuration of
  * VideoStationIds. It provides methods for retrieving, settings, and writing data for an VideoStationId object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigVideoStationId_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigVideoStationId_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/video_switch_agent/src/IVideoStationId.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{

    class IConfigVideoStationId : public virtual TA_Base_Core::IConfigItem,
                                  public virtual IVideoStationId
    {

    public:

        
        virtual ~IConfigVideoStationId() {};

        virtual void setStationId(unsigned long id) =0;
        virtual void setTaLocation(unsigned long track) =0;  
        
        /**
         * getDateCreated
         *
         * Returns the date created for this VideoStationId
         *
         * @return The date created for this VideoStationId as a time_t.
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
         * Returns the date modified for this VideoStationId.
         *
         * @return The date modified for this VideoStationId as a time_t.
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
         * Remove this VideoStationId from the database. Once this method has been executed, the
         * VideoStationIdHelper object should be destroyed.
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
         * pre: - this VideoStationId was initially loaded from the database
         *      - writeVideoStationIdData() has already been called
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

		/**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception TA_Base_Core::DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the VideoStationId object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This VideoStationId has not been deleted
         */
        virtual void applyChanges() = 0;
    
        
    };
    
    typedef std::vector<IConfigVideoStationId*> IConfigVideoStationIds;
    typedef IConfigVideoStationIds::iterator    IConfigVideoStationIdsIt;
} //close namespace TA_IRS_Core
#endif // !defined(IConfigVideoStationId_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
