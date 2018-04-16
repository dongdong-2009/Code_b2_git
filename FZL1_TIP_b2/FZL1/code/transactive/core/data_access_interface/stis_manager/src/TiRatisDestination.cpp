 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/stis_manager/src/TiRatisDestination.cpp $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * TiRatisDestination is an implementation of ITiRatisDestination. It holds the data specific to an TiRatisDestination entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/stis_manager/src/TiRatisDestination.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    TiRatisDestination::TiRatisDestination(const unsigned long key)
     : m_tiRatisDestinationHelper ( new TiRatisDestinationHelper(key))
    {}


    TiRatisDestination::TiRatisDestination(unsigned long row, TA_Base_Core::IData& data)
     : m_tiRatisDestinationHelper(new TiRatisDestinationHelper(row, data))
    {
    }


    TiRatisDestination::~TiRatisDestination() 
	{
        delete m_tiRatisDestinationHelper;
        m_tiRatisDestinationHelper=NULL;
    }

    unsigned long TiRatisDestination::getKey()
    {
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The TiRatisDestinationHelper pointer is null.");
        return m_tiRatisDestinationHelper->getKey();
    }  
    
    void TiRatisDestination::invalidate()
    {
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The TiRatisDestinationHelper pointer is null.");
        m_tiRatisDestinationHelper->invalidate();
    }

    std::string TiRatisDestination::getName()
    {
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The m_tiRatisDestinationHelper pointer is null.");

        std::ostringstream name;
		name << "RATIS Destination " << m_tiRatisDestinationHelper->getKey();

        return name.str();
    }

    std::string TiRatisDestination::getDestination()
    {
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The TiRatisDestinationHelper pointer is null.");
        return m_tiRatisDestinationHelper->getDestination();
    }    
 
} // closes TA_IRS_Core
