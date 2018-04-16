/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ecs_manager/src/RightsMgrSingleton.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This singleton class is responsible to create the rights library only once throughout the life of ECSManager
  *
  */

#include "app/ecs/ecs_manager/src/RightsMgrSingleton.h"
#include "core/synchronisation/src/ThreadGuard.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{
	// 10070-Start
	// Rights Library created multiple times issue fix	
	//////////////////////////////////////////////////////////////////////////
	// Initialize static variables.		
	TA_Base_Bus::RightsLibrary* CRightsMgrSingleton::m_pRightsMgr = NULL;
	TA_Base_Core::ReEntrantThreadLockable CRightsMgrSingleton::m_singletonLock;
	//////////////////////////////////////////////////////////////////////////

	CRightsMgrSingleton::CRightsMgrSingleton()
	{
	}

	CRightsMgrSingleton::~CRightsMgrSingleton()
	{
	}

	TA_Base_Bus::RightsLibrary* CRightsMgrSingleton::getRightsMgr()
    {
		// Build rights library if necessary and return.
        if ( NULL == m_pRightsMgr )
        {
            // Lock before creating a new instance
            TA_Base_Core::ThreadGuard guard(m_singletonLock);

            if ( NULL == m_pRightsMgr )
            {
                TA_Base_Bus::RightsLibraryFactory rightsFactory;
                m_pRightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
            }
		}

		return m_pRightsMgr;
	}

	void CRightsMgrSingleton::deleteRightsMgr()
    {
        //Delete Any Instance of Rights Manager
        if (m_pRightsMgr != NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_singletonLock);

            if (m_pRightsMgr != NULL)
            {
                delete m_pRightsMgr;
                m_pRightsMgr = NULL;
            }
        }
	}
	// 10070-End	
}