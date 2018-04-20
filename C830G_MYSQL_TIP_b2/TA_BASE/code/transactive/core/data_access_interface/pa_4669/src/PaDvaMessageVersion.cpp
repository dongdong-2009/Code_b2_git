 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessageVersion.cpp $
  * @author Jade Lee
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2013/09/26 14:59:27 $
  * Last modified by: $Author: huangjian $
  * 
  * PaDvaMessageVersion is an implementation of IPaDvaMessageVersion. It holds the data specific to an PaDvaMessageVersion entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa_4669/src/PaDvaMessageVersion.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    PaDvaMessageVersion::PaDvaMessageVersion(const unsigned long key)
     : m_paDvaMessageVersionHelper ( new PaDvaMessageVersionHelper(key))
    {}


    PaDvaMessageVersion::PaDvaMessageVersion(unsigned long row, TA_Base_Core::IData& data)
     : m_paDvaMessageVersionHelper(new PaDvaMessageVersionHelper(row, data))
    {
    }


    PaDvaMessageVersion::~PaDvaMessageVersion() 
	{
        delete m_paDvaMessageVersionHelper;
        m_paDvaMessageVersionHelper=NULL;
    }


    unsigned long PaDvaMessageVersion::getKey()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getKey();
    }


    unsigned long PaDvaMessageVersion::getLocationKey()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getLocationKey();
    }    
    

    std::string PaDvaMessageVersion::getVersionData()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getVersionData();
    }


    void PaDvaMessageVersion::invalidate()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        m_paDvaMessageVersionHelper->invalidate();
    }


    std::string PaDvaMessageVersion::getName()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The m_paDvaMessageVersionHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message Version " << m_paDvaMessageVersionHelper->getKey();

        return name.str();
    }

} // closes TA_Base_Core
