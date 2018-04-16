#if !defined(TA_3002_transactive_core_data_access_interface_bas_SubsystemHelper_H)
#define TA_3002_transactive_core_data_access_interface_bas_SubsystemHelper_H
#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

#include <string>
#include <vector>

#include "core/data_access_interface/src/IData.h"

namespace TA_IRS_Core
{
	class SubsystemHelper
	{

	public:

		/**
		* SubsystemHelper
		* 
		* This constructor creates a new object using the input data
		* which is representative of a row returned from SQL statement
		* 
		* @param : const unsigned long row
		*          - the row of data in the data object that we should collect data from
		*          in order to construct ourselves
		* @param : TA_Base_Core::IData& data
		*          - the IData interface that should have been obtained using the 
		*          getBasicQueryData function
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
		SubsystemHelper(const unsigned long row, TA_Base_Core::IData& data);

		/**
		* getKey
		*
		* Returns the key for this Station.
		*
		* @return The key for this Station as an unsigned long.
		*/
		virtual unsigned long getKey();

		/**
		* getName
		*
		* Returns the name of this Station. If this is the first time data for this Station
		* has been requested, all the data is loaded from the database.
		*
		* @return The name for this Station as a std::string
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

		virtual std::string getMergeName();

        virtual unsigned long getParentKey();

		virtual unsigned long getOrderID();
		/**
		* invalidate
		*
		* Make the data contained by this Location as invalid. The next call to get...() 
		* following a call to invalidate() will cause all the data to be reloaded from
		* the database.
		*/
		virtual void invalidate();

	private:

		SubsystemHelper(const SubsystemHelper&);
		SubsystemHelper& operator=(const SubsystemHelper&);

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

		static bool stringToBoolean(const std::string& boolean);
private:
		unsigned long m_key;
		std::string m_name;
		std::string m_assetName;
		bool m_isSystem;
		bool m_isValidData;
		bool m_isMerge;
        std::string m_mergeName;
        unsigned long m_ulParentKey;
		unsigned long m_orderID;
	};
}
#endif //TA_3002_transactive_core_data_access_interface_bas_SubsystemHelper_H
