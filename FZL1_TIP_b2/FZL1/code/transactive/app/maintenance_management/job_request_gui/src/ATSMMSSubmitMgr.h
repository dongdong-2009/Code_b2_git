/*
* The source code in this file is the property of 
* Combuilder and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/job_request_gui/src/ATSMMSSubmitMgr.h $
* @author: Noel R. Tajanlangit
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2012/03/20 14:12:57 $
* Last modified by:  $Noel R. Tajanlangit $
* 
* This class is a singleton class which manages the instance of ATSMMSSubmitter.
*
*/


#pragma once
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "IATSMMSSubmitter.h"

namespace TA_IRS_App
{
	class ATSMMSSubmitMgr
	{
	public:
		~ATSMMSSubmitMgr(void);
		
		/*
		 *@function		: getInstance
		 *
		 *@Description	: static function to retrieve the instance of this class
		 *
		 *@param		: none
		 *
		 *@return		: pointer to ATSMMSSubmitMgr
		 */	
		static ATSMMSSubmitMgr* getInstance();
		
		/*
		 *@function		: releaseInstance
		 *
		 *@Description	: static function to release the instance of this class
		 *
		 *@param		: none
		 *
		 *@return		: none
		 */	
		static void releaseInstance();

		/*
		 *@function		: getATSSubmitter
		 *
		 *@Description	: creates an instance of ATSMMSSubmitter and returns the pointer to its interface IATSMMSSubmitter.
		 *
		 *@param		: none
		 *
		 *@return		: pointer to IATSMMSSubmitter
		 */	
		IATSMMSSubmitter* getATSSubmitter();

	private: // Methods
		ATSMMSSubmitMgr(void);

	private: // variables
		static ATSMMSSubmitMgr* m_pInstance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		TA_Base_Core::ReEntrantThreadLockable m_lockSubmitter;

		IATSMMSSubmitter* m_pATSSubmitter;
	};

}
