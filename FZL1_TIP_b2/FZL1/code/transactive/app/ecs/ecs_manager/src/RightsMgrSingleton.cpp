/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/RightsMgrSingleton.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This singleton class is responsible to create the rights library only once throughout the life of ECSManager
  *
  */

#include "app/ecs/ecs_manager/src/stdafx.h"
#include "app/ecs/ecs_manager/src/ecsmanager.h"
#include "app/ecs/ecs_manager/src/RightsMgrSingleton.h"
#include "core/synchronisation/src/ThreadGuard.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;
using TA_Base_Core::ThreadGuard;

namespace TA_IRS_App
{
	// 10070-Start
	// Rights Library created multiple times issue fix	
	//////////////////////////////////////////////////////////////////////////
	// Initialize static variables.	
	CRightsMgrSingleton* CRightsMgrSingleton::m_pInstance		  = NULL;	
	TA_Base_Bus::RightsLibrary* CRightsMgrSingleton::m_pRightsMgr = NULL;
	TA_Base_Core::ReEntrantThreadLockable CRightsMgrSingleton::m_singletonLock;
	//////////////////////////////////////////////////////////////////////////
	/*
	* Function Name : CRightsMgrSingleton()
	* Input Params  : None
	* Output Params : None
	* Return Value  : None
	* Description   : Default constructor - Private. 
	*/
	CRightsMgrSingleton::CRightsMgrSingleton()
	{
		
	}

	/*
	* Function Name : ~CRightsMgrSingleton()
	* Input Params  : None
	* Output Params : None
	* Return Value  : None
	* Description   : Destructor
	*/
	CRightsMgrSingleton::~CRightsMgrSingleton()
	{
		
	}

	/*
	* Function Name : getInstance()
	* Input Params  : None
	* Output Params : None
	* Return Value  : CRightsMgrSingleton*
	* Description   : Call this function to get the only instance of this class.
	*/
	CRightsMgrSingleton* CRightsMgrSingleton::getInstance()
	{
		// Lock before creating a new instance
		ThreadGuard guard(m_singletonLock);
		
		// If the instance null,create a new Instance.
		if(m_pInstance == NULL)
		{
			m_pInstance  = new CRightsMgrSingleton();
			if(m_pInstance == NULL)
				ASSERT(m_pInstance);		
		}	
		return m_pInstance;
	}

	/*
	* Function Name : getRightsMgr()
	* Input Params  : None
	* Output Params : None
	* Return Value  : RightsLibrary*
	* Description   : Returns the RightsLibrary pointer.If called first time creates it.
	*/
	TA_Base_Bus::RightsLibrary* CRightsMgrSingleton::getRightsMgr()
	{
		// Build rights library if necessary and return.
		if(m_pRightsMgr == NULL)
		{
			TA_Base_Bus::RightsLibraryFactory rightsFactory;			
			m_pRightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);		
		}
		return m_pRightsMgr;
	}

	/*
	* Function Name : DeleteInstance()
	* Input Params  : None
	* Output Params : None
	* Return Value  : None
	* Description   : Deletes only Instance of this class.Rights manager is also deleted.
	*/
	void CRightsMgrSingleton::DeleteInstance()
	{
		if(m_pInstance != NULL)
		{
			//Delete Any Instance of Rights Manager
			if(m_pRightsMgr != NULL)
				delete m_pRightsMgr;
			
			//Delete Only Instace of RightsMgr Singleton
			delete m_pInstance;
		}
	}
	// 10070-End	
}