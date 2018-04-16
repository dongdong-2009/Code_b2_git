/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsFixedBlockMap.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * AtsFixedBlockMap is an implementation of IAtsFixedBlockMap. It holds the data specific to an AtsFixedBlockMap entry
  * in the database, and allows read-only access to that data.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsFixedBlockMap.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace
{
    static const unsigned long HEAD_CAR_CONVERSION_FACTOR = 64;
}

namespace TA_IRS_Core
{

    AtsFixedBlockMap::AtsFixedBlockMap(BranchZoneMap branchZoneMap)
        : m_branchZoneMap(branchZoneMap)
    {
    }


    double AtsFixedBlockMap::getZoneId(unsigned long branchId, unsigned long fixBlockAbscissa, unsigned long headCarAbscissa)
    {
		return getAtsFbMapRecord(branchId, fixBlockAbscissa, headCarAbscissa)->getZoneId();
    }


	std::string AtsFixedBlockMap::getTrack(unsigned long branchId, unsigned long fixBlockAbscissa, unsigned long headCarAbscissa)
    {
		return getAtsFbMapRecord(branchId, fixBlockAbscissa, headCarAbscissa)->getTrack();
    }


	TA_IRS_Core::IAtsFbMapRecord* AtsFixedBlockMap::getAtsFbMapRecord(unsigned long branchId, unsigned long fixBlockAbscissa, unsigned long headCarAbscissa)
	{
		if (m_branchZoneMap.find(branchId) == m_branchZoneMap.end())
        {
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"cannot find branchId=%d",branchId);
            TA_THROW(TA_Base_Core::DataException("No such branch in this zone type", TA_Base_Core::DataException::NO_VALUE, ""));
        }

        IAtsFbMapRecords& branchDetailVector = m_branchZoneMap[branchId];
        
        // the fixBlockAbscissa is in quantums of .125m, and the configuration should also be in terms of this
        // the headCarAbscissa is an offset FROM THE fixBlockAbscissa in quantums of 8m
        // so to convert the headCarAbscissa to the same scale as the fixBlockAbscissa it needs to be multipled by 8x8=64
        unsigned long abscissa = fixBlockAbscissa + ( headCarAbscissa * HEAD_CAR_CONVERSION_FACTOR );
		//unsigned long abscissa = fixBlockAbscissa + headCarAbscissa ;			//for testing
        
		for (unsigned int i = 0; i < branchDetailVector.size(); i++)
        {
            // check if the absolute abscissa is within the given range
            if ( ( branchDetailVector[i]->getMinAbscissa() <= abscissa ) &&
                 ( branchDetailVector[i]->getMaxAbscissa() >= abscissa ) )
            {
                return branchDetailVector[i];
            }
        }
        TA_THROW(TA_Base_Core::DataException("Abscissa not found in any zone along this branch", TA_Base_Core::DataException::NO_VALUE, ""));
	}


    AtsFixedBlockMap::~AtsFixedBlockMap()
    {
    }
} // closes TA_Core
