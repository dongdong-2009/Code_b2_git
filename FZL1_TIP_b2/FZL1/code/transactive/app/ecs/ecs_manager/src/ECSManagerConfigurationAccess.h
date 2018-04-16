#ifndef ECSMANAGERCONFIGURATIONACCESS_H
#define ECSMANAGERCONFIGURATIONACCESS_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Singleton to manage Access to the ECSManager Configuration.
  *
  * NOTE: TA_Base_Core::ECSManager is thread-safe.
  *
  */
//  -Prasanna- Prevent Warning C4786 being generated.
#pragma warning(disable: 4786) 
//  -Prasanna- 
#include <string>

#include "app/ecs/ecs_manager/src/resource.h"

namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
    class ECSManager;
}


namespace TA_IRS_App
{
    class ECSManagerConfigurationAccess
    {
    public:
        virtual ~ECSManagerConfigurationAccess();

   
        /** 
          * getInstance
          *
          * Return reference to this singleton.
          *
          * @return reference to this singleton.
          */
        static ECSManagerConfigurationAccess& getInstance();


       /**
         * removeInstance
         *
         * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
         * used upon program termination (e.g. from main()) so that Purify does not consider this class
         * and its members to be leaks.
         */
        static void removeInstance();


        /** 
          * setConfigurationSource
          *
          * Sets the source of the configuration.
          *
          * @param configurationSource - Reference to the TA_Base_Core::ECSManager this
          *                              singleton is to manages access to.
          */
        void setEntityConfiguration( TA_Base_Core::ECSManager& entityConfiguration );


        // 
        // enumerated type corresponding to value of the DialogType runParam parameter
        //
        enum EECSDialogType
        {
            STATION_MODES,
            STATION_MODES_OVERVIEW,
            NORMAL_MASTER_MODES,
            EMERGENCY_MASTER_MODES,
            EMERGENCY_MASTER_MODES_FOR_ZONE,
			CONGESTION_OPERATION_SETTINGS,
			TIMETABLE_CHANGE,
			// 10745g-Start
			// Overall ECSTimetable Setting dialog
			OVERALL_STATION_TIMETABLE_SETTING_MODE,
			// 10745g-End
            INVALID_VALUE // Used to flag incorrectly configured value for the DialogType parameter
        };


        /** 
          * getDialogType
          *
          * Returns an EECSDialogType corresponding to dialog type set as run param on the 
          * command line
          *
          * @return enum corresponding to dialog type set as run param on the command line.
          */
        EECSDialogType getDialogType();


        /** 
          * getConfiguration
          *
          * Accessor to TA_ECSManager this singleton is managing access for.
          *
          * @return reference to the TA_Base_Core::ECSManager this class is managing access to.
          */
        TA_Base_Core::ECSManager& getEntityConfiguration();

		unsigned int getDialogIDTemplate();

    private:
        /** 
          * Default Constructor
          *
          * Constructor is private so can control number of instances
          */
        ECSManagerConfigurationAccess();


        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        ECSManagerConfigurationAccess( const ECSManagerConfigurationAccess& );


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        ECSManagerConfigurationAccess& operator=( const ECSManagerConfigurationAccess& );


        /** 
          * getDialogTypeValueFromString
          *
          * Helper function to convert the RunParam DialogType value to enum and store so
          * as to be accessed by clients of class.
          *
          * @param std::string - string to be mapped to dialog tyup enum.
          *
          * @return EECSDialogType mapped corresponding to string passed in.
          */
        static EECSDialogType getDialogTypeValueFromString( const std::string& dialogTypeStr );


    private:

        // pointer to the single instance of this class
        static ECSManagerConfigurationAccess* m_theInstance;

        // Semaphore to thread guard this singleton
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        // Array of possible values mapped to their Enumerated Type value
        struct DialogTypeValuePair
        {
            const char*  stringValue;
            EECSDialogType typeValue;
        };

        static const DialogTypeValuePair DIALOG_TYPE_VALUE_ARRAY[];

        // pointer to ECSManager entity configuration object
        TA_Base_Core::ECSManager* m_entityConfiguration;

        // flag to indicate if DialogType has been converted and stored.
        bool m_isDialogTypeInitialised;

        EECSDialogType m_dialogType;
    };
} // namespace TA_IRS_App

#endif // ECSMANAGERCONFIGURATIONACCESS_H
