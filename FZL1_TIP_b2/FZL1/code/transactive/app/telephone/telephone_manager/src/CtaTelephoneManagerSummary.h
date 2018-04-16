/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/CtaTelephoneManagerSummary.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Refer to 9999Q01020201-Outsourcer_Guideline.doc
  */

#if !defined(CtaTelephoneManagerSummary_04C7C71A_B183_4432_98E9_27736EBEB732__INCLUDED_)
#define CtaTelephoneManagerSummary_04C7C71A_B183_4432_98E9_27736EBEB732__INCLUDED_

#include "core/message/src/CtaAuditing.h"
#include "core/types/public_types/TypeDefs.h"
#include "core/process_management/src/CtaRuntime.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "bus/security/rights_library/src/CtaRights.h"
#include "bus/generic_gui/ItaUserMessage/src/CtaUserMessage.h"
#include "app/telephone/telephone_manager/src/ItaTelephoneManagerSummary.h"
#include "app/telephone/telephone_manager/src/TelephoneDirectoryMessageSubscriber.h"
#include "app/telephone/telephone_manager/src/TelephoneDirectLinesMessageSubscriber.h"
#include "core/synchronisation/src/Semaphore.h"
#include "app/telephone/telephone_manager/src/TelephoneRightsInitializThread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class IEntityData;
}

class CtaTelephoneManagerSummary : public virtual TA_Base_Bus::CtaUserMessage,
                                   public virtual TA_Base_Core::CtaRuntime,
                                   public virtual TA_Base_Core::CtaAuditing,
                                   public virtual TelephoneDirectoryMessageSubscriber,
                                   public virtual TelephoneDirectLinesMessageSubscriber,
                                   public virtual ItaTelephoneManagerSummary,
                                   public virtual TelephoneRightsInitializThreadUserSetRights
{
public:
    CtaTelephoneManagerSummary(TA_Base_Core::IEntityDataPtr agentConfiguration);
    virtual ~CtaTelephoneManagerSummary();

public:
    /**
     * Subscribe
     *
     * This is overriden by the child classs to subscribe to its specialised messages.
     *
     * @return virtual void
     *
     * @exception <exceptions> Optional
     */
    virtual void Subscribe();

    /**
     * ExtractMessageData
     *
     * This is overriden by the child class to extract of the desired data structure
     * from the messageState.
     *
     * @return virtual void
     * @param : const TA_Base_Core::CommsMessageCorbaDef& message
     *
     * @exception <exceptions> Optional
     */
    virtual void ExtractMessageData(const TA_Base_Core::CommsMessageCorbaDef& message);

    /**
     * PublishToListener
     *
     * This is overriden by the child class to publish to the appropriate type of
     * listener.
     *
     * @return virtual void
     * @param : const TA_Base_Core::CommsMessageCorbaDef& message
     * @param : IGenericObserverListener* listener
     *
     * @exception <exceptions> Optional
     */
    virtual void PublishToListener(const TA_Base_Core::CommsMessageCorbaDef& message, IGenericObserverListener* listener);

    virtual void setRights( boost::shared_ptr< TA_Base_Bus::ItaRights > rights ) { m_rights = rights; };
    virtual TA_Base_Bus::ItaRights* getItaRights();
    TA_Base_Core::Semaphore& getSemaphore() { return m_semaphore; };
    TA_Base_Core::Semaphore m_semaphore;
    boost::shared_ptr< TA_Base_Bus::ItaRights > m_rights;
    TA_Base_Core::NonReEntrantThreadLockable m_rightsLock;

};
#endif // !defined(CtaTelephoneManagerSummary_04C7C71A_B183_4432_98E9_27736EBEB732__INCLUDED_)
