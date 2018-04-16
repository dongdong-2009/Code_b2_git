/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/data_access_interface/src/GlobalParameter.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * GlobalParameter is an implementation of IGlobalParameter. It holds the data specific to an GlobalParameter entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/src/GlobalParameter.h"
#include "core/data_access_interface/src/GlobalParameterHelper.h"
#include "core/data_access_interface/src/GlobalParameterAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    GlobalParameter::GlobalParameter( const unsigned long key )
		: m_GlobalParameterHelper( new GlobalParameterHelper(key) )
    {
    }


	GlobalParameter::GlobalParameter(const unsigned long row, TA_Base_Core::IData& data)
		: m_GlobalParameterHelper( new GlobalParameterHelper(row, data) )
    {
    }


    GlobalParameter::~GlobalParameter()
    {
        try
        {
            if (m_GlobalParameterHelper != NULL)
            {
                delete m_GlobalParameterHelper;
                m_GlobalParameterHelper = NULL;
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    unsigned long GlobalParameter::getKey()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getKey();
    }


    std::string GlobalParameter::getName()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getName();
    }


	std::string GlobalParameter::getValue()
	{
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getValue();
	}

	unsigned long GlobalParameter::getSubsystemKey()
	{
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        return m_GlobalParameterHelper->getSubsystemKey();
	}

    void GlobalParameter::invalidate()
    {
        TA_ASSERT(m_GlobalParameterHelper != NULL, "The GlobalParameter helper is NULL");

        m_GlobalParameterHelper->invalidate();
    }

} // closes TA_Base_Core
