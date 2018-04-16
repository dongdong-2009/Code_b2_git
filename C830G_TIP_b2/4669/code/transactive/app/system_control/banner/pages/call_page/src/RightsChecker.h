/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/system_control/banner/pages/call_page/src/RightsChecker.h $
  * @author:  Dirk McCormick - Copied from Configuration Editor rights checker by Karen G
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class is used to determine if a user has access rights for a
  * particular action that the call banner can perform.
  */

#ifndef APP_CALL_BANNER_RIGHTS_CHECKER
#define APP_CALL_BANNER_RIGHTS_CHECKER

namespace TA_Base_Bus
{
    class RightsLibrary;
}


namespace TA_IRS_App
{
	class RightsChecker
	{
	public:
		/**
         *
		 * Constructor
         *
         * @exception DataException       if the resource key cannot be
         * @exception DatabaseException   retrieved
		 */
		RightsChecker();


		/**
		 * Destructor
		 */
		~RightsChecker();


        /**
         * canPerformAction
         *
         * This checks if the specified action can be performed on the banner
         * by the current session.
         *
         * @param action  The action to test
         *
         * @return  true if the action can be performed, false otherwise
         */
        bool canPerformAction(unsigned long action);


    private:
        TA_Base_Bus::RightsLibrary* m_rightsLibrary;

        unsigned long m_bannerResourceId;
	};
}

#endif APP_CALL_BANNER_RIGHTS_CHECKER
