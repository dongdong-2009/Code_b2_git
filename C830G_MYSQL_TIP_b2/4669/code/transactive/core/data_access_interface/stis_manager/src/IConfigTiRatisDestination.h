/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/stis_manager/src/IConfigTiRatisDestination.h $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IConfigTiRatisDestination - This is the interface supported by objects that allow configuration of
  * TiRatisDestinations. It provides methods for retrieving, settings, and writing data for an TiRatisDestination object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */
#ifndef ITI_RATIS_DESTIONATION_CONFIG_H_INCLUDED
#define ITI_RATIS_DESTIONATION_CONFIG_H_INCLUDED

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/stis_manager/src/ITiRatisDestination.h"

namespace TA_IRS_Core
{

    class IConfigTiRatisDestination : public virtual TA_Base_Core::IConfigItem, public virtual ITiRatisDestination
    {

    public:

        virtual ~IConfigTiRatisDestination() {};

     
        /**
         * deleteThisObject
         *
         * Removes this TiRatisDestination from the database. 
         * The calling application MUST then delete this TiRatisDestination object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this TiRatisDestination was initially loaded from the database
         *      OR     - writeTiRatisDestinationData() has already been called
         *      This TiRatisDestination has not been deleted
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

		virtual void setDestination(const std::string & destination) = 0;

    };
} //close namespace TA_IRS_Core
#endif // !defined(IConfigTiRatisDestination_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
