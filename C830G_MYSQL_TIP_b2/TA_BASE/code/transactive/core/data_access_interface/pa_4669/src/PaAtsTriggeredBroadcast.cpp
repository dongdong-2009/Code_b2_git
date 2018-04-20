 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaAtsTriggeredBroadcast.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaAtsTriggeredBroadcast is an implementation of IPaAtsTriggeredBroadcast. It holds the data specific to an PaAtsTriggeredBroadcast entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa_4669/src/PaAtsTriggeredBroadcast.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    PaAtsTriggeredBroadcast::PaAtsTriggeredBroadcast(const unsigned long key)
     : m_paAtsTriggeredBroadcastHelper ( new PaAtsTriggeredBroadcastHelper(key))
    {}


    PaAtsTriggeredBroadcast::PaAtsTriggeredBroadcast(unsigned long row, TA_Base_Core::IData& data)
     : m_paAtsTriggeredBroadcastHelper(new PaAtsTriggeredBroadcastHelper(row, data))
    {
    }


    PaAtsTriggeredBroadcast::~PaAtsTriggeredBroadcast() 
	{
        delete m_paAtsTriggeredBroadcastHelper;
        m_paAtsTriggeredBroadcastHelper=NULL;
    }

    unsigned long PaAtsTriggeredBroadcast::getKey()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getKey();
    }

    unsigned long PaAtsTriggeredBroadcast::getAtsEventType()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getAtsEventType();
    }


    unsigned long PaAtsTriggeredBroadcast::getLocationKey()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getLocationKey();
    }
    

    unsigned long PaAtsTriggeredBroadcast::getDestinationLocationKey()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getDestinationLocationKey();
    }


    unsigned long PaAtsTriggeredBroadcast::getPlatformId()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getPlatformId();
    }


    bool          PaAtsTriggeredBroadcast::getIsLastTrain()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getIsLastTrain();
    }


    unsigned long PaAtsTriggeredBroadcast::getPaZoneGroupKey()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getPaZoneGroupKey();
    }


    unsigned long PaAtsTriggeredBroadcast::getPaDvaMessageKey()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getPaDvaMessageKey();
    }


    bool          PaAtsTriggeredBroadcast::getIsEnabledParameter()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getIsEnabledParameter();
    }


    std::string   PaAtsTriggeredBroadcast::getDvaMessageVersion()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        return m_paAtsTriggeredBroadcastHelper->getDvaMessageVersion();
    }


    void PaAtsTriggeredBroadcast::invalidate()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The PaAtsTriggeredBroadcastHelper pointer is null.");
        m_paAtsTriggeredBroadcastHelper->invalidate();
    }

    std::string PaAtsTriggeredBroadcast::getName()
    {
        TA_ASSERT(m_paAtsTriggeredBroadcastHelper != NULL,"The m_paAtsTriggeredBroadcastHelper pointer is null.");

        std::ostringstream name;
		name << "PA ATS Triggered Broadcast " << m_paAtsTriggeredBroadcastHelper->getKey();

        return name.str();
    }

} // closes TA_Base_Core
