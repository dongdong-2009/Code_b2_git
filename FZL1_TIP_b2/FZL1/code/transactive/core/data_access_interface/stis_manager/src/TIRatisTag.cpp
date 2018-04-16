 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/stis_manager/src/TIRatisTag.cpp $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * TiRatisTag is an implementation of ITiRatisTag. It holds the data specific to an TiRatisTag entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/stis_manager/src/TiRatisTag.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
    TiRatisTag::TiRatisTag(const unsigned long key)
     : m_tiRatisTagHelper ( new TiRatisTagHelper(key))
    {}


    TiRatisTag::TiRatisTag(unsigned long row, TA_Base_Core::IData& data)
     : m_tiRatisTagHelper(new TiRatisTagHelper(row, data))
    {
    }


    TiRatisTag::~TiRatisTag() 
	{
        delete m_tiRatisTagHelper;
        m_tiRatisTagHelper = NULL;
    }

    unsigned long TiRatisTag::getKey()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        return m_tiRatisTagHelper->getKey();
    }  
    
    void TiRatisTag::invalidate()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        m_tiRatisTagHelper->invalidate();
    }

    std::string TiRatisTag::getName()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");

        std::ostringstream name;
		name << "Ratis Tag " << m_tiRatisTagHelper->getKey();

        return name.str();
    }

    std::string TiRatisTag::getTagGroup()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        return m_tiRatisTagHelper->getTagGroup();
    }    
    
    std::string TiRatisTag::getDetail()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        return m_tiRatisTagHelper->getDetail();
    }    

	//RatisTag is the combination of "TagGroup" and "Detail";
    std::string TiRatisTag::getRatisTag()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        return m_tiRatisTagHelper->getRatisTag();
    }    

} // closes TA_IRS_Core
