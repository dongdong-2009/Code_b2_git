/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/IConfigPaDvaMessagePrivateVersion.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IConfigPaDvaMessagePrivateVersion - This is the interface supported by objects that allow configuration of
  * PaDvaMessagePrivateVersions. It provides methods for retrieving, settings, and writing data for an PaDvaMessagePrivateVersion object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined IConfigPaDvaMessagePrivateVersion_H
#define IConfigPaDvaMessagePrivateVersion_H

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/pa_4669/src/IPaDvaMessagePrivateVersion.h"

namespace TA_Base_Core
{

    class IConfigPaDvaMessagePrivateVersion : public virtual TA_Base_Core::IConfigItem, public virtual IPaDvaMessagePrivateVersion
    {

    public:

        virtual ~IConfigPaDvaMessagePrivateVersion() {};

        virtual void setLocationKey(unsigned long key) =0;
        virtual void setVersionData(const std::string& newVersion,
                                        IPaDvaMessagePrivateVersion::EVersionColumn column) = 0;

        

        /**
         * getDateCreated
         *
         * Returns the date created for this PaDvaMessagePrivateVersion
         *
         * @return The date created for this PaDvaMessagePrivateVersion as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this PaDvaMessagePrivateVersion.
         *
         * @return The date modified for this PaDvaMessagePrivateVersion as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;

        
        
        /**
         * deleteThisObject
         *
         * Removes this PaDvaMessagePrivateVersion from the database. 
         * The calling application MUST then delete this PaDvaMessagePrivateVersion object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaDvaMessagePrivateVersion was initially loaded from the database
         *      OR     - writePaDvaMessagePrivateVersionData() has already been called
         *      This PaDvaMessagePrivateVersion has not been deleted
         */
        virtual void deleteThisObject() = 0;

    };
} //close namespace TA_Base_Core
#endif // !defined IConfigPaDvaMessagePrivateVersion_H
