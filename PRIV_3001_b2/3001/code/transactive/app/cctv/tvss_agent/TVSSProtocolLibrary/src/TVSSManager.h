
#ifndef TVSS_MANAGER_H
#define TVSS_MANAGER_H

//#include "bus/cctv/TVSS_Agent/IDL/src/TVSSManagerCorbaDef.h"
#include "bus/cctv/tvss_agent/TVSSManager/IDL/src/TVSSManagerCorbaDef.h"
#include "core/corba/src/ServantBase.h"

namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
}

namespace TA_IRS_App
{
	class TVSSManager : public virtual POA_TA_Base_Bus::TVSSManagerCorbaDef,
						public virtual TA_Base_Core::ServantBase
	{
	public:

		/**
          * getInstance
          *
          * Gets the one and only AgentModeMonitor.
          *
          * @return     SwitchManager&
          *             The only SwitchManager.
          */
		
		static TVSSManager& getInstance();

		/**
          * ~SwitchManager
          *
          * Standard destructor.
          */

        virtual ~TVSSManager();

		void activate();

		/**
		 * Implement TVSSManagerCorbaDef Methods
		 */
		virtual void tvssClearImage(CORBA::Long trainId,CORBA::Char monitorId );
		virtual void tvssQuadScreen(CORBA::Long trainId,CORBA::Char monitorId, CORBA::Char cameraW, CORBA::Char cameraX, CORBA::Char cameraY, CORBA::Char cameraZ );
		virtual void tvssSingleScreen(CORBA::Long trannId, CORBA::Char monitorId, CORBA::Char cameraX );
		virtual void tvssSequencingScreen(CORBA::Long trannId, CORBA::Char monitorId, CORBA::Char sequenceId ); 
		virtual void tvssTrainBorneEvent(CORBA::Long trainID,const char* eventStr);
		virtual void tvssEquipmentAlarmQuery(CORBA::Long trainId, CORBA::Char tmfServer );
	private:
		
		/**
          * SwitchManager
          *
          * Standard constructor. This is private because this class is a singleton.
          * Use getInstance() to get the one and only instance of this class.
          */
        TVSSManager();

        //
        // Disable copy constructor and assignment operator
        //

        TVSSManager( const TVSSManager& theTVSSManager );
        TVSSManager& operator=( const TVSSManager& theTVSSManager );

		//
		// The one and only instance of this class.
		//

		static TVSSManager* m_theClassInstance;
		//
        // Protect singleton creation
        //

		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
	};
};
#endif