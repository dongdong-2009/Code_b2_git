/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ecs_manager/src/RightsMgrSingleton.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(_AFX_RIGHTSMGRSINGLETON_H_)
#define _AFX_RIGHTSMGRSINGLETON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "bus/security/rights_library/src/RightsLibrary.h"

// 10070-Start

namespace TA_IRS_App
{
	/*
	* CRightsMgrSingleton Class is responsible for creating and deleting the RightMgr
	* Library.The rights library factory is created only once for the whole 
	* application.	
	*/
	class CRightsMgrSingleton
	{
	private:
		CRightsMgrSingleton(const CRightsMgrSingleton& theSingleton);		
		CRightsMgrSingleton();

	public:
		static TA_Base_Bus::RightsLibrary *m_pRightsMgr;
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

	public:
        virtual ~CRightsMgrSingleton();
        static void deleteRightsMgr();
        static TA_Base_Bus::RightsLibrary* getRightsMgr();
	};
}
// 10070-End
#endif