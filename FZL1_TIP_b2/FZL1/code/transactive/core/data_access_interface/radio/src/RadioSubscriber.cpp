/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioSubscriber.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Implementation of the RadioSubscriber class.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/radio/src/RadioSubscriber.h"


namespace TA_IRS_Core
{
    RadioSubscriber::RadioSubscriber(const unsigned long key)
    :   m_radioSubscriberHelper(key)
    {
        FUNCTION_ENTRY("RadioSubscriber ctor()");
        FUNCTION_EXIT;
    }
    
    RadioSubscriber::RadioSubscriber(const unsigned long row, TA_Base_Core::IData& data)
    :   m_radioSubscriberHelper(row, data)
    {
        FUNCTION_ENTRY("RadioSubscriber ctor(const unsigned long, TA_Base_Core::IData&)");
        FUNCTION_EXIT;
    }

    RadioSubscriber::~RadioSubscriber()
    {
        FUNCTION_ENTRY("~RadioSubscriber");
        FUNCTION_EXIT;
    }
    

	char RadioSubscriber::getResourceType()
    {
        FUNCTION_ENTRY("getResourceType");

        FUNCTION_EXIT;
        return m_radioSubscriberHelper.getResourceType();
    }
    
	unsigned long RadioSubscriber::getResourceId()
    {
        FUNCTION_ENTRY("getResourceId");

        FUNCTION_EXIT;
        return m_radioSubscriberHelper.getResourceId();
    }

	unsigned long RadioSubscriber::getSSI()
    {
        FUNCTION_ENTRY("getSSI");

        FUNCTION_EXIT;
        return m_radioSubscriberHelper.getSSI();
    }

    std::string RadioSubscriber::getAlias()
    {
        FUNCTION_ENTRY("getAlias");

        FUNCTION_EXIT;
        return m_radioSubscriberHelper.getAlias();
    }

    unsigned long RadioSubscriber::getKey()
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_radioSubscriberHelper.getKey();
    }


    void RadioSubscriber::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        m_radioSubscriberHelper.invalidate();
        FUNCTION_EXIT;
    }

	std::string RadioSubscriber::getName()
	{
		return m_radioSubscriberHelper.getAlias();
	}
}
