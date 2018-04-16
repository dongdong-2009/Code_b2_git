/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioMFTAgentFramework.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "app/radio/radio_mft_agent/src/RadioMFTAgentFramework.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"

#include "app/radio/radio_manager_gui/src/DebugMsg.h"

RadioMFTAgentFramework::RadioMFTAgentFramework() :
    m_RadioMFTAgentSummary(NULL),
    m_RadioMFTAgentCommandServant(NULL)
{
    FUNCTION_ENTRY("RadioMFTAgentFramework");

    setup();

    FUNCTION_EXIT;
}

RadioMFTAgentFramework::~RadioMFTAgentFramework()
{
    FUNCTION_ENTRY("~RadioMFTAgentFramework");

//TD18785
	cleanup();
//TD18785
	//m_RadioMFTAgent.termination();

    

    FUNCTION_EXIT;
}

void RadioMFTAgentFramework::agentSetMonitor()
{
    //do nothing
}

void RadioMFTAgentFramework::agentTerminate()
{
    //do nothing
}

void RadioMFTAgentFramework::agentSetControl()
{
    //do nothing
}


TA_Base_Bus::IEntity* RadioMFTAgentFramework::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
{
    return NULL;
}

void RadioMFTAgentFramework::notifyGroupOffline(const std::string& group)
{
    //do nothing
}

void RadioMFTAgentFramework::notifyGroupOnline(const std::string& group)
{
    //do nothing
}

void RadioMFTAgentFramework::registerForStateUpdates()
{
    //do nothing
}

void RadioMFTAgentFramework::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
    //do nothing
}

void RadioMFTAgentFramework::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    //do nothing
}



void RadioMFTAgentFramework::setup()
{
    //TA_Base_Core::IEntityData* entity = RadioMFTAgentHelper::getRadioMFTAgentEntity();
	TA_Base_Core::IEntityDataPtr entity(RadioMFTAgentHelper::getRadioMFTAgentEntity());

    m_RadioMFTAgentSummary = new RadioMFTAgentSummary(entity);

    m_RadioMFTAgentCommandServant = new RadioMFTAgentCommandServant(*m_RadioMFTAgentSummary, *entity);
    //delete entity;

    m_RadioMFTAgentCommandServant->start();

    m_RadioMFTAgent.setServant(m_RadioMFTAgentCommandServant);
	m_RadioMFTAgent.initialisation();
}

void RadioMFTAgentFramework::cleanup()
{
    if (m_RadioMFTAgentCommandServant)
    {
        m_RadioMFTAgentCommandServant->stop(); // this will delete the servant
        m_RadioMFTAgentCommandServant = NULL;
    }

    delete m_RadioMFTAgentSummary;
    m_RadioMFTAgentSummary = NULL;

}

void RadioMFTAgentFramework::run()
{

}

