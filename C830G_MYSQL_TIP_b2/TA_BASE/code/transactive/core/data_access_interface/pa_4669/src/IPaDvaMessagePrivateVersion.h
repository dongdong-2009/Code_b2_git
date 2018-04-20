/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/IPaDvaMessagePrivateVersion.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IPaDvaMessagePrivateVersion is an interface to a PaDvaMessagePrivateVersion object.  It allows the PaDvaMessagePrivateVersion object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  */

#if !defined IPaDvaMessagePrivateVersion_H
#define IPaDvaMessagePrivateVersion_H

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IPaDvaMessagePrivateVersion;

    class IPaDvaMessagePrivateVersion : public virtual TA_Base_Core::IItem
    {
    public:
        enum EVersionColumn
        {
            PRIVATE_ADHOC1,
            PRIVATE_ADHOC2,
            PRIVATE_ADHOC3,
            PRIVATE_ADHOC4,
            PRIVATE_ADHOC5,
            PRIVATE_ADHOC6,
            PRIVATE_PRERECORDED,

            VERSION_COLUMN_MAX
        };

        enum ELocation
        {
            DEPOT,
            OCC,
            STATION
        };

        virtual unsigned long getKey() =0;
        virtual unsigned long getLocationKey() =0;
        virtual std::string getMessageVersion(IPaDvaMessagePrivateVersion::ELocation location, unsigned long stationDvaMessageId) = 0;
        virtual std::string getVersionData(EVersionColumn column) = 0;

        virtual ~IPaDvaMessagePrivateVersion() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
    };

    typedef std::vector<IPaDvaMessagePrivateVersion*> IPaDvaMessagePrivateVersions;
    typedef IPaDvaMessagePrivateVersions::iterator    IPaDvaMessagePrivateVersionsIt;

} //close namespace TA_Base_Core


#endif // !defined IPaDvaMessagePrivateVersion_H
