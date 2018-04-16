/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Justin Ebedes
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Implementation of the ISystemControllerAdminCorbaDef
  * interface, used by the System Manager.
  *
  */

#ifndef SYSTEMCONTROLLERADMIN_H
#define SYSTEMCONTROLLERADMIN_H

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/ISystemControllerAdminCorbaDef.h"
#include "core/corba/src/ServantBase.h"

namespace TA_Base_App
{
	class SystemController;	// Forward declaration

    class SystemControllerAdmin : public virtual POA_TA_Base_Core::ISystemControllerAdminCorbaDef,                           
                                  public virtual TA_Base_Core::ServantBase 
    {

    public:

        /**
          * Constructor
		  *
		  * @param systemController  A pointer to the parent SystemController object.
          */
        SystemControllerAdmin(SystemController* systemController);

        /**
          * Destructor
          */
        ~SystemControllerAdmin();

		/**
          * getProcessData
          *
          * Get a sequence with all the known processes for a System Controller
          */
        TA_Base_Core::ProcessDataSeq* getProcessData();

        /**
          * shutdown
          *
          * Shutdown a System Controller. The Process Monitors should sort out 
		  * between themselves how to run the processes that get dropped.
          */
        void shutdown(const char* hostname);

        /**
          * startProgram
          *
          * Start a process.
		  * 
          */
        void startProcess(const char* entityName);

        /**
          * changeAgentOperationMode
          *
          * Ask a System Controller to change the Operation mode of one of its
          * managed processes.
          * 
          */
        void changeAgentOperationMode(const char* entityName, 
                                      TA_Base_Core::EOperationMode operationMode);
        /**
          * stopProcess
          * 
          */
        void stopProcess(const char* entityName);

        /**
          * setRunParam
          *
          * Set a RunParam for a particular process
		  * 
          */
        void setRunParam(const char* entityName, const TA_Base_Core::RunParam& param);

		/**
		  * Returns the status of the agents running on this System Controller.
		  *
		  * @return AgentStatusEnum  An enum indiciating the overall status
		  *							 of the agents.
		  *
		  */
		TA_Base_Core::AgentStatusEnum getAgentsStatus();

        void deployProcesses( TA_Base_Core::EOperationMode normalOperationMode, CORBA::Boolean isStart ); // limin++ CL-21243

	private:

		/**
		  * A pointer to the parent SystemController object (needed to shutdown).
		  */
		SystemController* m_systemController;
    };

} // namespace TA_Base_App

#endif // SYSTEMCONTROLLERADMIN_H
