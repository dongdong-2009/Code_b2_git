#if !defined(TA_3002_transactive_core_data_access_interface_bas_ModeHelper_H)
#define TA_3002_transactive_core_data_access_interface_bas_ModeHelper_H
#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

#include <vector>
#include "core/data_access_interface/src/IData.h"


namespace TA_IRS_Core
{
	class ModeHelper
	{
	public:
		
		ModeHelper(const unsigned long row, TA_Base_Core::IData& data);

		virtual unsigned long getKey();

		virtual std::string getName();

		virtual unsigned int getValue();

		virtual std::string getDescription();

		virtual unsigned int getLocationKey();

		virtual unsigned int getSubsystemKey();

		virtual unsigned int getTypeKey();

		virtual std::string getLocationName();

		virtual std::string getSubsystemName();

		virtual std::string getTypeName();

		virtual void invalidate();

	private:

		ModeHelper(const ModeHelper&);
		ModeHelper& operator=(const ModeHelper&);

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

		/**
		* reload()
		*
		* This method reloads the data from the database. It is called when a get... 
		* or set... method is called and the data state is not valid.
		*
		* pre: Either - this entity was initially loaded from the database
		*      OR     - writeEntityData() has already been called
		*/
		void reload();

		/**
		* reloadUsing
		*
		* Fills out the local members with data contained in the input data object
		*
		* @param : const unsigned long row - the row to query in the data object
		* @param : TA_Base_Core::IData& data - the IData interface that should 
		*          have been obtained using the getBasicQueryData function
		*         
		* @exception  DatabaseException 
		*             - if there is a problem establishing a connection with the database.
		*             - if an error is encountered while retrieving data.
		* @exception  DataException
		*             - if the data cannot be converted to the required format 
		*             - NO_VALUE if the record cannot be found for the helper 
		*             constructed with a key.
		*             - NOT_UNIQUE if the primary key returns multiple records 
		*/
		void reloadUsing(const unsigned long row, TA_Base_Core::IData& data);

		unsigned long m_key;
		std::string m_name;
		unsigned int m_value;
		std::string m_description;
		unsigned int m_locationKey;
		std::string m_locationName;
		unsigned int m_subsystemKey;
		std::string m_subsystemName;
		unsigned int m_typeKey;
		std::string m_typeName;
		bool m_isValidData;
	};
}

#endif //TA_3002_transactive_core_data_access_interface_bas_ModeHelper_H