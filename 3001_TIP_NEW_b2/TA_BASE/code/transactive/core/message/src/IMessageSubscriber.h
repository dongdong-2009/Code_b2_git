#if !defined(IMessageSubscriber_75CABD6E_6D76_4570_8E21_495A9BDCFD9E__INCLUDED_)
#define IMessageSubscriber_75CABD6E_6D76_4570_8E21_495A9BDCFD9E__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/message/src/IMessageSubscriber.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/12/17 14:00:24 $
  * Last modified by:  $Author: shurui.liu $
  * 
  * IMessageHandler and IMessageFilter interfaces
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/utilities/src/TAAssert.h"

// TODO LPP: Rename this file to IMessageFilter.h

namespace CosNotification
{
    struct StructuredEvent;
};

namespace TA_Base_Core
{
	class IMessageFilter
	{
	public:
		virtual ~IMessageFilter() {}

		/**
		* match
		*
		* true should be returned if the event matches relevant criteria.
        * The entire StructuredEvent is passed to the handler, rather than only passing
        * the payload - this allows the client to access any header fields without 
        * having to duplicate them in the payload.
		*
		* @param    event	
		*/
		virtual bool match( const CosNotification::StructuredEvent& event ) const = 0;
	};

}; // TA_Base_Core


#endif // !defined(IMessageSubscriber_75CABD6E_6D76_4570_8E21_495A9BDCFD9E__INCLUDED_)
