#if !defined(IUpdateSubscriber_AB0B4591_92D5_480b_9A3E_FBF5F45DF680__INCLUDED_)
#define IUpdateSubscriber_AB0B4591_92D5_480b_9A3E_FBF5F45DF680__INCLUDED_

/**
  * IUpdateSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/trending/trend_viewer/src/IUpdateSubscriber.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class provides a contract between a Generic Item and an actual Update Subcriber. A Generic
  * Item makes calls on this interface to listen or not listen to messages.
  *
  */

namespace TA_Base_App
{
    //
    // Forward Declaration
    //
    class IDataUpdateObserver;

    //
    // IUpdateSubscriber
    //
    class IUpdateSubscriber
    {

    public:

        /**
          * ~IUpdateSubscriber
          * 
          * Destructor.
          */
        virtual ~IUpdateSubscriber() {}

        /**
          * subscribeToMessages
          * 
          * Subscribe to a message.
          */
        virtual void subscribeToMessages() =0;

        /**
          * unsubscribeToMessages
          * 
          * Unsubscribe to a message.
          */
        virtual void unsubscribeToMessages() =0;

        /**
          * setObserver
          * 
          * Set the data update observer for the subscriber
          *
          * @param dataUpdateObserver - pointer to update observer object
          */
        virtual void setObserver(IDataUpdateObserver* dataUpdateObserver) =0;
    };

}

#endif // !defined(IUpdateSubscriber_AB0B4591_92D5_480b_9A3E_FBF5F45DF680__INCLUDED_)
