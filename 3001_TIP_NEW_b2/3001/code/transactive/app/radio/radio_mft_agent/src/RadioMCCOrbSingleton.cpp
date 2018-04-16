
#include "RadioMCCOrbSingleton.h"

namespace TA_IRS_App
{
	RadioMCCOrbSingleton * RadioMCCOrbSingleton::m_singelonRadioMCCOrb = NULL;
	TA_Base_Core::ReEntrantThreadLockable RadioMCCOrbSingleton::m_threadLock;
	
	RadioMCCOrbSingleton::RadioMCCOrbSingleton () : m_bInit (false),
		m_Obj(NULL),
		m_Orb(NULL),
		m_childPoa (NULL),
		m_rootPoa(NULL),
		m_poaMngr(NULL)
	{
	}
	
	RadioMCCOrbSingleton::~RadioMCCOrbSingleton()
	{

	}

	RadioMCCOrbSingleton * RadioMCCOrbSingleton::getInstance()
	{
		{
			TA_THREADGUARD (m_threadLock);
			
			if (!m_singelonRadioMCCOrb)
			{
				m_singelonRadioMCCOrb = new RadioMCCOrbSingleton;
			}
			return m_singelonRadioMCCOrb;
		}
		
	}

	void RadioMCCOrbSingleton::removeInstance ()
	{
		{
			TA_THREADGUARD (m_threadLock);
			if (m_singelonRadioMCCOrb)
			{
				delete m_singelonRadioMCCOrb;
				m_singelonRadioMCCOrb = NULL;

			}
		}
	}

	CORBA::ORB_var RadioMCCOrbSingleton::getOrb ()
	{
		return m_Orb;
	}

	PortableServer::POA_var RadioMCCOrbSingleton::getRootPoa ()
	{
		return m_rootPoa;
	}

	PortableServer::POAManager_var RadioMCCOrbSingleton::getPoaMngr ()
	{
		return m_poaMngr;
	}

	PortableServer::POA_var RadioMCCOrbSingleton::getChildPoa ()
	{
		return m_childPoa;
	}

	CORBA::Object_var RadioMCCOrbSingleton::getObject()
	{
		return m_Obj;
	}

	void RadioMCCOrbSingleton::intializeMCCORB (const std::string & strIor)
	{
		m_strIor = strIor;
		
		if (!m_bInit)
		{
			initialize();
			m_bInit = true;
		}
		
	}

	void RadioMCCOrbSingleton::initialize ()
	{
		int   nArgC    = 2;
		char *pszArgV[2] = {0};
		pszArgV[0]  = "RAPIClient";
		char ior[1024];
		strcpy(ior,m_strIor.c_str());
		pszArgV[1]  = ior;

 		const char* options[][2] = { { "offerBiDirectionalGIOP", "1" },{"traceLevel","40"},{"traceFile","ORB_LOG.txt"},{"acceptBiDirectionalGIOP","1"},				{ "maxGIOPConnectionPerServer", "1"},
						   		{ "oneCallPerConnection", "0"},{"endPoint","giop:tcp:mft:9020"}, { 0, 0 } };
 
 		//Initialize ORB
 		m_Orb = CORBA::ORB_init(nArgC, pszArgV,"omniORB4",options);
 
 		m_Obj = m_Orb->resolve_initial_references("RootPOA");
 		m_rootPoa = PortableServer::POA::_narrow(m_Obj.in());
 		m_poaMngr = m_rootPoa->the_POAManager();

 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "POA Manager activated");

 		// Create a POA with the Bidirectional policy
 		CORBA::PolicyList pl;
 		pl.length(1);
 		::CORBA::Any a;
 		a<<= BiDirPolicy::BOTH;
 		pl[0] = m_Orb->create_policy(BiDirPolicy::BIDIRECTIONAL_POLICY_TYPE, a);
 
 
 		try 
 		{
 			m_childPoa = m_rootPoa->find_POA("childPOA", false);
 		}			
 		catch (const CORBA::Exception &ex) 
 		{
 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "childPOA not found");
 			m_childPoa = m_rootPoa->create_POA("childPOA", m_poaMngr.in(), pl);
 		}
 

 		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Portable server created.");
 		m_poaMngr->activate();
 
 		// Get the reference for server.
 		m_Obj = m_Orb->string_to_object(ior);
 		// Register a CallBack object in this process.
	}

}