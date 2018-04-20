/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/AbstractBroadcastCircularList.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <vector>
#include <string>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"
#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"
#include "app/pa/PAAgent/src/TrainDVABroadcast.h"
#include "core/synchronisation/src/ThreadGuard.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{

const unsigned char AbstractBroadcastCircularList::MIN_STATION_ANNOUNCE_ID 
    = TA_Base_Bus::IPAAgentCorbaDef::PA_MAXSRCID+1;
const unsigned char AbstractBroadcastCircularList::MAX_STATION_ANNOUNCE_ID = 255;
const unsigned char AbstractBroadcastCircularList::MAX_TRAIN_ANNOUNCEMENT_ID = 255;
const unsigned char AbstractBroadcastCircularList::MIN_TRAIN_ANNOUNCEMENT_ID = 1;

ReEntrantThreadLockable AbstractBroadcastCircularList::m_lockForSingleton;
AbstractBroadcastCircularList* AbstractBroadcastCircularList::m_me = NULL;

AbstractBroadcastCircularList* AbstractBroadcastCircularList::getInstance()
{
	if ( 0 == m_me )
	{
		// Double checking to prevent multiple threads
		// creating multiple instances.

        // Any lock is okay at this stage.
		ThreadGuard guard( m_lockForSingleton );

		if ( 0 == m_me )
		{
			m_me = new AbstractBroadcastCircularList();
    	}
	}

	return m_me;
}

void AbstractBroadcastCircularList::removeInstance( )
{
	// 
	// Guard this to prevent multiple threads atempting
	// to delete/create simultaneously
	//
	ThreadGuard guard( m_lockForSingleton );
	if ( m_me != NULL )
	{
		delete m_me;
		m_me = NULL;
	}
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


void AbstractBroadcastCircularList::getAllBroadcastIdTypes(TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq& broadcastIdTypeSeq)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    broadcastIdTypeSeq.length(m_list.size());
    
    std::list<AbstractBroadcast*>::iterator it = m_list.begin();
    for (int i=0 ; i < m_list.size(); ++i)
    {
        std::string broadcastId = (*it)->getBroadcastId();
        broadcastIdTypeSeq[i].broadcastId   = CORBA::string_dup(broadcastId.c_str());
        broadcastIdTypeSeq[i].broadcastType = (*it)->getBroadcastType();
        broadcastIdTypeSeq[i].sessionId     = (*it)->getSessionId().c_str();

        ++it;
    }
}

bool AbstractBroadcastCircularList::isStationAnnounceIdUsed(CORBA::Octet announceId)
{
    TA_Base_Core::ThreadGuard guard( m_lock );

    // Now check if this is occupied by an existing broadcast

    bool found(false);
    std::list<AbstractBroadcast*>::iterator it = m_list.begin();
    for ( ; it != m_list.end(); ++it)
    {
        // Ignore all train broadcasts
        AbstractStationBroadcast* stationBroadcast = dynamic_cast<AbstractStationBroadcast*>(*it);
        if (stationBroadcast && (announceId == stationBroadcast->getAnnounceId() ))
        {
            return true;
        }
    }

    return false;
}

CORBA::Octet AbstractBroadcastCircularList::getNextAvailableStationAnnounceId() 
    //throw(TA_Base_Core::NoFreeAnnounceIdException)
{
    static CORBA::Octet lastAnnounceId = MAX_STATION_ANNOUNCE_ID;

  	TA_Base_Core::ThreadGuard guard( m_lock );

    for (int count=0; count < MAX_STATION_ANNOUNCE_ID - MIN_STATION_ANNOUNCE_ID + 1; ++count)
    {
        if (lastAnnounceId == MAX_STATION_ANNOUNCE_ID)
        {
            // loop around - cannot use an ID < MIN_ANNOUNCE_ID as these are 
            // reserved for the sources.
            lastAnnounceId = MIN_STATION_ANNOUNCE_ID;
        }
        else
        {
            ++lastAnnounceId;
        }
    
        // Now check if this is occupied by an existing broadcast
        if (!isStationAnnounceIdUsed(lastAnnounceId))
        {
            // done - found an unused announceId
            return lastAnnounceId;
        }

    } // for loop

    TA_THROW(TA_Base_Core::NoFreeAnnounceIdException());

    return 0;
}

CORBA::Octet AbstractBroadcastCircularList::getNextAvailableTrainAnnouncementId()
    //throw(TA_Base_Core::NoFreeAnnounceIdException)
{
  	TA_Base_Core::ThreadGuard guard( m_lock );

    static CORBA::Octet lastAnnouncementId = MAX_TRAIN_ANNOUNCEMENT_ID;

    for (int count=0; count <= MAX_TRAIN_ANNOUNCEMENT_ID; ++count)
    {
        if (lastAnnouncementId == MAX_TRAIN_ANNOUNCEMENT_ID)
        {
            // loop around - cannot use an ID=0 < MIN_ANNOUNCE_ID as this is 
            // reserved to indicate a train broadcast that hasn't been executed.
            lastAnnouncementId = MIN_TRAIN_ANNOUNCEMENT_ID;
        }
        else
        {
            ++lastAnnouncementId;
        }
    
        // Now check if this is occupied by an existing broadcast

        bool found(false);
        std::list<AbstractBroadcast*>::iterator it = m_list.begin();
        for ( ; it != m_list.end(); ++it)
        {
            // Ignore all station broadcasts
            AbstractTrainBroadcast* trainBroadcast = dynamic_cast<AbstractTrainBroadcast*>(*it);
            if (trainBroadcast && (lastAnnouncementId == trainBroadcast->getAnnouncementId() ))
            {
                found = true;
                break; // for loop
            }
        }

        if (!found)
        {
            // done - found an unused announceId for the trainId
            return lastAnnouncementId;
        }

    } // for loop

    TA_THROW(TA_Base_Core::NoFreeAnnounceIdException());

    return 0;
}


} // namespace TA_IRS_App

