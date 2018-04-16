/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/RightsMgrSingleton.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(_AFX_RIGHTSMGRSINGLETON_H_)
#define _AFX_RIGHTSMGRSINGLETON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

// 10070-Start
namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
}

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
		static CRightsMgrSingleton					   *m_pInstance;		
		static TA_Base_Bus::RightsLibrary	           *m_pRightsMgr;
		static TA_Base_Core::ReEntrantThreadLockable	m_singletonLock;		
		TA_Base_Bus::RightsLibraryFactory		        m_rightsFactory;				
	public:
		virtual ~CRightsMgrSingleton();		
		static CRightsMgrSingleton* getInstance();		
		TA_Base_Bus::RightsLibrary* getRightsMgr();
		void DeleteInstance();
	};
}
// 10070-End
#endif