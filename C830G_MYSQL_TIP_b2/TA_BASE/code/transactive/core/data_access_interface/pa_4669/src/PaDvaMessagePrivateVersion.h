 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersion.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaDvaMessagePrivateVersion is an implementation of IPaDvaMessagePrivateVersion. It holds the data specific to an PaDvaMessagePrivateVersion entry
  * in the database, and allows read-only access to that data.
  *
  */


#if !defined PaDvaMessagePrivateVersion_H
#define PaDvaMessagePrivateVersion_H

#include <string>

#include "core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersionHelper.h"
#include "core/data_access_interface/pa_4669/src/IPaDvaMessagePrivateVersion.h"

namespace TA_Base_Core
{


    class PaDvaMessagePrivateVersion : public IPaDvaMessagePrivateVersion
    {

    public:



        /**
         * PaDvaMessagePrivateVersion (constructor)
         *
         * Construct an PaDvaMessagePrivateVersion class based around a key.
         *
         * @param key The key of this PaDvaMessagePrivateVersion in the database
         */
        PaDvaMessagePrivateVersion(const unsigned long key);

        /**
         * PaDvaMessagePrivateVersion (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaDvaMessagePrivateVersionHelper::PaDvaMessagePrivateVersionHelper(IData*)
         *
         */
        PaDvaMessagePrivateVersion(unsigned long row, TA_Base_Core::IData& data);

        virtual ~PaDvaMessagePrivateVersion();

        virtual unsigned long getKey();
        virtual unsigned long getLocationKey();
        virtual std::string getMessageVersion(IPaDvaMessagePrivateVersion::ELocation location, unsigned long stationDvaMessageId);
        virtual std::string getVersionData(EVersionColumn column);

        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  
         * For the PA DVA Message Version this is 
         * just the key in a string format e.g. "PA Dva Message Version 1".
         *
         * @return The key description for this item as a std::string
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
         * invalidate
         *
         * Make the data contained by this PaDvaMessagePrivateVersion as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:

        // Assignment PaDvaMessagePrivateVersion not used so it is made private
		PaDvaMessagePrivateVersion& operator=(const PaDvaMessagePrivateVersion &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the PaDvaMessagePrivateVersionHelper
        // has only been written for ConfigLocation objects and will not copy the PaDvaMessagePrivateVersion helper
        // for a read-only object (ie this one).
        PaDvaMessagePrivateVersion( const PaDvaMessagePrivateVersion& thePaDvaMessagePrivateVersion);  

        TA_Base_Core::PaDvaMessagePrivateVersionHelper * m_paDvaMessagePrivateVersionHelper;
    };
} // closes TA_Base_Core

#endif // !defined PaDvaMessagePrivateVersion_H
