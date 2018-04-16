/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/ats/src/Attic/AtsStationHelper.h,v $
  * @author:  Jiang Haipeng
  * @version: $Revision: 1.1.2.3 $
  *
  * Last modification: $Date: 2006/05/31 02:00:27 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  *
  */

// AtsStationHelper.h: interface for the AtsStationHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSSTATIONHELPER_H__94DD8B8C_C7C2_412A_9AC9_242EB62BCDD0__INCLUDED_)
#define AFX_ATSSTATIONHELPER_H__94DD8B8C_C7C2_412A_9AC9_242EB62BCDD0__INCLUDED_

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif


#include "IAtsStation.h"

#include <string>

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_IRS_Core
{
	class AtsStationHelper 
	{
	public:
		AtsStationHelper(unsigned long key ); 
		AtsStationHelper(const unsigned long row, TA_Base_Core::IData& data);
		AtsStationHelper(unsigned long location, const std::string& stationID);
		virtual ~AtsStationHelper();

		virtual unsigned long getKey();
		virtual std::string getName();
		virtual std::string getStationId();
		virtual unsigned long getLocation();
		void setStationId(const std::string& id);
		void setLocationKey(unsigned long location);
		void deleteObject();
		void save(bool insert); //insert or update
		void invalidate();
		unsigned long getNextSequenceNumber();
		//*******IConfigItem interface************************
        virtual bool isNew();

	public:
		static const std::string STATION_ID_COL;
		static const std::string KEY_COL;
		static const std::string LOCATION_COL;
		static const std::string ATSSTATION_TBL;
	private:
        bool            m_isNew;
        bool            m_isValidData;
		
		unsigned long m_key; 

		std::string m_stationID; 

		unsigned long m_location; 
		
	private:
		void reload();
		void reloadUsing(const unsigned long row, TA_Base_Core::IData& data);	
		void insert();
		void update();
		bool isRowDeleted(unsigned long key);
		
	private:
		AtsStationHelper();
		AtsStationHelper(AtsStationHelper &);	
		AtsStationHelper & operator = (AtsStationHelper&);
	};
}
#endif // !defined(AFX_ATSSTATIONHELPER_H__94DD8B8C_C7C2_412A_9AC9_242EB62BCDD0__INCLUDED_)
