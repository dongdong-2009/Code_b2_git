/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/ats/src/Attic/IConfigAtsStation.h,v $
  * @author:  Jiang Haipeng
  * @version: $Revision: 1.1.2.5 $
  *
  * Last modification: $Date: 2006/06/06 10:15:03 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  *
  */



#if !defined(ICONFIGATSSTATION_H)
#define ICONFIGATSSTATION_H

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "IAtsStation.h"

#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{
	class IConfigAtsStation : public virtual IAtsStation, public virtual TA_Base_Core::IConfigItem
	{
	public:
		virtual void setStationId(const std::string& id) =0;
		virtual void setLocationKey(unsigned long location) =0;
		virtual void deleteThisObject() =0;
		virtual void editThisObject() =0;
		virtual void addObject() =0;
        virtual unsigned long getUniqueIdentifier() = 0;

		//*******IConfigItem interface************************
        virtual TA_Base_Core::ItemChanges getAllItemChanges() = 0;

        virtual bool hasChanged() =0;

        virtual bool isNew() =0;

        virtual void setName(const std::string& name) =0;

        virtual void applyChanges() =0;
	};

	typedef std::vector<IConfigAtsStation*> IConfigAtsStations;
}
#endif // !defined({BAFC7FED-2240-45bd-BB7B-0922128B183B}__INCLUDED_)