/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/online_printing_agent/src/PrintableEventData.cpp $
 * @author:  Cameron Rochester
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * MmSchedulingData is an implementation of IMmSchedulingData. It holds the data specific to 
 * alarm plan associations.
 */



#include <sstream>
#include <string>

#include "core/data_access_interface/online_printing_agent/src/PrintableEventData.h"
#include "core/data_access_interface/online_printing_agent/src/PrintableEventHelper.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{


	PrintableEventData::PrintableEventData( unsigned long pkey,std::string printer,
                            std::string alarmflag, std::string createtime,
                            std::string severity, std::string value,
                            std::string asset,std::string description ,
                            std::string mms,std::string dss,
                            std::string avalanche,std::string operatorname,
                            std::string alarmcomment,std::string alm,
                            unsigned long locationkey)
							:m_helper(0)
	{
		FUNCTION_ENTRY("PrintableEventData");

		m_helper = new PrintableEventHelper( pkey, printer, alarmflag, createtime, severity,
			                                 value, asset, description, mms, dss, avalanche, 
											 operatorname, alarmcomment, alm, locationkey
										   );

		FUNCTION_EXIT;
	}


	PrintableEventData::~PrintableEventData()
	{
		FUNCTION_ENTRY("~PrintableEventData");

		if(0 != m_helper)
		{
			delete m_helper;
		}

		FUNCTION_EXIT;
	}
	


	unsigned long PrintableEventData::getPkey()
    {
        FUNCTION_ENTRY("getPkey");

		return m_helper->getPkey();

        FUNCTION_EXIT;
    }
	
	std::string PrintableEventData::getPrinter()
    {
        FUNCTION_ENTRY("getPrinter");
		
        return m_helper->getPrinter();

        FUNCTION_EXIT;
    }
	
	std::string PrintableEventData::getAlarmflag()
    {
        FUNCTION_ENTRY("getAlarmflag");
		
        return m_helper->getAlarmflag();
  
        FUNCTION_EXIT;
    }
	
	std::string PrintableEventData::getCreatetime()
    {
        FUNCTION_ENTRY("getCreatetime");
		
        return m_helper->getCreatetime();

        FUNCTION_EXIT;
		
		
    }
	
	std::string PrintableEventData::getSeverity()
    {
        FUNCTION_ENTRY("getSeverity");
		
        return m_helper->getSeverity();

        FUNCTION_EXIT;
		
		
    }
	
	std::string PrintableEventData::getAsset()
    {
        FUNCTION_ENTRY("getAsset");
		
         return m_helper->getAsset();

        FUNCTION_EXIT;
		
		
    }
	
	
	std::string PrintableEventData::getDescription()
    {
        FUNCTION_ENTRY("getDescription");
		
         return m_helper->getDescription();

        FUNCTION_EXIT;
		
		
    }
	
	
	std::string PrintableEventData::getValue()
    {
        FUNCTION_ENTRY("getValue");
		
        return m_helper->getValue();

        FUNCTION_EXIT;
		
		
    }
	
	
	std::string PrintableEventData::getAvalanche()
    {
        FUNCTION_ENTRY("getAvalanche");
		
        return m_helper->getAvalanche();
        FUNCTION_EXIT;
       
		
    }
	
	
	std::string PrintableEventData::getMMS()
    {
        FUNCTION_ENTRY("getMMS");
		
        return m_helper->getMMS();
        FUNCTION_EXIT;
        
		
    }
	
	
	std::string PrintableEventData::getDSS()
    {
        FUNCTION_ENTRY("getDSS");

		return m_helper->getDSS();
		
        FUNCTION_EXIT;
 
		
    }
	
	
	std::string PrintableEventData::getOperatorname()
    {
        FUNCTION_ENTRY("getOperatorname");
		
        return m_helper->getOperatorname();

        FUNCTION_EXIT;

		
		
    }

	std::string PrintableEventData::getAlarm_Comment()
    {
        FUNCTION_ENTRY("getAlarm_Comment");
		
        return m_helper->getAlarm_Comment();

        FUNCTION_EXIT;
 
		
		
    }


	std::string PrintableEventData::getAlm()
    {
        FUNCTION_ENTRY("getAlm");
		
        return m_helper->getAlm();
		
        FUNCTION_EXIT;
       
    }


	unsigned long PrintableEventData::getLocation_Key()
    {
        FUNCTION_ENTRY("getLocation_Key");
		
        return m_helper->getLocation_Key();

        FUNCTION_EXIT;
        
		
		
    }




	void PrintableEventData::invalidate()
	{
		FUNCTION_ENTRY("invalidate");

		m_helper->invalidate();

		FUNCTION_EXIT;
	}


} // closes TA_IRS_Core


