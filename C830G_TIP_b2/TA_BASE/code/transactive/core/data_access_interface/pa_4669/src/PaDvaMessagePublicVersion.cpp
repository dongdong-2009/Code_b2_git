 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessagePublicVersion.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaDvaMessagePublicVersion is an implementation of IPaDvaMessagePublicVersion. It holds the data specific to an PaDvaMessagePublicVersion entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa_4669/src/PaDvaMessagePublicVersion.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    PaDvaMessagePublicVersion::PaDvaMessagePublicVersion(const unsigned long key)
     : m_paDvaMessagePublicVersionHelper ( new PaDvaMessagePublicVersionHelper(key))
    {}


    PaDvaMessagePublicVersion::PaDvaMessagePublicVersion(unsigned long row, TA_Base_Core::IData& data)
     : m_paDvaMessagePublicVersionHelper(new PaDvaMessagePublicVersionHelper(row, data))
    {
    }


    PaDvaMessagePublicVersion::~PaDvaMessagePublicVersion() 
	{
        delete m_paDvaMessagePublicVersionHelper;
        m_paDvaMessagePublicVersionHelper=NULL;
    }


    unsigned long PaDvaMessagePublicVersion::getKey()
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
        return m_paDvaMessagePublicVersionHelper->getKey();
    }


    unsigned long PaDvaMessagePublicVersion::getLocationKey()
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
        return m_paDvaMessagePublicVersionHelper->getLocationKey();
    }    
    

    std::string PaDvaMessagePublicVersion::getMessageVersion(
                                            IPaDvaMessagePublicVersion::ELocation location,
                                            unsigned long stationDvaMessageId)
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
        return m_paDvaMessagePublicVersionHelper->getMessageVersion(location, stationDvaMessageId);
    }


    std::string PaDvaMessagePublicVersion::getVersionData(EVersionColumn column)
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
        return m_paDvaMessagePublicVersionHelper->getVersionData(column);
    }


    void PaDvaMessagePublicVersion::invalidate()
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
        m_paDvaMessagePublicVersionHelper->invalidate();
    }


    std::string PaDvaMessagePublicVersion::getName()
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The m_paDvaMessagePublicVersionHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message Version " << m_paDvaMessagePublicVersionHelper->getKey();

        return name.str();
    }

} // closes TA_Base_Core
