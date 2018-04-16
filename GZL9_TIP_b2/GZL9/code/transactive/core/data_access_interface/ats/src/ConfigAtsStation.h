/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/ats/src/Attic/ConfigAtsStation.h,v $
  * @author:  Jiang Haipeng
  * @version: $Revision: 1.1.2.3 $
  *
  * Last modification: $Date: 2006/06/06 10:15:03 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  *
  */

// ConfigAtsStation.h: interface for the ConfigAtsStation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGATSSTATION_H__EB7BF849_DA8E_4ADC_BA47_B4E117D273C7__INCLUDED_)
#define AFX_CONFIGATSSTATION_H__EB7BF849_DA8E_4ADC_BA47_B4E117D273C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IConfigAtsStation.h"
#include "AtsStationHelper.h"

namespace TA_IRS_Core
{
	class ConfigAtsStation: public IConfigAtsStation  
	{
	public:
		ConfigAtsStation();	
		ConfigAtsStation( unsigned long key );
		ConfigAtsStation( unsigned long row, TA_Base_Core::IData& data);
		virtual ~ConfigAtsStation();
		
		virtual void setStationId(const std::string& id);
		virtual void setLocationKey(unsigned long location);
		virtual void deleteThisObject();
		virtual void editThisObject();
		virtual void addObject();
		
		virtual unsigned long getKey();
		virtual std::string getName();
		virtual std::string getStationId();
		virtual unsigned long getLocation();
		void invalidate();
		void updateAtsStationIdChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);

		inline unsigned long getNextAvailableIdentifier()
		{
			m_nextAvailableIdentifier = m_helper->getNextSequenceNumber();
			return m_nextAvailableIdentifier;
		};
		//*******IConfigItem interface************************
		virtual TA_Base_Core::ItemChanges getAllItemChanges();
        virtual bool hasChanged();
        virtual bool isNew();
        virtual void setName(const std::string& name);
        virtual void applyChanges();

		virtual unsigned long getUniqueIdentifier();

	private:

		ConfigAtsStation(ConfigAtsStation&);
		ConfigAtsStation& operator = (ConfigAtsStation&);
	private:
		AtsStationHelper* m_helper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of guis in the
		// Configuration Editor because new guis do not have keys.
		// Therefore we use this method for identification.
		
        long m_nextAvailableIdentifier; // The next unique identifier available
		
		TA_Base_Core::ItemChanges m_atsStationIdChanges; // This is a map of all changes made to the VideoStationId so that they are
		// recorded and can be sent in audit or online update messages.
        static long s_nextAvailableIdentifier; // The next unique identifier available
	};
	
}
#endif // !defined(AFX_CONFIGATSSTATION_H__EB7BF849_DA8E_4ADC_BA47_B4E117D273C7__INCLUDED_)
