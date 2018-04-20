 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersion.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaDvaMessagePrivateVersion is an implementation of IPaDvaMessagePrivateVersion. It holds the data specific to an PaDvaMessagePrivateVersion entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersion.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    PaDvaMessagePrivateVersion::PaDvaMessagePrivateVersion(const unsigned long key)
     : m_paDvaMessagePrivateVersionHelper ( new PaDvaMessagePrivateVersionHelper(key))
    {}


    PaDvaMessagePrivateVersion::PaDvaMessagePrivateVersion(unsigned long row, TA_Base_Core::IData& data)
     : m_paDvaMessagePrivateVersionHelper(new PaDvaMessagePrivateVersionHelper(row, data))
    {
    }


    PaDvaMessagePrivateVersion::~PaDvaMessagePrivateVersion() 
	{
        delete m_paDvaMessagePrivateVersionHelper;
        m_paDvaMessagePrivateVersionHelper=NULL;
    }


    unsigned long PaDvaMessagePrivateVersion::getKey()
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
        return m_paDvaMessagePrivateVersionHelper->getKey();
    }


    unsigned long PaDvaMessagePrivateVersion::getLocationKey()
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
        return m_paDvaMessagePrivateVersionHelper->getLocationKey();
    }    
    

    std::string PaDvaMessagePrivateVersion::getMessageVersion(
                                            IPaDvaMessagePrivateVersion::ELocation location,
                                            unsigned long stationDvaMessageId)
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
        return m_paDvaMessagePrivateVersionHelper->getMessageVersion(location, stationDvaMessageId);
    }


    std::string PaDvaMessagePrivateVersion::getVersionData(EVersionColumn column)
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
        return m_paDvaMessagePrivateVersionHelper->getVersionData(column);
    }


    void PaDvaMessagePrivateVersion::invalidate()
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
        m_paDvaMessagePrivateVersionHelper->invalidate();
    }


    std::string PaDvaMessagePrivateVersion::getName()
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The m_paDvaMessagePrivateVersionHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message Version " << m_paDvaMessagePrivateVersionHelper->getKey();

        return name.str();
    }

} // closes TA_Base_Core
