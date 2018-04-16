/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/ItaTelephoneManagerSummary.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
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
