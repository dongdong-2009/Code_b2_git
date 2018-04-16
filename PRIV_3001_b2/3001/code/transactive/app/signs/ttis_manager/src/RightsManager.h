/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/RightsManager.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * A singleton used to check rights.
  * It posts out windows messages to a window when it should re-check rights.
  * It is also used to handle server online and server offline - by denying
  * some actions.
  *
  * TRAIN_TIS_ISCS_LIBRARY_UPGRADE
  * TRAIN_TIS_TIME_SCHEDULE_SET_CURRENT
  * TRAIN_TIS_TIME_SCHEDULE_EDIT
  * TRAIN_TIS_FREETEXT_MESSAGE
  * TRAIN_TIS_STORED_MESSAGE
  * TRAIN_TIS_DOWNLOAD_LIBRARY
  * TRAIN_TIS_DOWNLOAD_SCHEDULE
  * TRAIN_TIS_RESET_MESSAGE
  * RADIO_MANAGE_TRAIN_SELECTIONS
  */


#if !defined(RIGHTSMANAGER_H)
#define RIGHTSMANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"

#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/data_access_interface/src/PrimitiveWrapper.h"


namespace TA_IRS_App
{
    class RightsManager : public virtual TA_Base_Core::RunParamUser
    {

    public:

        /**
         * ~RightsManager
         *
         * Standard destructor.
         */

        virtual ~RightsManager();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      RightsManager&
         *              A reference to an instance of a RightsManager object.
         */
        static RightsManager& getInstance();


        /**
          * registerForRightsChanges
          *
          * When rights change, window will be given a
          * WM_UPDATE_RIGHTS message. It should re-check its rights.
          *
          * @param window       The window to be notified of change.
          */
        void registerForRightsChanges(CWnd* window);


        /**
          * deregisterForRightsChanges
          *
          * window is no longer interested in being notified of rights changes.
          *
          * @param window       The window to be removed
          */
        void deregisterForRightsChanges(CWnd* window);


        /**
          * canDisplayFreeText
          *
          * If true, the operator can display a free text message.
          * TRAIN_TIS_FREETEXT_MESSAGE
          */
        bool canDisplayFreeText();


        /**
          * canDisplayPredefined
          *
          * If true, the operator can display a predefined message.
          * TRAIN_TIS_STORED_MESSAGE
          */
        bool canDisplayPredefined();


        /**
          * canClear
          *
          * If true, the operator can send a clear request.
          * TRAIN_TIS_RESET_MESSAGE
          */
        bool canClear();


        /**
          * canUpgradeISCS
          *
          * If true, the operator can upgrade the predefined message library.
          * TRAIN_TIS_ISCS_LIBRARY_UPGRADE
          */
        bool canUpgradeISCS();


        /**
          * canDownloadUpgradeLibrary
          *
          * If true the operator can download/upgrade the predefined
          * message library to trains.
          * TRAIN_TIS_DOWNLOAD_LIBRAY
          */
        bool canDownloadUpgradeLibrary();


        /**
          * canEditSchedule
          *
          * If true the operator can create/edit/delete
          * train time schedules
          * TRAIN_TIS_TIME_SCHEDULE_EDIT
          */
        bool canEditSchedule();


        /**
          * canSetCurrentSchedule
          *
          * If true the operator can set the current train time schedule
          * TRAIN_TIS_TIME_SCHEDULE_SET_CURRENT
          */
        bool canSetCurrentSchedule();


        /**
          * canDownloadUpgradeSchedule
          *
          * If true the operator can download/upgrade the current
          * train time schedule to trains.
          * TRAIN_TIS_DOWNLOAD_SCHEDULE
          */
        bool canDownloadUpgradeSchedule();


        /**
          * canModifyTrainGroups
          *
          * If true the operator can create/edit/delete
          * train groups.
          * RADIO_MANAGE_TRAIN_SELECTIONS
          */
        bool canModifyTrainGroups();


        /**
         * onRunParamChange
         *
         * This tells the application when a RunParam has changed. This should check the
         * name to see that the parameter that has changed is the SESSIONID.
         *
         * @param string& name -  The name of the changed parameter
         * @param string& value - The new value for the parameter
         */
        void onRunParamChange(const std::string& name, const std::string& value);


        /**
          * setServerStatus
          *
          * This is used to handle server online
          * and server offline messages.
          *
          * @param online whether the server is online or not.
          */
        void setServerStatus(bool online);


    private:

        /**
         * RightsManager
         *
         * Private constructors.
         */
        RightsManager();
        RightsManager& operator=(const RightsManager &) {};
        RightsManager( const RightsManager& ) {};


        /**
          * checkPermission
          *
          * Checks the permission for the given action on the given resource
          *
          * @param actionKey
          * @param resourceKey
          *
          * @return whether the action is permitted or not
          */
        bool checkPermission(unsigned long actionKey, unsigned long resourceKey);


        /**
          * postMessageToWindows
          *
          * post the given message to the given vector of windows
          *
          * @param windows  A vector of windows to post the message to
          * @param message  The message number to post
          *
          */
        void postMessageToWindows(const std::vector<CWnd*>& windows, int message);


        /**
          * getTrainAgentResourceKey
          *
          * @return the resource key for the local train agent.
          */
        unsigned long getTrainAgentResourceKey();


        /**
          * getTisAgentResourceKey
          *
          * @return the resource key for the local tis agent.
          */
        unsigned long getTisAgentResourceKey();


        // class instance
        static RightsManager* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;


        // threadlock for registration/deregistration
        TA_Base_Core::NonReEntrantThreadLockable m_windowListLock;

        // the vector of windows
        std::vector<CWnd*> m_windowsToNotify;

        // whether this GUI will actually be able to contact any agents or not
        bool m_serverIsOnline;

        // the rights library to use
        TA_Base_Bus::RightsLibrary* m_rightsLibrary;

        // the agent resource keys
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_trainAgentResourceKey;
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_ttismanagerResourceKey;

    };

} // end namespace TA_IRS_App

#endif
