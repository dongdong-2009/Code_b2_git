 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/AtsBranchTrack.cpp $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * AtsBranchTrack is an implementation of IAtsBranchTrack. It holds the data specific to an AtsBranchTrack entry
  * in the database, and allows read-only access to that data.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsBranchTrack.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
	//TD14463 - STD has changed track strings to IT and OT
	const std::string AtsBranchTrack::TRACK_INNERBOUND		    = "IT";
	const std::string AtsBranchTrack::TRACK_OUTERBOUND		    = "OT";
    const std::string AtsBranchTrack::TRACK_CENTRAL_INNERBOUND  = "CIT";
	const std::string AtsBranchTrack::TRACK_CENTRAL_OUTERBOUND  = "COT";

    AtsBranchTrack::AtsBranchTrack(const unsigned long key)
    {
        m_AtsBranchTrackHelper = new AtsBranchTrackHelper(key);
    }

    AtsBranchTrack::AtsBranchTrack(unsigned long row, TA_Base_Core::IData& data)
     : m_AtsBranchTrackHelper( new AtsBranchTrackHelper(row, data))
    {
    }

    AtsBranchTrack::~AtsBranchTrack() 
	{
        delete m_AtsBranchTrackHelper;
        m_AtsBranchTrackHelper=NULL;
    }


	std::string AtsBranchTrack::getAtsBranchId()
    {
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
        return m_AtsBranchTrackHelper->getAtsBranchId();
    }


    std::string AtsBranchTrack::getTrack()
    {
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
        return m_AtsBranchTrackHelper->getTrack();
    }

	unsigned long AtsBranchTrack::getKey()
	{
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
        return m_AtsBranchTrackHelper->getKey();
    }

	std::string AtsBranchTrack::getName()
    {
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");

        std::ostringstream name;
		name << m_AtsBranchTrackHelper->getAtsBranchId();
        name << " - ";
		name << m_AtsBranchTrackHelper->getTrack();

        return name.str();
    }


    void AtsBranchTrack::invalidate()
    {
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
        m_AtsBranchTrackHelper->invalidate();
    }

} // closes TA_Core
