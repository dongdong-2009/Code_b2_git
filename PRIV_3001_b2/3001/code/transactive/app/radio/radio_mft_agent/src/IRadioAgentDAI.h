/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent/src/IRadioAgentDAI.h $
 * @author:  Glen Kidd
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 */

#ifndef IRADIO_AGENT_DAI_H_INCLUDED 
#define IRADIO_AGENT_DAI_H_INCLUDED

#include <string>
#include <list>
#include "app/radio/radio_mft_agent/src/CollectionType.h"

class IRadioAgentDAI
{
public:

    virtual ResourceButtonCollection getResourceButtonCollection(std::string operatorSession) =0;
	virtual PredefinedMessageCollection getPredefinedMessageList(std::string operatorSession) =0;
    virtual void changePredefinedMessage(std::string operatorSession, unsigned long messageKey, std::string messageName, std::string messageText) =0;
    virtual void deletePredefinedMessage(std::string operatorSession, unsigned long messageKey) =0;
	virtual MessageCollection getMessageInbox(std::string operatorSession) =0;
	virtual void associateButtonWithResource(std::string session, RadioResource resource, std::string buttonSet, unsigned int buttonRef) =0;
	virtual void clearButton(std::string session, std::string buttonSet, unsigned int buttonRef) =0;
    virtual void updateEntity(RadioResource resource, std::list<RadioResource> members) =0;
	virtual void deleteEntity(RadioResource resource) =0;

};
#endif // !defined({ADBB199B-3ABA-4984-AF04-59B86D5AA20D}__INCLUDED_)
