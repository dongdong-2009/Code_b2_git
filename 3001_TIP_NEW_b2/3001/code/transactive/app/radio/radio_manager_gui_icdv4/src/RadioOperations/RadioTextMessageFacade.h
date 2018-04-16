/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioTextMessageFacade.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Responsible for managing all external calls related to Text Messaging. 
 */

#if !defined(AFX_RADIOTEXTMESSAGEFACADE_H__FCC5400D_198D_4A5A_9761_2B5A5BC789F3__INCLUDED_)
#define AFX_RADIOTEXTMESSAGEFACADE_H__FCC5400D_198D_4A5A_9761_2B5A5BC789F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>

#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioTextMessageData.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"


class RadioTextMessageFacade  
{
	public:
		/**
		 * Returns the predefined text message list.
		 */
		static std::vector<PredefinedMessage> getPredefinedMessageList();
		/**
		 * Process the addMessage command. Throws ???
		 */
		static bool addMessage(PredefinedMessage& predefinedMessageData);
		/**
		 * Process the modifyMessage command.
		 */
		static bool modifyMessage(PredefinedMessage& predefinedMessageData);
		/**
		 * Process the removeMessage command.
		 */
		static bool removeMessage(PredefinedMessage& predefinedMessageData);
		
		static bool sendMessage(RadioTextMessageData& messageData,RadioResource& recepient);
		/**
		 * Returns the historical text messages received.
		 */
		static std::vector<MessageInbox> getHistoricalMessages();

		static void updateTextMessage(unsigned long key, bool flag);
		
	private:
        //disable method
		RadioTextMessageFacade();
        RadioTextMessageFacade (const RadioTextMessageFacade& radioObj);
        void operator=(const RadioTextMessageFacade& radioObj);

};

#endif // !defined(AFX_RADIOTEXTMESSAGEFACADE_H__FCC5400D_198D_4A5A_9761_2B5A5BC789F3__INCLUDED_)
