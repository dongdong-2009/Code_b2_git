/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/ats/src/Attic/AtsStation.cpp,v $
  * @author:  Jiang Haipeng
  * @version: $Revision: 1.1.2.2 $
  *
  * Last modification: $Date: 2006/06/06 10:15:02 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  *
  */

// AtsStation.cpp: implementation of the AtsStation class.
//
//////////////////////////////////////////////////////////////////////

#include "AtsStation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_Core
{
	
	AtsStation::AtsStation(const unsigned long row, TA_Base_Core::IData& data): m_helper(row, data)
	{
		
	}
	
	unsigned long AtsStation::getKey()
	{
		return m_helper.getKey();
	}
	
	std::string AtsStation::getName()
	{
		return m_helper.getName();
	}
	
	std::string AtsStation::getStationId()
	{
		return m_helper.getStationId();
	}
	
	unsigned long AtsStation::getLocation()
	{
		return m_helper.getLocation();	
	}
	void AtsStation::invalidate()
	{
		m_helper.invalidate();
	}
}