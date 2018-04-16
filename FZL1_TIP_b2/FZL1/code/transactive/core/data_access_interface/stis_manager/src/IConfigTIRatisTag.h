/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/stis_manager/src/IConfigTIRatisTag.h $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * IConfigTiRatisTag - This is the interface supported by objects that allow configuration of
  * TiRatisTags. It provides methods for retrieving, settings, and writing data for an TiRatisTag object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */
#ifndef ITI_RATIS_TAG_CONFIG_H_INCLUDED
#define ITI_RATIS_TAG_CONFIG_H_INCLUDED

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/stis_manager/src/ITiRatisTag.h"

namespace TA_IRS_Core
{

    class IConfigTiRatisTag : public virtual TA_Base_Core::IConfigItem, public virtual ITiRatisTag
    {

    public:

        virtual ~IConfigTiRatisTag() {};


      
        /**
         * deleteThisObject
         *
         * Removes this TiRatisTag from the database. 
         * The calling application MUST then delete this TiRatisTag object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this TiRatisTag was initially loaded from the database
         *      OR     - writeTiRatisTagData() has already been called
         *      This TiRatisTag has not been deleted
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

        /**
         * getModifiedKey
         *
         * @return the modified (if applicable) primary key, but if the
         *          key hasn't been modified, returns the actual primary key
         *
         * @see setKey
         */
        virtual unsigned long getModifiedKey() = 0;

		virtual void setTagGroup(const std::string & tagGroup) = 0;

		virtual void setDetail (const std::string& detail) = 0;

    };
} //close namespace TA_IRS_Core
#endif // !defined(IConfigTiRatisTag_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
