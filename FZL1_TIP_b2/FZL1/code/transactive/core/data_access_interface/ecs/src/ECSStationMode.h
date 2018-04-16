 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ecs/src/ECSStationMode.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * ECSStationMode is an implementation of IECSStationMode. It holds the data specific to an ECSStationMode entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(ECSStationMode_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ECSStationMode_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/ecs/src/ECSStationModeHelper.h"
#include "core/data_access_interface/ecs/src/IECSStationMode.h"

namespace TA_IRS_Core
{


    class ECSStationMode : public IECSStationMode
    {

    public:



        /**
         * ECSStationMode (constructor)
         *
         * Construct an ECSStationMode class based around a key.
         *
         * @param key The key of this ECSStationMode in the database
         */
        ECSStationMode(const unsigned long key);

        /**
         * ECSStationMode (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ECSStationModeHelper::ECSStationModeHelper(IData*)
         *
         */
        ECSStationMode(unsigned long row, TA_Base_Core::IData& data);


        virtual ~ECSStationMode();

        virtual unsigned long getKey();
        virtual unsigned long getStationMode();
        virtual unsigned long getLocationKey();
        virtual std::string getOperation();
        virtual std::string getTrack();
        virtual std::string getCondition();
        virtual std::string getCongestionLogic();

        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the ECS Station Mode this is 
         * just the key in a string format e.g. "ECS Station Mode 1".
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
         * Make the data contained by this ECSStationMode as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:

        // Assignment ECSStationMode not used so it is made private
		ECSStationMode& operator=(const ECSStationMode &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the ECSStationModeHelper
        // has only been written for ConfigLocation objects and will not copy the ECSStationMode helper
        // for a read-only object (ie this one).
        ECSStationMode( const ECSStationMode& theECSStationMode);  


        TA_IRS_Core::ECSStationModeHelper * m_ECSStationModeHelper;
    };
} // closes TA_Core

#endif // !defined(ECSStationMode_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
