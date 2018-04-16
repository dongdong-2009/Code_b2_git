
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_IRS_App
{
	//
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable TVSSManager::m_singletonLock;
	TVSSManager* TVSSManager::m_theClassInstance = NULL;

	TVSSManager::TVSSManager()
	{
	}
	
	TVSSManager::~TVSSManager()
	{
		TA_Base_Core::ThreadGuard guard( m_singletonLock );
        
        TVSSManager::m_theClassInstance = NULL;
	}

	//
    // getInstance
    //
	TVSSManager& TVSSManager::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( NULL == m_theClassInstance )
		{
			// Create the one & only object.

			m_theClassInstance = new TVSSManager();
		}

		return *m_theClassInstance;
    }

	/**
	 * Implement Corba Interface
	 */
	void TVSSManager::tvssClearImage(CORBA::Long trainId,CORBA::Char monitorId)
	{

	}
	
	void TVSSManager::tvssQuadScreen(CORBA::Long trainId,CORBA::Char monitorId,CORBA::Char cameraW, CORBA::Char cameraX, CORBA::Char cameraY, CORBA::Char cameraZ)
	{

	}

	void TVSSManager::tvssSingleScreen(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char cameraX)
	{

	}

	void TVSSManager::tvssSequencingScreen(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char sequenceId)
	{

	}

	void TVSSManager::tvssTrainBorneEvent(CORBA::Long trainID,const char* eventStr)
	{
		;
	}

	void TVSSManager::tvssEquipmentAlarmQuery(CORBA::Long trainId, CORBA::Char tmfServer)
	{

	}
	void TVSSManager::activate()
	{
		//TA_ASSERT( AgentModeMonitor::getInstance().isInControlMode(), "The agent is not in Control mode." );
           
        // The TVSSManager handles access to the agent, so activate it using the agent's name.
        activateServantWithName( TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ) );
	}
}
