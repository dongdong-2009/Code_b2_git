/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\IProtocolManager.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This interface will be implemented by the Protocol Manager.
  */

#ifndef _I_PROTOCOL_MANAGER_H_
#define _I_PROTOCOL_MANAGER_H_

#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_IRS_Bus
{
	class IProtocolParser;
	class IDataProcessor;

	class IProtocolManager 
	{
	public:
		/**
		 * registerProtocolParser
		 *
		 * Method used to register protocol parser
		 * 
		 * @param type           : ATS Message type
		 * @param protocolParser : Protocol parser object
		 */
		virtual void registerProtocolParser(AtsMessageType type,IProtocolParser* protocolParser) = 0;

		/**
		 * deRegisterProtocolParser
		 *
		 * Method used to de register protocol parser
		 * 
		 * @param type : ATS Message type
		 */
		virtual void deRegisterProtocolParser(AtsMessageType type) = 0;

		/**
		 * setAtsDataNotification
		 *
		 * Method used to set the Data notitification object
		 * 
		 * @param atsDataNofic : ATS Data notification object
		 */
		virtual void setAtsDataNotification(IAtsDataNotification* atsDataNofic) = 0;

		/**
		 * setOperationMode
		 *
		 * Method used to set the operation mode
		 * 
		 * @param eMode : eMode
		 */
		virtual void setOperationMode(TA_Base_Core::EOperationMode eMode) = 0;
	};
}
#endif