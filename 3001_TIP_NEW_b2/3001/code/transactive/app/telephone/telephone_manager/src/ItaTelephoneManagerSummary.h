/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/ItaTelephoneManagerSummary.h $
  * @author:  Raymond Pau
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  */


#if !defined(ItaTelephoneManagerSummary_E163BB1B_0D8D_4ba0_B827_F4ACD76641DA__INCLUDED_)
#define ItaTelephoneManagerSummary_E163BB1B_0D8D_4ba0_B827_F4ACD76641DA__INCLUDED_

#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/public_interfaces/ItaAuditing.h"
#include "core/message/src/GenericSubscriberPublisher.h"
#include "core/process_management/public_interfaces/ItaRuntime.h"
#include "bus/security/rights_library/public_interfaces/ItaRights.h"
#include "bus/generic_gui/ItaUserMessage/public_interfaces/ItaUserMessage.h"

class ItaTelephoneManagerSummary : public virtual GenericSubscriberPublisher<TA_Base_Core::CommsMessageCorbaDef>,
                                   public virtual TA_Base_Core::ItaAuditing,
                                   public virtual TA_Base_Core::ItaRuntime,
                                   public virtual TA_Base_Bus::ItaUserMessage
{
public:
    virtual ~ItaTelephoneManagerSummary() {};
};
#endif // !defined(ItaTelephoneManagerSummary_E163BB1B_0D8D_4ba0_B827_F4ACD76641DA__INCLUDED_)
