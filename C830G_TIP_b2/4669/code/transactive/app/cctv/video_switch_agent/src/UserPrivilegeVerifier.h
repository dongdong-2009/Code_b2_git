#if !defined(UserPrivilegeVerifier_FB29D17B_F4C1_4db8_9787_4FA82FC5DA60__INCLUDED_)
#define UserPrivilegeVerifier_FB29D17B_F4C1_4db8_9787_4FA82FC5DA60__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/01/25 17:58:20 $
  * Last modified by:  $Author: builder $
  *
  * Verifies the user privileges for the given function.
  */

#include <string>

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_IRS_App
{
	class UserPrivilegeVerifier
	{
	public:
        /**
          * UserPrivilegeVerifier
          *
          * Constructor that builds the rights library.
          */

        // Singleton class
//         UserPrivilegeVerifier();
        static UserPrivilegeVerifier& getInstance(); // HuangQi++

        /**
          * ~UserPrivilegeVerifier
          *
          * Standard destructor.
          */
       
        virtual ~UserPrivilegeVerifier() {};

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

        void verifyRights( const std::string& sessionId, TA_Base_Core::IEntityDataPtr data,
            unsigned long actionKey, const std::string& actionName );

    private:

        //
        // Disable copy constructor and assignment operator.
        //

        UserPrivilegeVerifier( const UserPrivilegeVerifier& theUserPrivilegeVerifier );
        UserPrivilegeVerifier& operator=( const UserPrivilegeVerifier& theUserPrivilegeVerifier );

        // private default constructor to make singleton class
        UserPrivilegeVerifier(); // HuangQi++

        // Singleton variables
        static UserPrivilegeVerifier* s_pInstance;
		static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock; // HuangQi++

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
