/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/user_settings/src/UserSettingsAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by:  $Author: grace.koh $
  * 
  * UserSettingsAccessFactory is a singleton that is used to generate and retrieve 
  * IUserSettings objects.
  */


#if !defined(UserSettingsAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)
#define UserSettingsAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_

#include <string>

namespace TA_IRS_Core
{
    class IPidSettings;

    class PidDataAccessFactory
    {

    public:
        virtual ~PidDataAccessFactory();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a PidDataAccessFactory object.
         */
        static PidDataAccessFactory& getInstance()
        {
            static PidDataAccessFactory fact;
            return fact;
        }

        /**
         * getUserSettings
         *
         * Retrieves the user settings for the specified operator and application type.
         *
         * @param string - The sessionId which we will use to retrieve the operator
         *                        key from.
         * @param unsigned long - The application key which will be used to retrieve the settings
         *
         * @return A pointer to an object conforming to the IUserSettings interface.
         *
         */
        IPidSettings* getUserSettings(std::string sessionId = "", unsigned long appType = 0);

    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        PidDataAccessFactory() { };
        PidDataAccessFactory& operator=(const PidDataAccessFactory &) { };
        PidDataAccessFactory( const PidDataAccessFactory& ) {};

    };
} //end namespace TA_Base_Core

#endif // !defined(UserSettingsAccessFactory_43BEBF21_DC71_4d0b_BCE8_ADA75F25CA00__INCLUDED_)
