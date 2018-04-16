/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/banner_pages/call_page_library/src/AbstractCallEntryFactory.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Classes that extend this abstract class are used to create ICallEntry
  * objects. The extending class should listen for events from the system,
  * and inform the CallEntryManager singleton when a Call Entry is created
  * or deleted.
  *
  * For example when a radio call is initiated, the RadioCallEntryFactory
  * creates a RadioCallEntry using new, and passes it to the CallEntryManager's
  * addCallEntryManager(ICallEntry*) method.
  *
  * The CallEntryManager takes responsibility for deleting the object when
  * informed by the RadioCallEntryFactory with the
  * deleteCallEntry(unsigned long callId) method.
  *
  * This means AbstractCallEntryFactory subclasses must be able to retrieve
  * the id for a given call. The saveCallId and retrieveCallId methods may
  * be used to facilitate this.
  */

// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER


#include <sstream>

#include "bus/banner_pages/call_page_library/src/AbstractCallEntryFactory.h"


namespace TA_IRS_Bus
{
    unsigned long AbstractCallEntryFactory::s_uniqueCallEntryId = 1;

    AbstractCallEntryFactory::AbstractCallEntryFactory()
    {
    }


    AbstractCallEntryFactory::~AbstractCallEntryFactory()
    {
    }


    unsigned long AbstractCallEntryFactory::getUniqueCallEntryId()
    {
        unsigned long id = s_uniqueCallEntryId;
        s_uniqueCallEntryId ++;
        return id;
    }


    void AbstractCallEntryFactory::saveCallId(
        const std::string& typeUniqueId,
        unsigned long callEntryId)
    {
        m_callIdMap[typeUniqueId] = callEntryId;
    }


    unsigned long AbstractCallEntryFactory::retrieveCallId(
        const std::string& typeUniqueId)
    {
        return m_callIdMap[typeUniqueId];
    }


    void AbstractCallEntryFactory::deleteCallId(
        const std::string& typeUniqueId)
    {
        m_callIdMap.erase(typeUniqueId);
    }


    bool AbstractCallEntryFactory::doesCallIdExist(
        const std::string& typeUniqueId)
    {
        return m_callIdMap[typeUniqueId] != 0;//( m_callIdMap.find(typeUniqueId) != m_callIdMap.end() );
    }


    std::string AbstractCallEntryFactory::getUnsignedLongAsString(
        unsigned long number)
    {
        std::stringstream stream;
        stream << number;
        return stream.str();
    }


    unsigned long AbstractCallEntryFactory::getStringAsUnsignedLong(
        const std::string& theString)
    {
        unsigned long number;
        std::stringstream stream;
        stream << theString;
        stream >> number;

        return number;
    }


}// TA_IRS_Bus


