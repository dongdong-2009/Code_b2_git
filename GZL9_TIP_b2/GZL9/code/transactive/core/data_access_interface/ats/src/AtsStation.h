/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/ats/src/Attic/AtsStation.h,v $
  * @author:  Jiang Haipeng
  * @version: $Revision: 1.1.2.2 $
  *
  * Last modification: $Date: 2006/06/06 10:15:02 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  *
  */

// AtsStation.h: interface for the AtsStation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSSTATION_H__4A594FFC_2AC0_4724_AFEB_63E7F563E3A4__INCLUDED_)
#define AFX_ATSSTATION_H__4A594FFC_2AC0_4724_AFEB_63E7F563E3A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "IAtsStation.h"
#include "AtsStationHelper.h"

#include <string>

namespace TA_IRS_Core
{
	class AtsStation : public IAtsStation  
	{
	public:
		AtsStation(const unsigned long row, TA_Base_Core::IData& data);
		virtual ~AtsStation(){};

		virtual unsigned long getKey();
		virtual std::string getName();
		virtual std::string getStationId();
		virtual unsigned long getLocation();

		virtual void invalidate();
	private:
		AtsStation();
		AtsStation(AtsStation &);
		AtsStation & operator = (AtsStation &);
	private:
		AtsStationHelper m_helper;
	};
}
#endif // !defined(AFX_ATSSTATION_H__4A594FFC_2AC0_4724_AFEB_63E7F563E3A4__INCLUDED_)
