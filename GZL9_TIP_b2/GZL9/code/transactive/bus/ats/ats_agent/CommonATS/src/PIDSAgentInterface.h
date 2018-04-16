/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\PIDSAgentInterface.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * The object that responsible for sending data to PIDS Agent
  */

#ifndef _PIDS_AGENT_INTERFACE_H_
#define _PIDS_AGENT_INTERFACE_H_

#include "core/naming/src/NamedObject.h"
#include "bus/signs/pids_agent/IDL/src/IPIDSManagerCorbaDef.h"
#include "core/threads/src/QueueProcessor.h"
#include "boost/shared_ptr.hpp"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/ats/ats_agent/CommonATS/src/IPIDSAgentInterface.h"
#include <vector>

namespace TA_IRS_Bus
{
	typedef TA_Base_Core::NamedObject< TA_Base_Bus::IPIDSManagerCorbaDef,
									   TA_Base_Bus::IPIDSManagerCorbaDef_ptr,
									   TA_Base_Bus::IPIDSManagerCorbaDef_var >
									   PIDSNamedObject;

	class PIDSAgentInterface : public IPIDSAgentInterface
	{
	public:
		/**
		 * Constructor
		 */
		PIDSAgentInterface();

		/**
		 * Destructor
		 */
		virtual ~PIDSAgentInterface();

		/**
		 * initializePIDSObject
		 *
		 * Method used to initialize PIDS Object
		 * 
		 */
		void initializePIDSObject();

		/**
		 * processPIDSInformation
		 *
		 * Method used to process PIDS Information
		 *
		 * @param data : DataBlock object
		 */
		void processPIDSInformation( const TA_Base_Bus::DataBlock<WORD>& pidsData );

		/**
		 * processPIDSInformation
		 *
		 * Method used to process PIDS Information
		 *
		 * @param data : vector data 
		 */
		void processPIDSInformation(const std::vector<unsigned char>& pidsData);

	private:
		/**
		 * Disable copy and assignment operator
		 */
		PIDSAgentInterface( const PIDSAgentInterface& );
		PIDSAgentInterface& operator= (const PIDSAgentInterface& );

		/**
		 * PIDS Agent objct
		 */
		PIDSNamedObject m_pidsAgent;

	};
}

#endif