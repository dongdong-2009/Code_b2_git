/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/ats/src/Attic/IAtsStation.h,v $
  * @author:  Jiang Haipeng
  * @version: $Revision: 1.1.2.6 $
  *
  * Last modification: $Date: 2006/06/06 10:15:03 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  *
  */



#if !defined(IATSSTATION_H)
#define IATSSTATION_H

#include <string>
#include <vector>

#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
	class IAtsStation: public virtual TA_Base_Core::IItem
	{
	public:
		virtual std::string getName() = 0;
		virtual std::string getStationId() = 0;
		virtual unsigned long getKey() = 0;
		virtual unsigned long getLocation() = 0;
		virtual void invalidate() = 0;
		virtual ~IAtsStation(){};
	};

	typedef std::vector<IAtsStation*> IAtsStations;
    typedef IAtsStations::iterator    IAtsStationsIt;
}
#endif // !defined({9D58965A-9AA3-4c81-90A7-1B980A03389E}__INCLUDED_)