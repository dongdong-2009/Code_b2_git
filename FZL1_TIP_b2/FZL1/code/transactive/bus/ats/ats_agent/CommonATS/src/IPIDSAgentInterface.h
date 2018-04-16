
/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  adamr
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Lucky Cuizon$
 *
 */

#ifndef _IPIDS_AGENT_INTERFACE_H_
#define _IPIDS_AGENT_INTERFACE_H_
#include "bus/modbus_comms/src/DataBlock.h"

namespace TA_IRS_Bus
{
	class IPIDSAgentInterface
	{
	public:

		/**
		 * Interface to send SigRequest to PIDS Agent
		 */
		virtual void processPIDSInformation( const TA_Base_Bus::DataBlock<WORD>& data ) = 0;
	};
}
#endif
