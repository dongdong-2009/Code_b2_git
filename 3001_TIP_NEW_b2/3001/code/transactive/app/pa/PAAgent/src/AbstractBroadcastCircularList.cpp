/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/AbstractBroadcastCircularList.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/06/19 17:59:15 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include <vector>
#include <string>

#include "core/utilities/src/DebugUtil.h"

#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"
#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"
#include "app/pa/PAAgent/src/TrainDVABroadcast.h"


namespace TA_IRS_App
{

AbstractBroadcastCircularList::AbstractBroadcastCircularList()
{
}

AbstractBroadcastCircularList::~AbstractBroadcastCircularList()
{
}

AbstractBroadcast* AbstractBroadcastCircularList::getBroadcast(const std::string& broadcastId)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    std::list<AbstractBroadcast*>::iterator it = m_list.begin();
    for ( ; it != m_list.end(); ++it)
    {
        if ((*it)->getBroadcastId() == broadcastId)
        {
            // Found
            return (*it);
        }
    }

    // Not found
    return NULL;
}

AbstractTrainBroadcast* AbstractBroadcastCircularList::getTrainBroadcast(CORBA::Octet trainId, CORBA::Octet announcementId)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    std::list<AbstractBroadcast*>::iterator it = m_list.begin();
    for ( ; it != m_list.end(); ++it)
    {
        // Ignore all station broadcasts
        AbstractTrainBroadcast* trainBroadcast = dynamic_cast<AbstractTrainBroadcast*>(*it);
        if (trainBroadcast && trainBroadcast->getIsPartOfBroadcast(trainId, announcementId))
        {
            return trainBroadcast;
        }
    }

    // Not found
    return NULL;
}

AbstractTrainBroadcast* AbstractBroadcastCircularList::getTrainBroadcast(CORBA::Octet announcementId)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    std::list<AbstractBroadcast*>::iterator it = m_list.begin();
    for ( ; it != m_list.end(); ++it)
    {
        // Ignore all station broadcasts
        AbstractTrainBroadcast* trainBroadcast = dynamic_cast<AbstractTrainBroadcast*>(*it);
        if (trainBroadcast && (trainBroadcast->getAnnouncementId() == announcementId) )
        {
            return trainBroadcast;
        }
    }

    // Not found
    return NULL;
}


std::list<TrainDVABroadcast*> AbstractBroadcastCircularList::getAllTrainDvaBroadcasts()
{
    std::list<TrainDVABroadcast*> resultList;

    TA_Base_Core::ThreadGuard guard( m_lock );

    std::list<AbstractBroadcast*>::iterator it = m_list.begin();
    for ( ; it != m_list.end(); ++it)
    {
        // Ignore all station broadcasts
        TrainDVABroadcast* trainDvaBroadcast = dynamic_cast<TrainDVABroadcast*>(*it);

        if ( NULL != trainDvaBroadcast )
        {
            resultList.push_back( trainDvaBroadcast );
        }
    }

    return resultList;
}


void AbstractBroadcastCircularList::appendAllBroadcastIdTypes(TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq& broadcastIdTypeSeq)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    unsigned int unStartIndex = broadcastIdTypeSeq.length();
    broadcastIdTypeSeq.length(unStartIndex + m_list.size());

    std::list<AbstractBroadcast*>::iterator it = m_list.begin();
    for (unsigned int i = 0 ; i < m_list.size(); ++i)
    {
        std::string broadcastId = (*it)->getBroadcastId();
        broadcastIdTypeSeq[unStartIndex + i].broadcastId   = CORBA::string_dup(broadcastId.c_str());
        broadcastIdTypeSeq[unStartIndex + i].broadcastType = (*it)->getBroadcastType();
        broadcastIdTypeSeq[unStartIndex + i].sessionId     = (*it)->getSessionId().c_str();

        ++it;
    }
}

void AbstractBroadcastCircularList::clearAllBroadcasts()
{
    TA_Base_Core::ThreadGuard guard( m_lock );

    for ( std::list<AbstractBroadcast*>::iterator itLoop = m_list.begin(); 
        itLoop != m_list.end(); ++itLoop )
    {
        if ( NULL != *itLoop )
        {
            delete *itLoop;
            *itLoop = NULL;
        }
    }

    m_list.clear();
}

bool AbstractBroadcastCircularList::isAnyBroadcastBusy()
{
    TA_Base_Core::ThreadGuard guard( m_lock );

    for ( std::list<AbstractBroadcast*>::iterator it = m_list.begin(); it != m_list.end(); ++it )
    {
        if ( (*it)->isBusy() )
        {
            // Busy
            return true;
        }
    }

    return false;
}

} // namespace TA_IRS_App

