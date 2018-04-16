/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubAlarmSeverityData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */

#ifndef STUBALARMSEVERITYDATA_H_
#define STUBALARMSEVERITYDATA_H_

#include "core/data_access_interface/src/IAlarmSeverityData.h"

namespace TA_IRS_App_Test
{
    class StubAlarmSeverityData : public TA_Base_Core::IAlarmSeverityData
    {
        public:
		    StubAlarmSeverityData( const unsigned long key, std::string name)
		        : m_key(key), 
                  m_name(name)
		    {
		    }
		
		    ~StubAlarmSeverityData()
		    {
		    }
		
		    unsigned long getKey()
		    {
		        return m_key;
		    }
		
		    std::string getName()
		    {
		        return m_name;
		    }
		
		    unsigned long getSeverityColourKey( EColourType type  )
		    {
		        return 142;
		    }
		
		    time_t getDateCreated()
		    {
		        return 0;
		    }
		
		    time_t getDateModified()
		    {
		        return 0;
		    }
		
		    void invalidate()
		    {
		    }        

    private:
        unsigned long m_key;
        std::string m_name;
    };
}
#endif /*STUBALARMSEVERITYDATA_H_*/
