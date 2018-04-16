#if !defined(TA_3002_transactive_core_data_access_interface_bas_Subsystem_H)
#define TA_3002_transactive_core_data_access_interface_bas_Subsystem_H
#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

#include <memory>
#include "core/data_access_interface/src/IData.h"

namespace TA_IRS_Core
{
	class SubsystemHelper;
	class Subsystem
	{
	public:

		/**
		* Constructor
		*
		* Construct a Subsystem class based around dataset
		*/
		Subsystem(const unsigned long row, TA_Base_Core::IData& data);

		/**
		* Destructor
		*/
		virtual ~Subsystem();

		/**
		* getKey
		*
		* Returns the key for this Subsystem.
		*
		* @return The key for this Subsystem as an unsigned long.
		*/
		virtual unsigned long getKey();

		/**
		* getName
		*
		* Returns the name of this Subsystem. If this is the first time data for this Location
		* has been requested, all the data is loaded from the database.
		*
		* @return The name for this Subsystem as a std::string
		*
		* @exception DatabaseException A DatabaseException will be thrown if there is a
		*            problem establishing a connection with the database, or if an
		*            error is encountered while retrieving data.
		* @exception DataException A DataException will be thrown if the data cannot be 
		*            converted to the required format (e.g. ACCESSLEVEL should be an 
		*            unsigned long), or if the wrong ammount of data is retrieved.
		*/
		virtual std::string getName();

		virtual std::string getAssetName();

		virtual bool getIsSystem();

		virtual bool getIsMerge();

        virtual unsigned long getParentKey();

		virtual std::string getMergeName();

		virtual unsigned long getOrderID();
		/**
		* invalidate
		*
		* Make the data contained by this Subsystem as invalid. The next call to get...() 
		* following a call to invalidate() will cause all the data to be reloaded from
		* the database.
		*/
		virtual void invalidate();

	private:
		Subsystem& operator=(const Subsystem&);
		Subsystem(const Subsystem&);
	private:
		std::auto_ptr<SubsystemHelper> m_helper;

	};
}
#endif //TA_3002_transactive_core_data_access_interface_bas_Subsystem_H
