/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/DvaVersionChangeSubscriber.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * DvaVersionChangeSubscriber subscriber to listen out for changes to
  * DVA version changes (for the DVA Versions tab)
  *
  */

#if !defined(DvaVersionChangeSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
#define DvaVersionChangeSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_

#include "bus/pa/messagesubscriber/src/GenericMessageSubscriber.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"


class DvaVersionChangeSubscriber 
: 
public TA_IRS_Bus::GenericMessageSubscriber<TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords>
{
public:
    
    /**
     * DvaVersionChangeSubscriber(constructor)
     */
    DvaVersionChangeSubscriber();

    /**
     * getMessageType (interface implementation)
     *
     * @see GenericMessageSubscriber::getMessageType for more details
     *
     */
    virtual const TA_Base_Core::MessageType& getMessageType() const;

protected:

    /**
     * getNotificationData (interface implementation)
     *
     * @see GenericMessageSubscriber::getNotificationData
     *
     */
    virtual TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords getNotificationData(
                    const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords& packetData) { return packetData; }    

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
    virtual void onPreProcessMessage(const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords& updateData);
    
    /**
     * onPostProcessMessage (interface implementation)
     *
     * @see GenericMessageSubscriber::onPostProcessMessage for more details
     *
     */
    virtual void onPostProcessMessage(const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords& updateData);
};


#endif // !defined(DvaVersionChangeSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
