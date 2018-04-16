/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioManager/src/IRadioAgentMessageListener.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#if !defined(AFX_IRADIOAGENTMESSAGELISTENER_H__2A410388_FD2B_46D3_B3EA_74CC6B73C820__INCLUDED_)
#define AFX_IRADIOAGENTMESSAGELISTENER_H__2A410388_FD2B_46D3_B3EA_74CC6B73C820__INCLUDED_



#include "core/message/src/IGenericObserverListener.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

class IRadioAgentMessageListener : public IGenericObserverListener  
{
	public:

		virtual ~IRadioAgentMessageListener();

		/**
		 * This must be implemented by the realised class and will
		 * receive notifications of the SDS message
		 */
		virtual void onRecvSDS(RadioResource& src, std::string& message) = 0;

};

#endif 
