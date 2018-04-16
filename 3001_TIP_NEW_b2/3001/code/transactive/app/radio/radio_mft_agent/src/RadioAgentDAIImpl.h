/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioAgentDAIImpl.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef RADIO_AGENT_DAI_IMPL_H_INCLUDED
#define RADIO_AGENT_DAI_IMPL_H_INCLUDED

#include <string>
#include <vector>
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "core/message/src/CommsMessageSender.h"


class RadioAgentDAIImpl
{

public:
	RadioAgentDAIImpl();
	virtual ~RadioAgentDAIImpl();

public:
    /**
     * Retrieves the collection of buttons that the operator may access,
     * including any profile-based buttons associated with the operator's
     * current profile
     */
    virtual std::vector<ResourceButton> getResourceButtonCollection(const std::string& session);

    /**
     * Get list of predefined messages 
     */
    virtual std::vector<PredefinedMessage> getPredefinedMessageList(const std::string& session);

    /**
     * Add or update the predefined message
     */
    virtual void changePredefinedMessage(const std::string& session, PredefinedMessage& message);

    /**
     * delete the predefined message 
     */
	virtual void deletePredefinedMessage(const std::string& session, unsigned long messageKey);

    /**
     * Returns the requested operator's text messaging inbox. 
     */
    virtual std::vector<MessageInbox> getMessageInbox(const std::string& session);

    /**
     * Associate a button with a resource
     */
	virtual void associateButtonWithResource(const std::string& session, const RadioResource& resource, bool isProfileButton, unsigned int buttonRef);

    /**
     * Remove association
     */
	virtual void clearButton(const std::string& session, const RadioResource& resource, bool isProfileButton, unsigned int buttonRef);

    /**
     * Create a new transactive managed radio resource, or modify an
     * existing one
     */
    virtual void updateEntity(RadioResource& resource, std::vector<RadioResource>& members);

    /**
     * Delete an existing transactive managed radio resource
     */
	virtual void deleteEntity(RadioResource& resource);
	
    /**
     * Method to set the text message received as read
     */
	void textMessageRead(unsigned long key, bool flag);

private:
    //Disabled Methods
    RadioAgentDAIImpl(const RadioAgentDAIImpl&);
    RadioAgentDAIImpl& operator=(const RadioAgentDAIImpl&);

private:
	
	TA_Base_Core::CommsMessageSender* m_commsSender;

    unsigned long m_entityKey;
    unsigned long m_entityLocation;
    unsigned long m_entitySubsystem;

};

#endif // !defined({C9422A19-9052-4d47-8B66-04B0C5BD5779})
