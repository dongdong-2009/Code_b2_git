/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/ats/src/Attic/AtsStationAccessFactory.h,v $
  * @author:  Jiang Haipeng
  * @version: $Revision: 1.1.2.5 $
  *
  * Last modification: $Date: 2006/06/06 10:15:02 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  *
  */



#ifndef ATSSTATIONACCESSFACTORY_H
#define ATSSTATIONACCESSFACTORY_H

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "IAtsStation.h"
#include "IConfigAtsStation.h"
#include "ace/Singleton.h"
 

#include <vector>	


namespace TA_IRS_Core
{
	class AtsStationAccessFactory
	{
	public:
		AtsStationAccessFactory();
		virtual ~AtsStationAccessFactory();
		
	public:
		static AtsStationAccessFactory& getInstance();
		void removeInstance();
		std::vector<IAtsStation *> getAllStations();
		void getAllStations(std::vector<IConfigAtsStation*> & configStation);
		IConfigAtsStation * createStation(unsigned long locationID, const std::string & stationID);
		IConfigAtsStation * createStation();
		IConfigAtsStation * createStation(const std::string & stationID);
		IConfigAtsStation * editStation(unsigned long key, unsigned long locationID, const std::string & stationID);
		IConfigAtsStation * deleteStation(unsigned long key);
		bool locationIdExists(unsigned long locationID, unsigned long pkey);
		bool stationIdExists(const std::string & stationID, unsigned long pkey);
	private:
		AtsStationAccessFactory(AtsStationAccessFactory&);
		AtsStationAccessFactory & operator = (AtsStationAccessFactory&);
		static AtsStationAccessFactory* m_instance;
	};
	typedef ACE_Singleton<AtsStationAccessFactory, ACE_Null_Mutex>  The_AtsStationAccessFactory;
		
	
}
#endif // !defined({E031A822-2693-4341-9A50-46A4CD9C32E8}__INCLUDED_)