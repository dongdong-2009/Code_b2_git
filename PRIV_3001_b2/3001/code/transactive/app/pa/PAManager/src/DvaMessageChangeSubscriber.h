/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/DvaMessageChangeSubscriber.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * DvaMessageChangeSubscriber subscriber to listen out for changes to
  * DVA message changes
  *
  */

#if !defined(DvaMessageChangeSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
#define DvaMessageChangeSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_

#include "bus/pa/messagesubscriber/src/GenericMessageSubscriber.h"
#pragma warning (disable : 4250)
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#pragma warning (default : 4250)
#include "bus/pa/cachedmaps/src/ICachedMapObserver.h"


class DvaMessageChangeSubscriber 
: 
public TA_IRS_Bus::GenericMessageSubscriber
<TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData, DvaMessageUpdateData >,
public TA_IRS_Bus::ICachedMapObserver
{
public:

    typedef TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData DvaMessagesUpdateData;

    /**
     * DvaMessageChangeSubscriber(constructor)
     */
    DvaMessageChangeSubscriber();

    /**
     * DvaMessageChangeSubscriber (destructor)
     */
    virtual ~DvaMessageChangeSubscriber();

    /**
     * getMessageType (interface implementation)
     *
     * @see GenericMessageSubscriber::getMessageType for more details
     *
     */
    virtual const TA_Base_Core::MessageType& getMessageType() const;


    /**
     * onCachedMapChange (interface implementation)
     *
     * @see ICachedMapObserver::onCachedMapChange for more details
     *
     */
    virtual void onCachedMapChanged(unsigned long pKey, bool changesMade);


protected:

    /**
     * updateMessageQueue (interface override)
     *
     * Custom implementation required to handle insertion in message queue
     *
     * @see GenericMessageSubscriber::updateMessageQueue for more details
     *
     */
    virtual void updateMessageQueue(TA_IRS_Bus::UpdateBuffer<DvaMessageUpdateData>& msgQueue,
                                        const std::set<HWND>& windowHwnds,
                                        const DvaMessageUpdateData& updateData) const;


    /**
     * getNotificationData (interface implementation)
     *
     * @see GenericMessageSubscriber::getNotificationData
     *
     */
    virtual DvaMessageUpdateData getNotificationData(
                        const DvaMessagesUpdateData& packetData);
    
    /**
     * onSubscriptionSuccessful (interface implementation)
     *
     * @see GenericMessageSubscriber::onSubscriptionSuccessful for more details
     *
     */
    virtual void onSubscriptionSuccessful();

    /**
     * onPreProcessMessage (interface implementation)
     *
     * @see GenericMessageSubscriber::onPreProcessMessage for more details
     *
     */
    virtual void onPreProcessMessage(const DvaMessagesUpdateData& updateData);
    
    /**
     * onPostProcessMessage (interface implementation)
     *
     * @see GenericMessageSubscriber::onPostProcessMessage for more details
     *
     */
    virtual void onPostProcessMessage(const DvaMessagesUpdateData& updateData);

private:

    /**
     * resetCachedMapDvaMessageChangeObserverRegistration
     *
     * Resets observer registration with the Station Dva Message Cached Map
     *
     * @param subscribe true to register ourself as an observer with cached map
     *              (if not done already), false to deregister (if not already)
     *
     */
    void resetCachedMapDvaMessageChangeObserverRegistration(bool subscribe);

    // Lock on accesses to m_receiveTypeWatch or m_receiveLabelWatch
    TA_Base_Core::NonReEntrantThreadLockable         m_receiveCheckLock;

    bool    m_subscribedToCachedMapChanges;
};


#endif // !defined(DvaMessageChangeSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
