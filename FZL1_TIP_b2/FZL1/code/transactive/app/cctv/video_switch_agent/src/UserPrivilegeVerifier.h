#if !defined(UserPrivilegeVerifier_FB29D17B_F4C1_4db8_9787_4FA82FC5DA60__INCLUDED_)
#define UserPrivilegeVerifier_FB29D17B_F4C1_4db8_9787_4FA82FC5DA60__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/30 17:42:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Verifies the user privileges for the given function.
  */

#include <string>

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{
    class InternalUserPrivilegeVerifier;
    class UserPrivilegeVerifier
    {
        public:
            UserPrivilegeVerifier();
            ~UserPrivilegeVerifier() {};
            void verifyRights(const std::string& sessionId, TA_Base_Core::IEntityDataPtr data,
                              unsigned long actionKey, const std::string& actionName);
        private:
            static InternalUserPrivilegeVerifier* s_pInstance;
            static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;
    };

    class InternalUserPrivilegeVerifier
    {
            friend class UserPrivilegeVerifier;
        public:

        private:
            /**
             * verifyRights
             *
             * Verifies that the user has the rights to perform the
             * specified action on the given resource.  If not, an
             * exception is thrown.
             *
             * @param      const std::string& sessionId
             *             Used to look up the user's profiles.
             * @param      IEntityData& data
             *             The database object from which the resource key can be found.
             * @param      unsigned long actionKey
             *             The key to the action the user is trying to perform.
             * @param      const std::string& actionName
             *             The name of the action the user is trying to perform.
             *             This will be used in any error messages/exceptions
             *             that are logged/thrown.
             *
             * @exception  VideoSwitchAgentException
             *             Thrown if:
             *             1. The user does not have the required rights.
             *             2. The user's rights cannot be determined.
             *             3. The resource key cannot be retrieved from the database.
             */

            void verifyRights(const std::string& sessionId, TA_Base_Core::IEntityDataPtr data,
                              unsigned long actionKey, const std::string& actionName);

            //
            // Disable copy constructor and assignment operator.
            //
            /**
             * UserPrivilegeVerifier
             *
             * Constructor that builds the rights library.
             */

            InternalUserPrivilegeVerifier();

            /**
              * ~UserPrivilegeVerifier
              *
              * Standard destructor.
              */

            virtual ~InternalUserPrivilegeVerifier() {};

            InternalUserPrivilegeVerifier(const InternalUserPrivilegeVerifier& theUserPrivilegeVerifier);
            InternalUserPrivilegeVerifier& operator=(const InternalUserPrivilegeVerifier& theUserPrivilegeVerifier);

            //
            // The Rights factory and library.  Use an auto_ptr for the library,
            // so that it is automatically cleaned up.
            //

            TA_Base_Bus::RightsLibraryFactory m_rightsFactory;
            std::auto_ptr< TA_Base_Bus::RightsLibrary > m_rightsLibrary;

            //
            // The decision module used to determine if the user has the required rights.
            //

            TA_Base_Bus::EDecisionModule m_decisionModule;
    };

} // TA_IRS_App

#endif // UserPrivilegeVerifier_FB29D17B_F4C1_4db8_9787_4FA82FC5DA60__INCLUDED_
