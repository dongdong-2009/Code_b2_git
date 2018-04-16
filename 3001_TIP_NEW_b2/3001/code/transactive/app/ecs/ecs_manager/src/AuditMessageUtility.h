/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/AuditMessageUtility.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#if !defined(AFX_AUDITMESSAGEUTILITY_H__9AF46370_92E7_4F9B_95C2_0804FEDF57B9__INCLUDED_)
#define AFX_AUDITMESSAGEUTILITY_H__9AF46370_92E7_4F9B_95C2_0804FEDF57B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/message/types/ECSAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"

// wen ching++
// bug 30 (TD12345)
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
// ++wen ching
// bug 30 (TD12345)


#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"

#include "bus/application_types/src/apptypes.h"

using namespace TA_IRS_App;
using namespace TA_Base_Core;

namespace TA_IRS_App
{
	class CAuditMessageUtility  
	{
	public:
		void LogParameterChangeAuditMessage(std::string &aParameterName, std::string &aParameterNewValue, const MessageType &aMessageType);
		void LogTimetableChangeAuditMessage(std::string &aStationName, std::string &aTimetableNewValue, const MessageType &aMessageType);
			
		CAuditMessageUtility();
		virtual ~CAuditMessageUtility();

	private:
		AuditMessageSender* m_auditMsgSender;

		// wen ching++
		// bug 30 (TD12345)
		IEntityData* m_ecsManager;
		// ++wen ching
		// bug 30 (TD12345)
		

	};

} // namespace

#endif // !defined(AFX_AUDITMESSAGEUTILITY_H__9AF46370_92E7_4F9B_95C2_0804FEDF57B9__INCLUDED_)
