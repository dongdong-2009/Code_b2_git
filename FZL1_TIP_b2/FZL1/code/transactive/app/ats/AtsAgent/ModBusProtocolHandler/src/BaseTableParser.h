/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\app\ats\AtsAgent\ModBusProtocolHandler\src\BaseTableParser.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * The base class for all Table parser that must be extended for specific table parsing purposes.
  * E.g PATableParser, TrainTableParser, PIDSTableParser
  */

#ifndef _BASE_TABLE_PARSER_H_
#define	_BASE_TABLE_PARSER_H_

#include "bus/ats/ats_agent/CommonATS/src/IProtocolParser.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"

namespace TA_IRS_Bus
{
	class IAtsDataNotification;
}
namespace TA_IRS_App
{
	class BaseTableParser  : public TA_IRS_Bus::IProtocolParser
	{
	public:
		/**
		 * Constructor
		 */
		BaseTableParser(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		/**
		 * Destructor
		 */
		virtual ~BaseTableParser();

		/**
		 * setAtsDataNotification
		 *
		 * Method to use to set Data Notification object
		 *
		 * @param atsDataNofic : Data notification object
		 */
		void setAtsDataNotification(TA_IRS_Bus::IAtsDataNotification* atsDataNofic);		
	private:
		/**
		 * Disable default, copy and assignment constructor
		 */
		BaseTableParser(const BaseTableParser&);
		BaseTableParser& operator= (const BaseTableParser&);

	protected:
		TA_IRS_Bus::IAtsDataNotification* m_atsNotification;
		TA_Base_Core::AtsAgentEntityDataPtr m_AtsEntityData;
	};
}
#endif