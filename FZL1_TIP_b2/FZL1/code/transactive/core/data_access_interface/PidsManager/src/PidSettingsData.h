/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_TIP/TA_BASE/transactive/core/data_access_interface/user_settings/src/UserSettingsData.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/10/31 12:12:40 $
  * Last modified by:  $Author: grace.koh $
  * 
  * This class contains the database retrievals and writes for the UserSettings library.
  * It implements the IUserSettings interface.
  */


#if !defined(SettingsData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
#define UserSettingsData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_

#include <map>
#include "IPidSettings.h"

namespace TA_IRS_Core
{
	class PidSettingsData : public IPidSettings
    {

    public:

        /**
         * Constructor
         *
         * @param string - This is the current user's session Id. This will be stored
         *                        so the settings are saved for the same user as when they were
         *                        loaded.
         * @param unsigned long - This is the application type. This is needed to determine
         *                        which user settings to load/save.
         */
        PidSettingsData(std::string sessionId, unsigned long appType);

        /**
         * Destructor
         */
        virtual ~PidSettingsData();

        
        
        
        virtual void setSetting(const int id, const std::wstring& name, const std::wstring& value);

		virtual PidMessages& getPidMessages();
        
        /**
         * addSetting
         *
         * This adds a new setting into the database and then adds it to the local cache.
         *
         * @param string - This is the name of the setting to add
         * @param string - This is the value to set for the specified setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingsException - This is thrown if the user setting data was
         *                                        invalid in some way.
         */
        virtual void addSetting(const int id, const std::wstring& name, const std::wstring& value);

    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        PidSettingsData& operator=(const PidSettingsData &) { };
        PidSettingsData( const PidSettingsData& ) {};

        /**
         * reload
         *
         * This loads all settings from the database.
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingException - This is thrown if duplicate settings appear in
         *                                       the database.
         */
        void reload();

        /**
         * updateSettingInDatabase
         *
         * This updates a setting in the database.
         *
         * @param string - This is the name of the setting to update
         * @param unsigned long - This is the value to set for the specified setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or the SQL failed to execute for
         *                                some reason.
         */
        void updateSettingInDatabase(const int id, const std::wstring& name, const std::wstring& value);

        /**
         * insertSettingIntoDatabase
         *
         * This inserts a new setting into the database.
         *
         * @param string - This is the name of the setting to insert
         * @param unsigned long - This is the value to set for the specified setting
         *
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or the SQL failed to execute for
         *                                some reason.
         */
        void insertSettingIntoDatabase(const int id, const std::wstring& name, const std::wstring& value);

        
        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
		std::string getLocalDatabaseName();

        //
        // These constants are used for retrieving data from the IData object.
        //

        //
        // We will use this type of map to store all our settings
        // as (name, value).
        //
        //typedef std::map<std::string, std::string> Settings;

        //
        // This holds all the current, configured settings retrieved from the
        // database
        //
        //Settings m_settings;
		PidMessages m_pidMsgs;

        //
        // We want to store the sesionID and application type so that when we go to
        // save the settings later we use the same operator and application type.
        //
		std::string m_sessionId;
        unsigned long m_applicationType;

        //
        // This indicates whether we currently have valid data or not. This is
        // initialised to false and is set to true once we do a read from the
        // database.
        //
        bool m_isValidData;

        std::string m_localDatabase;

    };
} //end namespace TA_Base_Core

#endif // !defined(UserSettingsData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
