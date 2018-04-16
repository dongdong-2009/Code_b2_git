/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/IConfigAtsPlatform.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IConfigAtsPlatform - This is the interface supported by objects that allow configuration of
  * AtsPlatforms. It provides methods for retrieving, settings, and writing data for an AtsPlatform object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigAtsPlatform_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigAtsPlatform_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/ats/src/IAtsPlatform.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{

    class IConfigAtsPlatform : public virtual TA_Base_Core::IConfigItem, public virtual IAtsPlatform
    {

    public:

        
        virtual ~IConfigAtsPlatform() {};

		virtual void setPlatformId(unsigned long platformId) =0;
		virtual void setStationId(unsigned long id) =0; 

        /** 
          * setTisPlatformId
          *
          * sets the platform ID of this platform for sending
          * platform information to the STIS server.
          *
          * @param tisPlatformId a single digit number (1-9)
          */
        virtual void setTisPlatformId(unsigned char tisPlatformId) =0;
             
        /**
         * deleteThisObject
         *
         * Remove this AtsPlatform from the database. Once this method has been executed, the
         * AtsPlatformHelper object should be destroyed.
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
         * pre: - this AtsPlatform was initially loaded from the database
         *      - writeAtsPlatformData() has already been called
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
    
    typedef std::vector<IConfigAtsPlatform*> IConfigAtsPlatforms;
    typedef IConfigAtsPlatforms::iterator    IConfigAtsPlatformsIt;
} //close namespace TA_IRS_Core
#endif // !defined(IConfigAtsPlatform_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
